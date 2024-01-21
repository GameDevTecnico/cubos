/// @file
/// @brief Class @ref cubos::core::ecs::QueryData.
/// @ingroup core-ecs-query

#pragma once

#include <tuple>
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
            for (int i = 0; i < QueryFilter::MaxTargetCount; ++i)
            {
                mPreparedArchetypes[i] = ArchetypeId::Invalid;
            }

            // Extract terms from the query argument types.
            std::vector<QueryTerm> argumentTerms = {QueryFetcher<Ts>::term(world)...};

            // Resolve them with the received extra terms.
            auto terms = QueryTerm::resolve(world.types(), extraTerms, argumentTerms);

            // Initialize the filter.
            mFilter = new QueryFilter(world, terms);

            // Initialize the fetchers.
            int i = 0;
            auto getTerm = [&]() { return argumentTerms[i++]; };
            mFetchers = new std::tuple<QueryFetcher<Ts>...>{QueryFetcher<Ts>(world, getTerm())...};
            (void)getTerm; // Necessary to silence unused warning in case Ts is empty.

            // Find the cursor for each fetcher.
            for (std::size_t argI = 0; argI < argumentTerms.size(); ++argI)
            {
                // Find the actual index of the term in the resolved vector.
                std::size_t termI;
                for (termI = 0; termI < terms.size(); ++termI)
                {
                    if (terms[termI].compare(world.types(), argumentTerms[termI]))
                    {
                        break;
                    }
                }

                CUBOS_ASSERT(termI < terms.size(),
                             "QueryTerm::resolve should put the argument term somewhere in the resulting terms vector");

                mFetcherCursors.emplace_back(mFilter->cursorIndex(termI));
            }
        }

        /// @brief Move constructs.
        /// @param other Other query data.
        QueryData(QueryData&& other)
            : mWorld{other.mWorld}
            , mFilter{other.mFilter}
            , mFetchers{other.mFetchers}
            , mFetcherCursors{other.mFetcherCursors}
        {
            for (int i = 0; i < QueryFilter::MaxTargetCount; ++i)
            {
                mPreparedArchetypes[i] = other.mPreparedArchetypes[i];
            }

            other.mFilter = nullptr;
            other.mFetchers = nullptr;
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
        /// @return Requested components, or nothing if the entity does not match the query.
        Opt<std::tuple<Ts...>> at(Entity entity)
        {
            auto view = this->view().pin(0, entity);

            if (view.begin() == view.end())
            {
                return {};
            }

            return *view.begin();
        }

    private:
        /// @brief If necessary, prepares the fetchers for iteration over the given archetypes.
        /// @param archetypes Target archetypes.
        void prepare(const ArchetypeId* archetypes)
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

                if (!mWorld.tables().contains(archetypes[target]))
                {
                    empty = true;
                }
            }

            if (changed && !empty)
            {
                std::apply([archetypes](auto&&... fetchers) { (fetchers.prepare(archetypes), ...); }, *mFetchers);
            }
        }

        World& mWorld;
        QueryFilter* mFilter;

        std::tuple<QueryFetcher<Ts>...>* mFetchers;
        std::vector<size_t> mFetcherCursors;
        ArchetypeId mPreparedArchetypes[QueryFilter::MaxTargetCount];
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

        /// @brief Returns a new view equal to this one but with the given target pinned to the given entity.
        ///
        /// Effectively this filters out all matches where the given target isn't the given entity.
        ///
        /// @param target Target index.
        /// @param entity Entity.
        /// @return View.
        View pin(int target, Entity entity) &&
        {
            mView = std::move(mView).pin(target, entity);
            return *this;
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
            mData.prepare(mIterator.targetArchetypes());
            auto* cursorRows = mIterator.cursorRows();

            int i = 0;
            auto fetch = [&]<typename T>(QueryFetcher<T>& fetcher) -> T {
                return fetcher.fetch(cursorRows[mData.mFetcherCursors[i++]]);
            };

            return std::apply(
                [&fetch](auto&&... fetchers) { return std::tuple<Ts...>(fetch.template operator()<Ts>(fetchers)...); },
                *mData.mFetchers);
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
