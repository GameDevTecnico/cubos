#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/font/atlas/glyph.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(FontGlyph)
{
    return cubos::core::ecs::TypeBuilder<FontGlyph>("cubos::engine::Glyph")
        .withField("texCoordsMin", &FontGlyph::texCoordsMin)
        .withField("texCoordsMax", &FontGlyph::texCoordsMax)
        .withField("positionMin", &FontGlyph::positionMin)
        .withField("positionMax", &FontGlyph::positionMax)
        .withField("advance", &FontGlyph::advance)
        .build();
}
