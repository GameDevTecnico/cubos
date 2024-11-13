#pragma once

#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/reflection/reflect.hpp>

namespace cubos::engine
{
    struct UITextImpl
    {
        CUBOS_REFLECT;

        core::gl::VertexArray vertexArray{nullptr};

        size_t vertexCount{0};
    };
} // namespace cubos::engine
