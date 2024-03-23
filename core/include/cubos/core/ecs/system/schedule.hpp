/// @file
/// @brief Class @ref cubos::core::ecs::Schedule.
/// @ingroup core-ecs-system

#pragma once

#include <queue>
#include <vector>

#include <cubos/core/ecs/query/opt.hpp>
#include <cubos/core/ecs/system/registry.hpp>

namespace cubos::core::ecs
{
    /// @brief Stores schedule nodes and the restrictions that must be met for them to run.
    ///
    /// In short, the algorithm implemented here is based on a graph of dependencies. There are three types of nodes:
    /// - system nodes.
    /// - condition nodes;
    /// - repeat nodes;
    ///
    /// Additionally, there are three types of constraints which can be applied to the flow:
    /// - ordering constraints, e.g., node 1 runs before node 2 - specified through @ref order.
    /// - only if constraints, e.g., node 1 runs if condition node 2 returns true - specified through @ref onlyIf.
    /// - repeat constraints, e.g., node 1 is part of repeat node 2 - specified during node creation time.
    ///
    /// For a more in-depth and accurate analysis of the behavior of this class, take a look at its unit tests.
    ///
    /// # Execution
    ///
    /// Each node stores its 'needed satisfaction', which represents how many nodes must mark it as 'satisfied' for it
    /// to be illegible to run. During execution, nodes keep the current satisfaction level as state. When that level
    /// reaches the level needed to run, the node is pushed to the execution queue.
    ///
    /// System nodes are the most basic of the node types. When they execute, their respective system is run. All other
    /// nodes which have order constraints depending on it have their satisfaction increased.
    ///
    /// Condition nodes which evaluate to true mark all dependents as satisfied. If they evaluate to false, they mark
    /// the dependents of the dependents as satisfied, effectively skipping these nodes.
    ///
    /// Repeat nodes are the most complex of the nodes. Each node stores the identifier of the repeat node it belongs to
    /// (which may be none). All nodes belonging to one have a base satisfaction level of 1. When the repeat node
    /// evaluates to true, this level is increased, allowing their execution to start. Additionally, the repeat node's
    /// satisfaction is decreased by the number of belonging nodes. When each of those nodes finishes, it increments the
    /// satisfaction level of the repeat node. When all nodes are finished, the repeat node runs once again.
    ///
    /// If a repeat node evaluates to false, it marks as satisfied all of its dependents, as only then it has finished.
    ///
    /// The execution stops when the queue is emptied.
    ///
    /// # Constraint Specification
    ///
    /// Any two nodes may constrained to run in a given order in relation to each other. If two nodes are part of
    /// different repeat nodes, the ordering is applied for their lowest ancestor repeat nodes which have the same
    /// parent repeat node. This means, that, if, for example, you have two systems A and B, each part of their own
    /// repeat node, if you force A to run before B, then A's repeat node will entirely run before B's repeat node. This
    /// is necessary as the number of times a repeat node executes is unknown - it may vary depending on the condition.
    ///
    /// When only if constraints are applied between a node and a condition, the condition always runs before the
    /// system, but the system only runs if the condition evaluates to true. Only if constraints are only allowed when
    /// the condition doesn't repeat from the point of view of the node. More formally, the condition must be part of
    /// the node's repeat node or one of its ancestors.
    ///
    /// Any operation which would introduce a cycle fails by returning false and logging an error.
    ///
    /// # Concurrency
    ///
    /// This algorithm makes it easy to run systems concurrently. To do this, we just have to distribute the work
    /// between threads each time a node is popped from the queue. One thing to keep in mind is that this class by
    /// itself doesn't solve node conflicts. If two nodes write to the same resource, for example, and if they aren't
    /// ordered, an arbitrary order constraint should be added between them, to ensure that they won't run concurrently.
    ///
    /// @ingroup core-ecs-system
    class Schedule
    {
    public:
        /// @brief Identifies a node in the schedule.
        struct NodeId
        {
            std::size_t inner; ///< Node index in the schedule.

            /// @brief Compares two identifiers for equality.
            /// @param other Other identifier.
            /// @return Whether the two identifiers are equal.
            bool operator==(const NodeId& other) const = default;
        };

        /// @brief Removes all nodes from the schedule.
        ///
        /// All previously returned node identifiers must no longer be used after this is called.
        void clear();

        /// @brief Adds a repeat node to the schedule which repeats as long as the given condition evaluates to true.
        /// @param conditionId Condition identifier.
        /// @param repeatId Repeat node to become part of, if any.
        /// @return Repeat node identifier, or nothing if the given repeat node is not a repeat node.
        Opt<NodeId> repeat(ConditionId conditionId, Opt<NodeId> repeatId = {});

        /// @brief Adds a system node to the schedule.
        /// @param systemId System identifier.
        /// @param repeatId Repeat node to become part of, if any.
        /// @return System node identifier, or nothing if the given repeat node is not a repeat node.
        Opt<NodeId> system(SystemId systemId, Opt<NodeId> repeatId = {});

        /// @brief Adds a condition node to the schedule.
        /// @param conditionId Condition identifier.
        /// @param repeatId Repeat node to become part of, if any.
        /// @return Condition node identifier, or nothing if the given repeat node is not a repeat node.
        Opt<NodeId> condition(ConditionId conditionId, Opt<NodeId> repeatId = {});

        /// @brief Specifies that a node should only run if a given condition node returns true after finishing.
        ///
        /// Implicitly enforces an ordering constraint, such that the condition always finishes before the node starts.
        /// Fails by returning false if the given condition node isn't a condition node.
        ///
        /// @param node Node.
        /// @param condition Condition.
        /// @return Whether the operation was successful.
        bool onlyIf(NodeId node, NodeId condition);

        /// @brief Specifies that a node must run after another node finishes.
        ///
        /// If doing so would introduce a dependency cycle in the schedule, the operation is cancelled and the method
        /// returns false.
        ///
        /// @param before Node that must finish.
        /// @param after Node that must run after @p before finishes.
        /// @return Whether the restriction was successfully added.
        bool order(NodeId before, NodeId after);

        /// @brief Runs the systems in the schedule.
        /// @param registry Registry containing the systems.
        /// @param context Context to run the systems with.
        void run(SystemRegistry& registry, SystemContext& context);

    private:
        /// @brief Holds the state of a schedule node.
        struct Node
        {
            /// @brief Current satisfaction level.
            int satisfaction{0};

            /// @brief Satisfaction needed for the node to run.
            int neededSatisfaction{0};

            /// @brief Whether the node is a repeat node and is currently repeating.
            bool isRepeating{false};

            /// @brief Whether the node is a repeat node and has already been satisfied in the current run.
            bool alreadyRepeated{false};

            /// @brief Whether the node has already finished once in the current run.
            bool alreadyFinished{false};

            /// @brief Whether the node has already evaluated to true once in the current run.
            bool alreadyEvaluatedToTrue{false};

            /// @brief If the node is part of a repeat node, holds the repeat node's identifier.
            Opt<NodeId> repeatId{};

            /// @brief If the node is a system node, holds the system identifier.
            Opt<SystemId> systemId{};

            /// @brief If the node is a repeat or condition node, holds the respective condition identifier.
            Opt<ConditionId> conditionId{};

            /// @brief Used to distinguish between repeat and condition nodes.
            bool isRepeat{false};

            /// @brief Nodes whose satisfaction should be incremented when this node finishes.
            std::vector<NodeId> satisfyOnFinish{};

            /// @brief Nodes whose satisfaction should be incremented when this node returns true.
            ///
            /// Used only for condition nodes.
            std::vector<NodeId> satisfyOnTrue{};

            /// @brief Nodes which are part of this node. Used by repeat nodes.
            std::vector<NodeId> parts{};

            /// @brief How many of the part nodes have been skipped due to false external conditions?
            ///
            /// Only incremented when a condition outside of the repeat node which affects a part evaluates to false.
            int skippedParts{0};

            /// @brief How deep the node is in the repeat hierarchy.
            int depth{0};
        };

        /// @brief Adds a new default constructed node to the schedule.
        /// @param repeatId Repeat node to become part of, if any.
        /// @return Node identifier, or nothing if the given repeat node is not a repeat node.
        Opt<NodeId> node(Opt<NodeId> repeatId);

        /// @brief Runs a specific node.
        /// @param registry Registry containing the systems.
        /// @param context Context to run the systems with.
        /// @param nodeId Node identifier.
        void run(SystemRegistry& registry, SystemContext& context, NodeId nodeId);

        /// @brief Checks if there is an ordering restriction between the given nodes in the given direction.
        /// @param before Before node identifier.
        /// @param after After node identifier.
        /// @return Whether the nodes are ordered.
        bool ordered(NodeId before, NodeId right) const;

        /// @brief Gets the uppermost repeat node of a given node, or the node itself if it isn't part of one.
        /// @param node Node identifier to start from.
        /// @return Node identifier.
        NodeId rootRepeatNode(NodeId node) const;

        /// @brief Traverses up in both nodes until their depths are repeat nodes are the same.
        /// @param left First node identifier.
        /// @param right Second node identifier.
        void matchNodeDepths(NodeId& left, NodeId& right) const;

        /// @brief Increments the satisfaction of the given node, if there's one.
        /// @param node Node identifier.
        void incrementSatisfaction(Opt<NodeId> node);

        /// @brief Increments the satisfaction of all of the given nodes.
        /// @param nodes Node identifiers.
        void incrementSatisfaction(const std::vector<NodeId>& nodes);

        /// @brief Decrements the satisfaction of all of the given nodes.
        /// @param nodes Node identifiers.
        void decrementSatisfaction(const std::vector<NodeId>& nodes);

        std::vector<Node> mNodes;
        std::queue<NodeId> mSatisfied;
    };
} // namespace cubos::core::ecs
