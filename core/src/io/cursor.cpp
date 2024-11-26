#include <cubos/core/io/cursor.hpp>
#include <cubos/core/reflection/traits/enum.hpp>
#include <cubos/core/reflection/type.hpp>

using namespace cubos::core;

using cubos::core::io::Cursor;
using cubos::core::reflection::EnumTrait;
using cubos::core::reflection::Type;

#ifdef CUBOS_CORE_GLFW
io::Cursor::Cursor(GLFWcursor* handle)
{
    mGlfwHandle = handle;
}
#endif

io::Cursor::~Cursor()
{
#ifdef CUBOS_CORE_GLFW
    if (mGlfwHandle != nullptr)
    {
        glfwDestroyCursor(mGlfwHandle);
    }
#endif
}

CUBOS_REFLECT_EXTERNAL_IMPL(Cursor::Standard)
{
    return Type::create("Standard")
        .with(EnumTrait{}
                  .withVariant<Cursor::Standard::Arrow>("Arrow")
                  .withVariant<Cursor::Standard::IBeam>("IBeam")
                  .withVariant<Cursor::Standard::Cross>("Cross")
                  .withVariant<Cursor::Standard::Hand>("Hand")
                  .withVariant<Cursor::Standard::EWResize>("EWResize")
                  .withVariant<Cursor::Standard::NSResize>("NSResize")
                  .withVariant<Cursor::Standard::NWSEResize>("NWSEResize")
                  .withVariant<Cursor::Standard::NESWResize>("NESWResize")
                  .withVariant<Cursor::Standard::AllResize>("AllResize")
                  .withVariant<Cursor::Standard::NotAllowed>("NotAllowed"));
}
