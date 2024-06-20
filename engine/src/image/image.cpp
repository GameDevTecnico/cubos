#include <stb_image.h>

#include <cubos/core/reflection/external/cstring.hpp>
#include <cubos/core/reflection/type.hpp>
#include <cubos/core/tel/logging.hpp>

#include <cubos/engine/image/image.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::Image)
{
    return core::reflection::Type::create("cubos::engine::Image");
}

cubos::engine::Image::Image(core::memory::Stream& stream)
{
    stream.seek(0, cubos::core::memory::SeekOrigin::End);
    size_t streamSize = stream.tell();
    void* contents = operator new(sizeof(char) * streamSize);
    stream.seek(0, cubos::core::memory::SeekOrigin::Begin);
    if (stream.read((char*)contents, streamSize) < streamSize)
    {
        CUBOS_ERROR("Couldn't load image: Read less than stream lenght", stbi_failure_reason());
        data = nullptr;
        operator delete(contents);
        return;
    };
    int x;
    int y;
    int n;
    data = stbi_load_from_memory((const unsigned char*)contents, (int)streamSize, &x, &y, &n, 4);
    if (data == nullptr)
    {
        CUBOS_ERROR("Couldn't load image: {}", stbi_failure_reason());
    }
    width = (size_t)x;
    height = (size_t)y;
    operator delete(contents);
}

cubos::engine::Image::~Image()
{
    if (data != nullptr)
    {
        stbi_image_free(data);
    }
}

cubos::engine::Image::Image(Image&& other) noexcept
    : data(other.data)
    , width(other.width)
    , height(other.height)
{
    other.data = nullptr;
}