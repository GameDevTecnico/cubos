#include <cubos/core/ecs/query/node/node.hpp>
#include <cubos/core/log.hpp>

using cubos::core::ecs::QueryNode;

QueryNode::QueryNode(int cursor)
    : mCursor{cursor}
{
}

int QueryNode::cursor() const
{
    return mCursor;
}

QueryNode::TargetMask QueryNode::pins() const
{
    return mPins;
}

void QueryNode::pins(int target)
{
    CUBOS_ASSERT(target >= 0 && target < MaxTargetCount);
    mPins |= 1 << target;
}
