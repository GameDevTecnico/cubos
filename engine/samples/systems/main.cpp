#include <cubos/engine/cubos.hpp>

void tagA()
{
    CUBOS_INFO("[By Tag] A");
}
void tagB()
{
    CUBOS_INFO("[By Tag] B");
}
void tagC()
{
    CUBOS_INFO("[By Tag] C");
}

void systemA()
{
    CUBOS_INFO("[By System] A");
}
void systemB()
{
    CUBOS_INFO("[By System] B");
}
void systemC()
{
    CUBOS_INFO("[By System] C");
}

void systemInherit1()
{
    CUBOS_INFO("[By System] Inheritance 1");
}

void systemInherit2()
{
    CUBOS_INFO("[By System] Inheritance 2");
}

int main()
{
    cubos::engine::Cubos cubos;

    cubos.startupTag("B").before("C");
    cubos.startupTag("C").after("A");
    cubos.startupTag("A");

    // Order using tags
    cubos.startupSystem(tagB).tagged("B");
    cubos.startupSystem(tagC).tagged("C");
    cubos.startupSystem(tagA).tagged("A");

    // Closed loop. This will prevent chain compilation!
    // cubos.startupTag("A").after("C");

    // Conditions, determining system execution
    auto lambdaCondYes = []() { CUBOS_INFO("System ran with condition true!"); };
    cubos.startupSystem(lambdaCondYes).runIf([]() { return true; });

    auto lambdaCondNo = []() { CUBOS_INFO("System ran with condition false, you should not be reading this!"); };
    cubos.startupSystem(lambdaCondNo).runIf([]() { return false; }).runIf([]() { return true; });

    // Conditions can also be inherited by systems and tags
    cubos.startupTag("conditionInherit").runIf([]() { return false; });
    cubos.startupSystem(lambdaCondNo).tagged("conditionInherit");

    cubos.run();
}
