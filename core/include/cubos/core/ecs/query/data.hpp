/// @file
/// @brief Class @ref cubos::core::ecs::QueryData.
/// @ingroup core-ecs-query

#pragma once

#include <tuple>
#include <unordered_set>
#include <utility>

#include <cubos/core/ecs/query/fetcher.hpp>
#include <cubos/core/ecs/query/filter.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

namespace cubos::core::ecs
{
    /// @brief Holds the data necessary to execute a query.
    ///
    /// The argument types of the query specify exactly what data will be accessed during iteration. For example, the
    /// query might have been constructed with properties such that a given column must be present on the entity. That
    /// doesn't mean that the data will actually end up being accessed.
    ///
    /// If an argument type is present which requires a certain column to be present, then that column is automatically
    /// added to the query properties, if it isn't there already.
    ///
    /// @tparam Ts Argument types.
    /// @ingroup core-ecs-query
    template <typename... Ts>
    class QueryData
    {
    public:
        /// @brief Can be iterated to view the query matches.
        class View;

        ~QueryData()
        {
            delete mFetchers;
            delete mFilter;
        }

        /// @brief Constructs.
        /// @param world World being queried.
        /// @param extraTerms Extra query terms.
        QueryData(World& world, const std::vector<QueryTerm>& extraTerms)
            : mWorld{world}
        {
            for (auto& archetype : mPreparedArchetypes)
            {
                archetype = ArchetypeId::Invalid;
            }

            for (auto& depth : mPreparedDepths)
            {
                depth = -1;
            }

            // Extract terms from the query argument types.
            std::vector<QueryTerm> argumentTerms = {QueryFetcher<Ts>::term(world)...};

            // Fill the access set with the query argument types.
            for (const auto& term : argumentTerms)
            {
                if (!term.isEntity())
                {
                    mAccesses.insert(term.type);
                }
            }

            // Resolve them with the received extra terms.
            auto terms = QueryTerm::resolve(world.types(), extraTerms, argumentTerms);

            // Initialize the filter.
            mFilter = new QueryFilter(world, terms);

            // Initialize the fetchers with a templated functor which receives a sequence of indices (at compile time)
            // and initializes each fetcher for the argument term with the corresponding index.
            auto initFetchers = [&]<std::size_t... Is>(std::index_sequence<Is...>)
            {
                return new std::tuple<QueryFetcher<Ts>...>(QueryFetcher<Ts>(world, argumentTerms[Is])...);
            };

            // Call the above functor with a sequence of indices from 0 to the number of fetchers.
            mFetchers = initFetchers(std::index_sequence_for<Ts...>{});

            // Find the cursor for each fetcher.
            for (std::size_t argI = 0; argI < argumentTerms.size(); ++argI)
            {
                // First check how many argument terms are equal to the current argument term.
                // We will have to skip that many resolved terms later.
                std::size_t skipCount = 0;
                for (std::size_t i = 0; i < argI; ++i)
                {
                    if (argumentTerms[i].compare(world.types(), argumentTerms[argI]))
                    {
                        ++skipCount;
                    }
                }

                // Find the actual index of the term in the resolved vector.
                std::size_t termI;
                for (termI = 0; termI < terms.size(); ++termI)
                {
                    if (terms[termI].compare(world.types(), argumentTerms[argI]))
                    {
                        // We need to skip the terms respective to earlier equal terms.
                        if (skipCount == 0)
                        {
                            break;
                        }

                        --skipCount;
                    }
                }

                CUBOS_ASSERT(termI < terms.size(),
                             "QueryTerm::resolve should put the argument term somewhere in the resulting terms vector");

                mFetcherCursors.emplace_back(mFilter->cursorIndex(termI));
            }
        }

        /// @brief Move constructs.
        /// @param other Other query data.
        QueryData(QueryData&& other) noexcept
            : mWorld{other.mWorld}
            , mFilter{other.mFilter}
            , mAccesses{std::move(other.mAccesses)}
            , mFetchers{other.mFetchers}
            , mFetcherCursors{std::move(other.mFetcherCursors)}
        {
            for (int i = 0; i < QueryNode::MaxTargetCount; ++i)
            {
                mPreparedArchetypes[i] = other.mPreparedArchetypes[i];
                mPreparedDepths[i] = other.mPreparedDepths[i];
            }

            other.mFilter = nullptr;
            other.mFetchers = nullptr;
        }

        /// @brief Gets a set with the data types accessed by this query.
        /// @return Data type set.
        const std::unordered_set<DataTypeId, DataTypeIdHash>& accesses() const
        {
            return mAccesses;
        }

        /// @brief Fetches any new matching archetypes that have been added since the last call to this function.
        void update()
        {
            mFilter->update();
        }

        /// @brief Returns a view which can be used to iterate over the matches.
        /// @return View.
        View view()
        {
            return {*this, mFilter->view()};
        }

        /// @brief Accesses the match for the given entity, if there is one.
        /// @param entity Entity.
        /// @return Requested data, or nothing if the entity does not match the query.
        memory::Opt<std::tuple<Ts...>> at(Entity entity)
        {
            CUBOS_ASSERT(mFilter->targetCount() == 1);

            auto view = this->view().pin(0, entity);

            if (view.begin() == view.end())
            {
                return {};
            }

            return *view.begin();
        }

        /// @brief Accesses the match for the given entities, if there is one.
        /// @param firstEntity Entity for the first target.
        /// @param secondEntity Entity for the second target.
        /// @return Requested data, or nothing if the entities do not match the query.
        memory::Opt<std::tuple<Ts...>> at(Entity firstEntity, Entity secondEntity)
        {
            CUBOS_ASSERT(mFilter->targetCount() == 2);

            auto view = this->view().pin(0, firstEntity).pin(1, secondEntity);

            if (view.begin() == view.end())
            {
                return {};
            }

            return *view.begin();
        }

    private:
        /// @brief If necessary, prepares the fetchers for iteration over the given archetypes.
        /// @param archetypes Target archetypes.
        /// @param cursorDepths Cursor depths.
        void prepare(const ArchetypeId* archetypes, const int* cursorDepths)
        {
            // Check if anything changed at all, and if any of the archetypes is empty.
            bool changed = false;
            bool empty = false;
            for (int target = 0; target < mFilter->targetCount(); ++target)
            {
                if (mPreparedArchetypes[target] != archetypes[target])
                {
                    mPreparedArchetypes[target] = archetypes[target];
                    changed = true;
                }

                if (!mWorld.tables().dense().contains(archetypes[target]))
                {
                    empty = true;
                }
            }

            for (auto cursor : mFetcherCursors)
            {
                if (mPreparedDepths[cursor] != cursorDepths[cursor])
                {
                    mPreparedDepths[cursor] = cursorDepths[cursor];
                    changed = true;
                }
            }

            if (changed && !empty)
            {
                // Templated functor which receives a sequence of indices (at compile time) and prepares the
                // corresponding fetcher with each cursor depth.
                auto prepareAll = [&]<std::size_t... Is>(std::index_sequence<Is...>)
                {
                    (std::get<Is>(*mFetchers).prepare(archetypes, cursorDepths[mFetcherCursors[Is]]), ...);
                };

                // Call the above functor with a sequence of indices from 0 to the number of fetchers.
                // This will expand to a call to prepare() for each fetcher.
                prepareAll(std::index_sequence_for<Ts...>{});
            }
        }

        World& mWorld;
        QueryFilter* mFilter;

        std::unordered_set<DataTypeId, DataTypeIdHash> mAccesses;
        std::tuple<QueryFetcher<Ts>...>* mFetchers;
        std::vector<size_t> mFetcherCursors;
        ArchetypeId mPreparedArchetypes[QueryNode::MaxTargetCount];
        int mPreparedDepths[QueryNode::MaxCursorCount];
    };

    template <typename... Ts>
    class QueryData<Ts...>::View
    {
    public:
        /// @brief Used to iterate over the query matches.
        class Iterator;

        /// @brief Constructs.
        /// @param data Query data.
        /// @param view Query filter view.
        View(QueryData& data, QueryFilter::View view)
            : mData{data}
            , mView{view}
        {
        }

        /// @brief Copy constructs.
        /// @param view Other view.
        View(const View& view) = default;

        /// @brief Returns a new view equal to this one but with the given target pinned to the given entity.
        ///
        /// Effectively this filters out all matches where the given target isn't the given entity.
        ///
        /// @param target Target index.
        /// @param entity Entity.
        /// @return View.
        View pin(int target, Entity entity)
        {
            return {mData, mView.pin(target, entity)};
        }

        /// @brief Returns an iterator pointing to the first query match.
        /// @return Iterator.
        Iterator begin()
        {
            return {mData, mView.begin()};
        }

        /// @brief Returns an out of bounds iterator representing the end of the query matches.
        /// @return Iterator.
        Iterator end()
        {
            return {mData, mView.end()};
        }

        /// @brief Gets a reference to the underlying query data.
        /// @return Query data.
        QueryData& data()
        {
            return mData;
        }

    private:
        QueryData& mData;
        QueryFilter::View mView;
    };

    template <typename... Ts>
    class QueryData<Ts...>::View::Iterator
    {
    public:
        /// @brief Output structure of the iterator.
        using Match = std::tuple<Ts...>;

        /// @brief Constructs.
        /// @param data Query data.
        /// @param iterator Query filter iterator.
        Iterator(QueryData& data, QueryFilter::View::Iterator iterator)
            : mData{data}
            , mIterator{iterator}
        {
        }

        /// @brief Copy constructs.
        /// @param other Other iterator.
        Iterator(const Iterator& other) = default;

        /// @brief Compares two iterators.
        /// @param other Other iterator.
        /// @return Whether the iterators point to the same match.
        bool operator==(const Iterator& other) const
        {
            return &mData == &other.mData && mIterator == other.mIterator;
        }

        /// @brief Accesses the match referenced by this iterator.
        /// @note Aborts if out of bounds.
        /// @return Match.
        Match operator*() const
        {
            mData.prepare(mIterator.targetArchetypes(), mIterator.cursorDepths());
            const auto* cursorRows = mIterator.cursorRows();

            // Templated functor which receives a sequence of indices (at compile time) and fetches the corresponding
            // data from the tuple of fetchers with each index.
            auto fetchAll = [&]<std::size_t... Is>(std::index_sequence<Is...>)
            {
                return std::tuple<Ts...>(
                    std::get<Is>(*mData.mFetchers).fetch(cursorRows[mData.mFetcherCursors[Is]])...);
            };

            // Call the above functor with a sequence of indices from 0 to the number of fetchers.
            // This will expand to a call to fetch() for each fetcher.
            return fetchAll(std::index_sequence_for<Ts...>{});
        }

        /// @brief Advances the iterator.
        /// @note Aborts if out of bounds.
        /// @return Reference to this.
        Iterator& operator++()
        {
            ++mIterator;
            return *this;
        }

    private:
        QueryData& mData;
        QueryFilter::View::Iterator mIterator;
    };
} // namespace cubos::core::ecs
