#pragma once

#include <map>
#include <vector>

#include <cubos/core/ecs/system/system.hpp>

using namespace cubos::core::ecs;

inline System<void> makeSystem(World& world, auto system)
{
    return System<void>::make(world, std::move(system), {});
}

inline System<bool> makeCondition(World& world, auto condition)
{
    return System<bool>::make(world, std::move(condition), {});
}

template <int value>
inline void pushToOrder(std::vector<int>& order)
{
    order.push_back(value);
}

template <int value, bool result>
inline bool pushToOrderAndReturn(std::vector<int>& order)
{
    order.push_back(value);
    return result;
}

template <int value, int times>
inline bool pushToOrderAndTrueNTimes(std::map<int, int>& state, std::vector<int>& order)
{
    state.emplace(value, 0);
    order.push_back(value);

    if (state.at(value) == times)
    {
        state.at(value) = 0;
        return false;
    }
    else
    {
        state.at(value) += 1;
        return true;
    }
}

inline std::string orderToString(const std::vector<int>& order)
{
    std::string str;
    for (const auto& i : order)
    {
        if (!str.empty())
        {
            str += ' ';
        }
        str += std::to_string(i);
    }
    return str;
}
