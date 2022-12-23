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

int main(int argc, char** argv)
{
    cubos::engine::Cubos cubos;

    // Order using tags
    cubos.startupSystem(tagB)
        .tagged("B")
        .beforeTag("C");
    cubos.startupSystem(tagC)
        .tagged("C")
        .afterTag("A");
    cubos.startupSystem(tagA)
        .tagged("A");


    // Order using systems
    cubos.startupSystem(systemC);

    cubos.startupSystem(systemA)
        .beforeSystem(systemC)
        .afterSystem(tagC);
    
    cubos.startupSystem(systemB)
        .afterSystem(systemA);

    // Now add a lambda between both systems to test
    auto lambda = []() { CUBOS_INFO("--- INTERMISSION ---");};

    cubos.startupSystem(lambda)
        .afterSystem(tagC);

    cubos.run();
}
