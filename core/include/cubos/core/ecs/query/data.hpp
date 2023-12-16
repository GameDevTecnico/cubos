/// @file
/// @brief Class @ref cubos::core::ecs::QueryData.
/// @ingroup core-ecs-query

#include <tuple>

#include <cubos/core/ecs/query/fetcher.hpp>
#include <cubos/core/ecs/query/properties.hpp>
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
        /// @brief Used to iterate over the query matches.
        class Iterator;

        /// @brief Constructs.
        /// @param properties Query properties.
        QueryData(World& world, QueryProperties properties)
            : mWorld{world}
            , mQueryFetchers{QueryFetcher<Ts>(world)...}
            , mProperties{std::move(properties)}
        {
            // Analyze the access patterns of each query argument.
            std::apply([this](auto&&... fetchers) { (fetchers.analyze(mAccess), ...); }, mQueryFetchers);

            // Add missing column requirements from the query arguments.
            for (auto dataTypeInner : mAccess.dataTypes)
            {
                DataTypeId dataType{dataTypeInner};
                bool missing = true;

                for (auto columnId : mProperties.withColumns)
                {
                    if (columnId.dataType() == dataType)
                    {
                        missing = false;
                        break;
                    }
                }

                if (missing)
                {
                    mProperties.withColumns.emplace_back(DenseColumnId::make(dataType));
                }
            }

            // Find the archetype which contains only the required components.
            for (auto columnId : mProperties.withColumns)
            {
                CUBOS_ASSERT(!mWorld.archetypeGraph().contains(mBaseArchetype, columnId),
                             "Query properties must not have duplicate column ids");
                mBaseArchetype = mWorld.archetypeGraph().with(mBaseArchetype, columnId);
            }

            this->update();
        }

        /// @brief Move constructs.
        /// @param other Other query data.
        QueryData(QueryData&& other) noexcept = default;

        /// @brief Fetches any new matching archetypes that have been added since the last call to this function.
        void update()
        {
            // Collect all archetypes which have at least the required columns.
            std::vector<ArchetypeId> newArchetypes;
            mSeenArchetypes = mWorld.archetypeGraph().collect(mBaseArchetype, newArchetypes, mSeenArchetypes);
            mArchetypes.reserve(mArchetypes.size() + newArchetypes.size());

            // Filter out archetypes which have any of the excluded columns.
            for (auto archetype : newArchetypes)
            {
                bool matches = true;

                for (auto columnId : mProperties.withoutColumns)
                {
                    if (mWorld.archetypeGraph().contains(archetype, columnId))
                    {
                        matches = false;
                        break;
                    }
                }

                if (matches)
                {
                    mArchetypes.emplace_back(archetype);
                }
            }
        }

        /// @brief Returns an iterator pointing to the first query match.
        /// @return Iterator.
        Iterator begin()
        {
            return {*this, 0, 0};
        }

        /// @brief Returns an out of bounds iterator representing the end of the query matches.
        /// @return Iterator.
        Iterator end()
        {
            return {*this, mArchetypes.size(), 0};
        }

        /// @brief Accesses the match for the given entity, if there is one.
        /// @param entity Entity.
        /// @return Requested components, or nothing if the entity does not match the query.
        Opt<std::tuple<Ts...>> at(Entity entity)
        {
            // Get the archetype of the entity.
            auto archetype = mWorld.archetype(entity);

            // Check if it matches the query at all.
            bool matches = false;
            for (auto matchArchetype : mArchetypes)
            {
                if (matchArchetype == archetype)
                {
                    matches = true;
                    break;
                }
            }

            if (!matches)
            {
                return {};
            }

            // If it matches, just fetch the components.
            this->prepare(archetype);
            auto row = mWorld.tables().dense(archetype).row(entity.index);
            return std::apply([row](auto&&... fetchers) { return std::tuple<Ts...>(fetchers.fetch(row)...); },
                              mQueryFetchers);
        }

    private:
        /// @brief If necessary, prepares for iteration over the given archetype.
        void prepare(ArchetypeId archetype)
        {
            if (mPreparedArchetype != archetype)
            {
                mPreparedArchetype = archetype;

                // Only prepare the fetchers if the archetype actually contains anything.
                if (mWorld.tables().contains(archetype))
                {
                    std::apply([archetype](auto&&... fetchers) { (fetchers.prepare(archetype), ...); }, mQueryFetchers);
                }
            }
        }

        World& mWorld;

        ArchetypeId mBaseArchetype{ArchetypeId::Empty};
        std::vector<ArchetypeId> mArchetypes;
        std::size_t mSeenArchetypes{0};

        std::tuple<QueryFetcher<Ts>...> mQueryFetchers;
        ArchetypeId mPreparedArchetype{ArchetypeId::Invalid};

        QueryAccess mAccess;
        QueryProperties mProperties;
    };

    template <typename... Ts>
    class QueryData<Ts...>::Iterator
    {
    public:
        /// @brief Output structure of the iterator.
        using Match = std::tuple<Ts...>;

        /// @brief Constructs.
        /// @param data Query data.
        /// @param archetypeIndex Archetype index.
        /// @param row Archetype's dense table row.
        Iterator(QueryData& data, std::size_t archetypeIndex, std::size_t row)
            : mData{data}
            , mArchetypeIndex{archetypeIndex}
            , mRow{row}
        {
            this->findArchetype();
        }

        /// @brief Copy constructs.
        /// @param other Other iterator.
        Iterator(const Iterator& other) = default;

        /// @brief Compares two iterators.
        /// @param other Other iterator.
        /// @return Whether the iterators point to the same match.
        bool operator==(const Iterator& other) const
        {
            return &mData == &other.mData && mArchetypeIndex == other.mArchetypeIndex && mRow == other.mRow;
        }

        /// @brief Accesses the match referenced by this iterator.
        /// @note Aborts if out of bounds.
        /// @return Match.
        Match operator*() const
        {
            CUBOS_ASSERT(mArchetypeIndex < mData.mArchetypes.size(), "Iterator out of bounds");

            mData.prepare(mData.mArchetypes[mArchetypeIndex]);
            return std::apply([this](auto&&... fetchers) { return std::tuple<Ts...>(fetchers.fetch(mRow)...); },
                              mData.mQueryFetchers);
        }

        /// @brief Advances the iterator.
        /// @note Aborts if out of bounds.
        /// @return Reference to this.
        Iterator& operator++()
        {
            CUBOS_ASSERT(mArchetypeIndex < mData.mArchetypes.size(), "Iterator out of bounds");

            ++mRow;
            this->findArchetype();
            return *this;
        }

    private:
        /// @brief Advances the iterator's archetype until a non-empty one is found.
        void findArchetype()
        {
            while (mArchetypeIndex < mData.mArchetypes.size() &&
                   (!mData.mWorld.tables().contains(mData.mArchetypes[mArchetypeIndex]) ||
                    mRow >= mData.mWorld.tables().dense(mData.mArchetypes[mArchetypeIndex]).size()))
            {
                ++mArchetypeIndex;
                mRow = 0;
            }
        }

        QueryData& mData;
        std::size_t mArchetypeIndex;
        std::size_t mRow;
    };
} // namespace cubos::core::ecs
