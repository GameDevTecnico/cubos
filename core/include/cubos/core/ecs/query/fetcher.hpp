/// @file
/// @brief Class @ref cubos::core::ecs::QueryFetcher.
/// @ingroup core-ecs-query

#pragma once

#include <cubos/core/ecs/query/opt.hpp>
#include <cubos/core/ecs/query/term.hpp>
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
        /// @param term Term corresponding to the argument being fetched, with specified targets.
        QueryFetcher(World& world, const QueryTerm& term)
        {
            (void)world;
            (void)term;

            // This should never be instantiated. This constructor is only defined for documentation purposes.
            static_assert(sizeof(T) == 0, "Invalid query argument type");
        }

        /// @brief Creates a query term with unspecified targets for the argument type.
        /// @param world World being queried.
        /// @return Term information.
        static QueryTerm term(World& world)
        {
            (void)world;

            // This should never be instantiated. This method is only defined for documentation purposes.
            static_assert(AlwaysFalse<T>, "Invalid query argument type");

            CUBOS_UNREACHABLE();
        }

        /// @brief Called when iteration starts for the given archetypes. Always called before @ref fetch(), which
        /// may be called multiple times after this.
        /// @param targetArchetypes Pointer to array with the archetype identifiers for the targets.
        void prepare(const ArchetypeId* targetArchetypes)
        {
            (void)targetArchetypes;

            // This should never be instantiated. This method is only defined for documentation purposes.
            static_assert(AlwaysFalse<T>, "Invalid query argument type");
        }

        /// @brief Called to get the actual desired data for a specific match. Always called after @ref prepare() has
        /// been called at least once.
        /// @param row Row to fetch.
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
        QueryFetcher(World& world, const QueryTerm& term)
            : mWorld{world}
            , mTarget{term.entity.target}
        {
        }

        static QueryTerm term(World& /*world*/)
        {
            return QueryTerm::makeEntity(-1);
        }

        void prepare(const ArchetypeId* targetArchetypes)
        {
            mTable = &mWorld.tables().dense().at(targetArchetypes[mTarget]);
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
        int mTarget;
    };

    template <reflection::Reflectable T>
    class QueryFetcher<T&>
    {
    public:
        QueryFetcher(World& world, const QueryTerm& term)
            : mWorld{world}
            , mDataType{world.types().id(reflection::reflect<T>())}
            , mIsComponent{world.types().isComponent(mDataType)}
        {
            if (mIsComponent)
            {
                mTargets[0] = term.component.target;
            }
            else
            {
                mTargets[0] = term.relation.fromTarget;
                mTargets[1] = term.relation.toTarget;
            }
        }

        static QueryTerm term(World& world)
        {
            auto type = world.types().id(reflection::reflect<T>());

            if (world.types().isComponent(type))
            {
                return QueryTerm::makeWithComponent(type, -1);
            }

            if (world.types().isRelation(type))
            {
                return QueryTerm::makeRelation(type, -1, -1);
            }

            CUBOS_FAIL("Query arguments of the form T& must be components or relations");
        }

        void prepare(const ArchetypeId* targetArchetypes)
        {
            if (mIsComponent)
            {
                auto id = ColumnId::make(mDataType);
                mColumn = &mWorld.tables().dense().at(targetArchetypes[mTargets[0]]).column(id);
            }
            else
            {
                mSparseRelationTable = &mWorld.tables().sparseRelation().at({
                    mDataType,
                    targetArchetypes[mTargets[0]],
                    targetArchetypes[mTargets[1]],
                });
            }
        }

        T& fetch(std::size_t row)
        {
            if (mColumn != nullptr)
            {
                return *static_cast<T*>(mColumn->at(row));
            }

            return *static_cast<T*>(mSparseRelationTable->at(row));
        }

    private:
        World& mWorld;
        DataTypeId mDataType;
        memory::AnyVector* mColumn{nullptr};
        SparseRelationTable* mSparseRelationTable{nullptr};
        int mTargets[2];
        bool mIsComponent;
    };

    template <reflection::Reflectable T>
    class QueryFetcher<const T&>
    {
    public:
        QueryFetcher(World& world, const QueryTerm& term)
            : mWorld{world}
            , mDataType{world.types().id(reflection::reflect<T>())}
            , mIsComponent{world.types().isComponent(mDataType)}
        {
            if (mIsComponent)
            {
                mTargets[0] = term.component.target;
            }
            else
            {
                mTargets[0] = term.relation.fromTarget;
                mTargets[1] = term.relation.toTarget;
            }
        }

        static QueryTerm term(World& world)
        {
            auto type = world.types().id(reflection::reflect<T>());

            if (world.types().isComponent(type))
            {
                return QueryTerm::makeWithComponent(type, -1);
            }

            if (world.types().isRelation(type))
            {
                return QueryTerm::makeRelation(type, -1, -1);
            }

            CUBOS_FAIL("Query arguments of the form const T& must be components or relations");
        }

        void prepare(const ArchetypeId* targetArchetypes)
        {
            if (mIsComponent)
            {
                auto id = ColumnId::make(mDataType);
                mColumn = &mWorld.tables().dense().at(targetArchetypes[mTargets[0]]).column(id);
            }
            else
            {
                mSparseRelationTable = &mWorld.tables().sparseRelation().at({
                    mDataType,
                    targetArchetypes[mTargets[0]],
                    targetArchetypes[mTargets[1]],
                });
            }
        }

        const T& fetch(std::size_t row)
        {
            if (mColumn != nullptr)
            {
                return *static_cast<const T*>(mColumn->at(row));
            }

            return *static_cast<const T*>(mSparseRelationTable->at(row));
        }

    private:
        World& mWorld;
        DataTypeId mDataType;
        memory::AnyVector* mColumn{nullptr};
        SparseRelationTable* mSparseRelationTable{nullptr};
        int mTargets[2];
        bool mIsComponent;
    };

    template <reflection::Reflectable T>
    class QueryFetcher<Opt<T&>>
    {
    public:
        QueryFetcher(World& world, const QueryTerm& term)
            : mWorld{world}
            , mColumnId{ColumnId::make(world.types().id(reflection::reflect<T>()))}
            , mTarget{term.component.target}
        {
        }

        static QueryTerm term(World& world)
        {
            auto type = world.types().id(reflection::reflect<T>());

            CUBOS_ASSERT(world.types().isComponent(type), "Query arguments of the form Opt<T&> must be components");

            return QueryTerm::makeOptComponent(type, -1);
        }

        void prepare(const ArchetypeId* targetArchetypes)
        {
            if (mWorld.tables().dense().at(targetArchetypes[mTarget]).contains(mColumnId))
            {
                mColumn = &mWorld.tables().dense().at(targetArchetypes[mTarget]).column(mColumnId);
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
        ColumnId mColumnId;
        memory::AnyVector* mColumn{nullptr};
        int mTarget;
    };

    template <reflection::Reflectable T>
    class QueryFetcher<Opt<const T&>>
    {
    public:
        QueryFetcher(World& world, const QueryTerm& term)
            : mWorld{world}
            , mColumnId{ColumnId::make(world.types().id(reflection::reflect<T>()))}
            , mTarget{term.component.target}
        {
        }

        static QueryTerm term(World& world)
        {
            auto type = world.types().id(reflection::reflect<T>());

            CUBOS_ASSERT(world.types().isComponent(type),
                         "Query arguments of the form Opt<const T&> must be components");

            return QueryTerm::makeOptComponent(type, -1);
        }

        void prepare(const ArchetypeId* targetArchetypes)
        {
            if (mWorld.tables().dense().at(targetArchetypes[mTarget]).contains(mColumnId))
            {
                mColumn = &mWorld.tables().dense().at(targetArchetypes[mTarget]).column(mColumnId);
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
        ColumnId mColumnId;
        memory::AnyVector* mColumn{nullptr};
        int mTarget;
    };
} // namespace cubos::core::ecs
