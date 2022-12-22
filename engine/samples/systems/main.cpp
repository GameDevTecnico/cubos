#include <cubos/engine/cubos.hpp>

void a()
{
    CUBOS_INFO("A");
}
void b()
{
    CUBOS_INFO("B");
}
void c()
{
    CUBOS_INFO("C");
}

int main(int argc, char** argv)
{
    cubos::engine::Cubos()

        .startupSystem(b)
        .tagged("B")
        .beforeTag("C")

        .startupSystem(c)
        .tagged("C")
        .afterTag("A")

        .startupSystem(a)
        .tagged("A")

        .run();
}
