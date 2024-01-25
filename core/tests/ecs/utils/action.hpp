#pragma once

#include "expected.hpp"

namespace test::ecs
{
    // Abstract test which can be performed on a world.
    class Action
    {
    public:
        virtual ~Action() = default;

        // Returns a string describing the action.
        virtual std::string description() const = 0;

        // Runs the test on the given world, possibly modifying it.
        virtual void test(World& world, ExpectedWorld& expected) = 0;
    };

    // Creates a new entity.
    class CreateAction : public Action
    {
    public:
        static Action* random(ExpectedWorld& expected);
        std::string description() const override;
        virtual void test(World& world, ExpectedWorld& expected) override;
    };

    // Destroys an entity.
    class DestroyAction : public Action
    {
    public:
        DestroyAction(Entity entity);

        static Action* random(ExpectedWorld& expected);
        std::string description() const override;
        virtual void test(World& world, ExpectedWorld& expected) override;

    private:
        Entity mEntity;
    };

    // Adds a component to an entity.
    class AddAction : public Action
    {
    public:
        AddAction(Entity entity, const Type& type, int value);

        static Action* random(ExpectedWorld& expected);
        std::string description() const override;
        virtual void test(World& world, ExpectedWorld& expected) override;

    private:
        Entity mEntity;
        const Type& mType;
        int mValue;
    };

    // Removes a component from an entity.
    class RemoveAction : public Action
    {
    public:
        RemoveAction(Entity entity, const Type& type);

        static Action* random(ExpectedWorld& expected);
        std::string description() const override;
        virtual void test(World& world, ExpectedWorld& expected) override;

    private:
        Entity mEntity;
        const Type& mType;
    };

    // Adds a relation between two entities.
    class RelateAction : public Action
    {
    public:
        RelateAction(Entity from, Entity to, const Type& type, int value);

        static Action* random(ExpectedWorld& expected);
        std::string description() const override;
        virtual void test(World& world, ExpectedWorld& expected) override;

    private:
        Entity mFrom;
        Entity mTo;
        const Type& mType;
        int mValue;
    };

    // Removes a relation between two entities.
    class UnrelateAction : public Action
    {
    public:
        UnrelateAction(Entity from, Entity to, const Type& type);

        static Action* random(ExpectedWorld& expected);
        std::string description() const override;
        virtual void test(World& world, ExpectedWorld& expected) override;

    private:
        Entity mFrom;
        Entity mTo;
        const Type& mType;
    };

    // Executes a query with 'with component', 'without component' and 'optional component' terms.
    class SingleTargetQueryAction : public Action
    {
    public:
        SingleTargetQueryAction(std::unordered_set<const Type*> with, std::unordered_set<const Type*> without,
                                std::unordered_set<const Type*> optional);

        static Action* random(ExpectedWorld& expected);
        std::string description() const override;
        virtual void test(World& world, ExpectedWorld& expected) override;

    private:
        std::unordered_set<const Type*> mWith;
        std::unordered_set<const Type*> mWithout;
        std::unordered_set<const Type*> mOptional;
    };

    // Executes a query with 'with component', 'without component', 'optional component' and 'relation' terms.
    class SingleRelationQueryAction : public Action
    {
    public:
        SingleRelationQueryAction(const Type& relation, std::unordered_set<const Type*> withA,
                                  std::unordered_set<const Type*> withoutA, std::unordered_set<const Type*> optionalA,
                                  std::unordered_set<const Type*> withB, std::unordered_set<const Type*> withoutB,
                                  std::unordered_set<const Type*> optionalB);

        static Action* random(ExpectedWorld& expected);
        std::string description() const override;
        virtual void test(World& world, ExpectedWorld& expected) override;

    private:
        const Type& mRelation;
        std::unordered_set<const Type*> mWithA;
        std::unordered_set<const Type*> mWithoutA;
        std::unordered_set<const Type*> mOptionalA;
        std::unordered_set<const Type*> mWithB;
        std::unordered_set<const Type*> mWithoutB;
        std::unordered_set<const Type*> mOptionalB;
    };
} // namespace test::ecs
