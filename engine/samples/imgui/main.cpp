#include <iostream>
#include <map>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <imgui.h>

#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/map.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/vector.hpp>
#include <cubos/core/reflection/reflect.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/traits/enum.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/traits/mask.hpp>
#include <cubos/core/reflection/traits/nullable.hpp>

/// [Including plugin headers]
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/font/plugin.hpp>
#include <cubos/engine/imgui/inspector.hpp>
#include <cubos/engine/imgui/plugin.hpp>
/// [Including plugin headers]
#include <cubos/engine/render/target/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

using cubos::core::reflection::ConstructibleTrait;
using cubos::core::reflection::EnumTrait;
using cubos::core::reflection::FieldsTrait;
using cubos::core::reflection::MaskTrait;
using cubos::core::reflection::reflect;
using cubos::core::reflection::Type;

using namespace cubos::engine;

struct MyUnsupported
{
    CUBOS_REFLECT;
};

CUBOS_REFLECT_IMPL(MyUnsupported)
{
    return Type::create("MyUnsupported");
}

struct MyStruct
{
    CUBOS_REFLECT;

    int integer{};
    float fract{};

    MyStruct(int integer = 3, float fract = 0.14F)
        : integer(integer)
        , fract(fract)
    {
    }

    MyStruct(float number)
    {
        integer = static_cast<int>(number);
        fract = number - static_cast<float>(integer);
    }
};

CUBOS_REFLECT_IMPL(MyStruct)
{
    return Type::create("MyStruct")
        .with(FieldsTrait{}.withField("integer", &MyStruct::integer).withField("fract", &MyStruct::fract))
        .with(ConstructibleTrait::typed<MyStruct>()
                  .withBasicConstructors()
                  .withCustomConstructor<int, float>({"integer", "fract"})
                  .withCustomConstructor<float>({"number"})
                  .build());
}

enum class MyEnum
{
    Foo,
    Bar,
    Baz,
};

CUBOS_REFLECT_EXTERNAL_DECL(CUBOS_EMPTY, MyEnum);
CUBOS_REFLECT_EXTERNAL_IMPL(MyEnum)
{
    return Type::create("MyEnum")
        .with(
            EnumTrait{}.withVariant<MyEnum::Foo>("Foo").withVariant<MyEnum::Bar>("Bar").withVariant<MyEnum::Baz>("Baz"))
        .with(ConstructibleTrait::typed<MyEnum>().withBasicConstructors().build());
}

enum class MyMask
{
    Flag1 = 1 << 0,
    Flag2 = 1 << 1,
    Flag3 = 1 << 2,
    Flag4 = 1 << 3,
};

MyMask operator|(MyMask lhs, MyMask rhs)
{
    return static_cast<MyMask>(static_cast<int>(lhs) | static_cast<int>(rhs));
}

MyMask operator&(MyMask lhs, MyMask rhs)
{
    return static_cast<MyMask>(static_cast<int>(lhs) & static_cast<int>(rhs));
}

MyMask operator~(MyMask lhs)
{
    return static_cast<MyMask>(~static_cast<int>(lhs));
}

CUBOS_REFLECT_EXTERNAL_DECL(CUBOS_EMPTY, MyMask);
CUBOS_REFLECT_EXTERNAL_IMPL(MyMask)
{
    return Type::create("MyMask")
        .with(MaskTrait{}
                  .withBit<MyMask::Flag1>("Flag1")
                  .withBit<MyMask::Flag2>("Flag2")
                  .withBit<MyMask::Flag3>("Flag3")
                  .withBit<MyMask::Flag4>("Flag4"))
        .with(ConstructibleTrait::typed<MyMask>().withBasicConstructors().build());
}

struct GLMState
{
    CUBOS_REFLECT;

    glm::mat2 mat2;
    glm::dmat2 dmat2;
    glm::mat2x3 mat2x3;
    glm::dmat2x3 dmat2x3;
    glm::mat2x4 mat2x4;
    glm::dmat2x4 dmat2x4;
    glm::mat3x2 mat3x2;
    glm::dmat3x2 dmat3x2;
    glm::mat3 mat3;
    glm::dmat3 dmat3;
    glm::mat3x4 mat3x4;
    glm::dmat3x4 dmat3x4;
    glm::mat4x2 mat4x2;
    glm::dmat4x2 dmat4x2;
    glm::mat4x3 mat4x3;
    glm::dmat4x3 dmat4x3;
    glm::mat4 mat4{1.0F};
    glm::dmat4 dmat4{1.0F};

    glm::vec2 vec2;
    glm::bvec2 bvec2;
    glm::dvec2 dvec2;
    glm::ivec2 ivec2;
    glm::uvec2 uvec2;
    glm::vec3 vec3;
    glm::bvec3 bvec3;
    glm::dvec3 dvec3;
    glm::ivec3 ivec3;
    glm::uvec3 uvec3;
    glm::vec4 vec4;
    glm::bvec4 bvec4;
    glm::dvec4 dvec4;
    glm::ivec4 ivec4;
    glm::uvec4 uvec4;

    glm::quat quat;
    glm::dquat dquat;
};

CUBOS_REFLECT_IMPL(GLMState)
{
    return Type::create("GLMState")
        .with(FieldsTrait()
                  .withField("mat2", &GLMState::mat2)
                  .withField("dmat2", &GLMState::dmat2)
                  .withField("mat2x3", &GLMState::mat2x3)
                  .withField("dmat2x3", &GLMState::dmat2x3)
                  .withField("mat2x4", &GLMState::mat2x4)
                  .withField("dmat2x4", &GLMState::dmat2x4)
                  .withField("mat3x2", &GLMState::mat3x2)
                  .withField("dmat3x2", &GLMState::dmat3x2)
                  .withField("mat3", &GLMState::mat3)
                  .withField("dmat3", &GLMState::dmat3)
                  .withField("mat3x4", &GLMState::mat3x4)
                  .withField("dmat3x4", &GLMState::dmat3x4)
                  .withField("mat4x2", &GLMState::mat4x2)
                  .withField("dmat4x2", &GLMState::dmat4x2)
                  .withField("mat4x3", &GLMState::mat4x3)
                  .withField("dmat4x3", &GLMState::dmat4x3)
                  .withField("mat4", &GLMState::mat4)
                  .withField("dmat4", &GLMState::dmat4)
                  .withField("vec2", &GLMState::vec2)
                  .withField("bvec2", &GLMState::bvec2)
                  .withField("dvec2", &GLMState::dvec2)
                  .withField("ivec2", &GLMState::ivec2)
                  .withField("uvec2", &GLMState::uvec2)
                  .withField("vec3", &GLMState::vec3)
                  .withField("bvec3", &GLMState::bvec3)
                  .withField("dvec3", &GLMState::dvec3)
                  .withField("ivec3", &GLMState::ivec3)
                  .withField("uvec3", &GLMState::uvec3)
                  .withField("vec4", &GLMState::vec4)
                  .withField("bvec4", &GLMState::bvec4)
                  .withField("dvec4", &GLMState::dvec4)
                  .withField("ivec4", &GLMState::ivec4)
                  .withField("uvec4", &GLMState::uvec4)
                  .withField("quat", &GLMState::quat)
                  .withField("dquat", &GLMState::dquat))
        .with(ConstructibleTrait::typed<GLMState>().withDefaultConstructor().build());
}

struct State
{
    CUBOS_REFLECT;

    bool boolPrimitive{true};
    char charPrimitive{'A'};
    signed char signedCharPrimitive{1};
    short shortPrimitive{-123};
    int intPrimitive{-321};
    long longPrimitive{12345};
    long long longLongPrimitive{987654321};
    unsigned char unsignedCharPrimitive{5};
    unsigned short unsignedShortPrimitive{55};
    unsigned int unsignedIntPrimitive{555};
    unsigned long unsignedLongPrimitive{5555};
    unsigned long long unsignedLongLongPrimitive{55555};
    float floatPrimitive{3.14159265359F};
    double doublePrimitive{2.718281828459045235360287471352};
    std::string string{"Hello, ImGui!"};
    MyUnsupported myUnsupported;
    MyEnum myEnum{MyEnum::Foo};
    MyMask myMask{MyMask::Flag1 | MyMask::Flag3};
    MyStruct myStruct;
    std::vector<int> intVector{1, 2, 3, 4, 5};
    std::vector<MyStruct> myStructVector{{1, 0.1F}, {2, 0.2F}};
    std::map<int, int> intIntMap{{1, 2}, {2, 1}, {3, 4}, {4, 3}};
    std::map<MyEnum, MyStruct> enumStructMap{
        {MyEnum::Foo, {1, 0.1F}},
        {MyEnum::Bar, {2, 0.2F}},
    };
    std::map<int, std::map<int, int>> intIntIntMap{
        {1, {{1, 2}, {2, 3}}},
        {2, {{3, 5}, {4, 6}}},
    };
    GLMState glm{};
};

CUBOS_REFLECT_IMPL(State)
{
    return Type::create("State")
        .with(FieldsTrait()
                  .withField("boolPrimitive", &State::boolPrimitive)
                  .withField("charPrimitive", &State::charPrimitive)
                  .withField("signedCharPrimitive", &State::signedCharPrimitive)
                  .withField("shortPrimitive", &State::shortPrimitive)
                  .withField("intPrimitive", &State::intPrimitive)
                  .withField("longPrimitive", &State::longPrimitive)
                  .withField("longLongPrimitive", &State::longLongPrimitive)
                  .withField("unsignedCharPrimitive", &State::unsignedCharPrimitive)
                  .withField("unsignedShortPrimitive", &State::unsignedShortPrimitive)
                  .withField("unsignedIntPrimitive", &State::unsignedIntPrimitive)
                  .withField("unsignedLongPrimitive", &State::unsignedLongPrimitive)
                  .withField("unsignedLongLongPrimitive", &State::unsignedLongLongPrimitive)
                  .withField("floatPrimitive", &State::floatPrimitive)
                  .withField("doublePrimitive", &State::doublePrimitive)
                  .withField("string", &State::string)
                  .withField("myUnsupported", &State::myUnsupported)
                  .withField("myEnum", &State::myEnum)
                  .withField("myMask", &State::myMask)
                  .withField("myStruct", &State::myStruct)
                  .withField("intVector", &State::intVector)
                  .withField("myStructVector", &State::myStructVector)
                  .withField("intIntMap", &State::intIntMap)
                  .withField("enumStructMap", &State::enumStructMap)
                  .withField("intIntIntMap", &State::intIntIntMap)
                  .withField("glm", &State::glm))
        .with(ConstructibleTrait::typed<State>().withDefaultConstructor().build());
}

int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};

    cubos.plugin(settingsPlugin);
    cubos.plugin(windowPlugin);
    cubos.plugin(renderTargetPlugin);
    cubos.plugin(assetsPlugin);
    cubos.plugin(fontPlugin);
    /// [Adding the plugin]
    cubos.plugin(imguiPlugin);
    /// [Adding the plugin]

    cubos.startupSystem("configure settings").before(settingsTag).call([](Settings& settings) {
        settings.setString("assets.app.osPath", APP_ASSETS_PATH);
        settings.setString("assets.builtin.osPath", BUILTIN_ASSETS_PATH);
    });

    /// [ImGui initialization]
    cubos.startupSystem("set ImGui context").after(imguiInitTag).call([](ImGuiContextHolder& holder) {
        ImGui::SetCurrentContext(holder.context);
    });
    /// [ImGui initialization]

    /// [ImGui Demo]
    cubos.system("show ImGui demo").tagged(imguiTag).call([]() { ImGui::ShowDemoWindow(); });
    /// [ImGui Demo]

    cubos.resource<State>();

    /// [ImGuiInspector window example]
    cubos.system("data inspector example").tagged(imguiTag).call([](ImGuiInspector inspector, State& state) {
        ImGui::Begin("ImGui Inspector");
        inspector.show("State (Show)", state);
        inspector.edit("State (Edit)", state);
        ImGui::End();
    });
    /// [ImGuiInspector window example]

    cubos.run();
}
