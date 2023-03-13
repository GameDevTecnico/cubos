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

    cubos.startupTag("B").beforeTag("C");
    cubos.startupTag("C").afterTag("A");
    cubos.startupTag("A");

    // Order using tags
    cubos.startupSystem(tagB).tagged("B");
    cubos.startupSystem(tagC).tagged("C");
    cubos.startupSystem(tagA).tagged("A");

    // Order using systems
    cubos.startupSystem(systemC);
    cubos.startupSystem(systemA).beforeSystem(systemC).afterTag("C");
    cubos.startupSystem(systemB).afterSystem(systemA);

    // Lambda between systems and tags
    auto lambda = []() { CUBOS_INFO("--- INTERMISSION ---"); };
    cubos.startupSystem(lambda).afterTag("C").beforeSystem(systemA);

    // System inheritance
    cubos.startupSystem(systemInherit1).tagged("A");
    cubos.startupSystem(systemInherit2).afterTag("B").beforeSystem(systemInherit1);

    // Closed loop. This will prevent chain compilation!
    // cubos.startupTag("A").afterTag("C");

    cubos.run();
}
