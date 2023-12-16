/// @file
/// @brief Class @ref cubos::core::ecs::QueryFetcher.
/// @ingroup core-ecs-query

#include <cubos/core/ecs/query/access.hpp>
#include <cubos/core/ecs/query/opt.hpp>
#include <cubos/core/ecs/world.hpp>

namespace cubos::core::ecs
{
    /// @brief Type meant to be specialized which implements for each argument type the necessary logic to extract it
    /// from the tables.
    /// @tparam T Argument type.
    /// @ingroup core-ecs-query
    template <typename T>
    class QueryFetcher
    {
    public:
        /// @brief Called when a query is constructed for the first time.
        /// @param world World being queried.
        QueryFetcher(World& world)
        {
            (void)world;

            // This should never be instantiated. This constructor is only defined for documentation purposes.
            static_assert(sizeof(T) == 0, "Invalid query argument type");
        }

        /// @brief Called to predict the access patterns of this argument type.
        /// @param[out] access Access patterns.
        void analyze(QueryAccess& access)
        {
            (void)access;

            // This should never be instantiated. This method is only defined for documentation purposes.
            static_assert(AlwaysFalse<T>, "Invalid query argument type");
        }

        /// @brief Called when iteration starts for the given archetype. Always called before @ref fetch(), which
        /// may be called multiple times after this.
        /// @param archetype Archetype identifier.
        void prepare(ArchetypeId archetype)
        {
            (void)archetype;

            // This should never be instantiated. This method is only defined for documentation purposes.
            static_assert(AlwaysFalse<T>, "Invalid query argument type");
        }

        /// @brief Called to get the actual desired data for a specific entity. Always called after @ref prepare() has
        /// been called at least once.
        /// @param row Entity's row in its archetype's dense table.
        T fetch(std::size_t row)
        {
            (void)row;

            // This should never be instantiated. This method is only defined for documentation purposes.
            static_assert(AlwaysFalse<T>, "Invalid query argument type");
        }

    private:
        // Looks dumb, but this is necessary to make the static_assert's only trigger when the template is instantiated.
        // Reference: https://stackoverflow.com/questions/44059557/whats-the-right-way-to-call-static-assertfalse
        template <typename U>
        static constexpr bool AlwaysFalse = false;
    };

    template <>
    class QueryFetcher<Entity>
    {
    public:
        QueryFetcher(World& world)
            : mWorld{world}
        {
        }

        void analyze(QueryAccess& /*access*/)
        {
        }

        void prepare(ArchetypeId archetype)
        {
            mTable = &mWorld.tables().dense(archetype);
        }

        Entity fetch(std::size_t row)
        {
            auto index = mTable->entity(row);
            auto generation = mWorld.generation(index);
            return {index, generation};
        }

    private:
        World& mWorld;
        DenseTable* mTable{nullptr};
    };

    template <reflection::Reflectable T>
    class QueryFetcher<T&>
    {
    public:
        QueryFetcher(World& world)
            : mWorld{world}
            , mColumnId{DenseColumnId::make(world.types().id(reflection::reflect<T>()))}
        {
        }

        void analyze(QueryAccess& access)
        {
            access.insert(mColumnId.dataType());
        }

        void prepare(ArchetypeId archetype)
        {
            mColumn = &mWorld.tables().dense(archetype).column(mColumnId);
        }

        T& fetch(std::size_t row)
        {
            return *static_cast<T*>(mColumn->at(row));
        }

    private:
        World& mWorld;
        DenseColumnId mColumnId;
        memory::AnyVector* mColumn{nullptr};
    };

    template <reflection::Reflectable T>
    class QueryFetcher<const T&>
    {
    public:
        QueryFetcher(World& world)
            : mWorld{world}
            , mColumnId{DenseColumnId::make(world.types().id(reflection::reflect<T>()))}
        {
        }

        void analyze(QueryAccess& access)
        {
            access.insert(mColumnId.dataType());
        }

        void prepare(ArchetypeId archetype)
        {
            mColumn = &mWorld.tables().dense(archetype).column(mColumnId);
        }

        const T& fetch(std::size_t row)
        {
            return *static_cast<const T*>(mColumn->at(row));
        }

    private:
        World& mWorld;
        DenseColumnId mColumnId;
        memory::AnyVector* mColumn{nullptr};
    };

    template <reflection::Reflectable T>
    class QueryFetcher<Opt<T&>>
    {
    public:
        QueryFetcher(World& world)
            : mWorld{world}
            , mColumnId{DenseColumnId::make(world.types().id(reflection::reflect<T>()))}
        {
        }

        void analyze(QueryAccess& /*access*/)
        {
        }

        void prepare(ArchetypeId archetype)
        {
            if (mWorld.tables().dense(archetype).contains(mColumnId))
            {
                mColumn = &mWorld.tables().dense(archetype).column(mColumnId);
            }
            else
            {
                mColumn = nullptr;
            }
        }

        Opt<T&> fetch(std::size_t row)
        {
            if (mColumn == nullptr)
            {
                return {};
            }

            return {*static_cast<T*>(mColumn->at(row))};
        }

    private:
        World& mWorld;
        DenseColumnId mColumnId;
        memory::AnyVector* mColumn{nullptr};
    };

    template <reflection::Reflectable T>
    class QueryFetcher<Opt<const T&>>
    {
    public:
        QueryFetcher(World& world)
            : mWorld{world}
            , mColumnId{DenseColumnId::make(world.types().id(reflection::reflect<T>()))}
        {
        }

        void analyze(QueryAccess& /*access*/)
        {
        }

        void prepare(ArchetypeId archetype)
        {
            if (mWorld.tables().dense(archetype).contains(mColumnId))
            {
                mColumn = &mWorld.tables().dense(archetype).column(mColumnId);
            }
            else
            {
                mColumn = nullptr;
            }
        }

        Opt<const T&> fetch(std::size_t row)
        {
            if (mColumn == nullptr)
            {
                return {};
            }

            return {*static_cast<const T*>(mColumn->at(row))};
        }

    private:
        World& mWorld;
        DenseColumnId mColumnId;
        memory::AnyVector* mColumn{nullptr};
    };
} // namespace cubos::core::ecs
