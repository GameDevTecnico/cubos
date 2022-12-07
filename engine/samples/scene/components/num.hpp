#ifndef COMPONENTS_NUM_HPP
#define COMPONENTS_NUM_HPP

#include <components/base.hpp>

struct [[cubos::component("num", VecStorage)]] Num
{
    int value;
};

#endif // COMPONENTS_NUM_HPP
