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
    int foo{3};
    float bar{0.14F};
};

CUBOS_REFLECT_IMPL(MyStruct)
{
    return Type::create("MyStruct")
        .with(FieldsTrait{}.withField("foo", &MyStruct::foo).withField("bar", &MyStruct::bar))
        .with(ConstructibleTrait::typed<MyStruct>().withBasicConstructors().build());
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

    bool bool_primitive{true};
    char char_primitive{'A'};
    signed char signed_char_primitive{1};
    short short_primitive{-123};
    int int_primitive{-321};
    long long_primitive{12345};
    long long long_long_primitive{987654321};
    unsigned char unsigned_char_primitive{5};
    unsigned short unsigned_short_primitive{55};
    unsigned int unsigned_int_primitive{555};
    unsigned long unsigned_long_primitive{5555};
    unsigned long long unsigned_long_long_primitive{55555};
    float float_primitive{3.14159265359F};
    double double_primitive{2.718281828459045235360287471352};
    std::string string{"Hello, ImGui!"};
    MyUnsupported my_unsupported;
    MyEnum my_enum{MyEnum::Foo};
    MyMask my_mask{MyMask::Flag1 | MyMask::Flag3};
    MyStruct my_struct;
    std::vector<int> int_vector{1, 2, 3, 4, 5};
    std::vector<MyStruct> my_struct_vector{{1, 0.1F}, {2, 0.2F}};
    std::map<int, int> int_int_map{{1, 2}, {2, 1}, {3, 4}, {4, 3}};
    std::map<MyEnum, MyStruct> enum_struct_map{
        {MyEnum::Foo, {1, 0.1F}},
        {MyEnum::Bar, {2, 0.2F}},
    };
    std::map<int, std::map<int, int>> int_int_int_map{
        {1, {{1, 2}, {2, 3}}},
        {2, {{3, 5}, {4, 6}}},
    };
    GLMState glm{};
};

CUBOS_REFLECT_IMPL(State)
{
    return Type::create("State")
        .with(FieldsTrait()
                  .withField("bool_primitive", &State::bool_primitive)
                  .withField("char_primitive", &State::char_primitive)
                  .withField("signed_char_primitive", &State::signed_char_primitive)
                  .withField("short_primitive", &State::short_primitive)
                  .withField("int_primitive", &State::int_primitive)
                  .withField("long_primitive", &State::long_primitive)
                  .withField("long_long_primitive", &State::long_long_primitive)
                  .withField("unsigned_char_primitive", &State::unsigned_char_primitive)
                  .withField("unsigned_short_primitive", &State::unsigned_short_primitive)
                  .withField("unsigned_int_primitive", &State::unsigned_int_primitive)
                  .withField("unsigned_long_primitive", &State::unsigned_long_primitive)
                  .withField("unsigned_long_long_primitive", &State::unsigned_long_long_primitive)
                  .withField("float_primitive", &State::float_primitive)
                  .withField("double_primitive", &State::double_primitive)
                  .withField("string", &State::string)
                  .withField("my_unsupported", &State::my_unsupported)
                  .withField("my_enum", &State::my_enum)
                  .withField("my_mask", &State::my_mask)
                  .withField("my_struct", &State::my_struct)
                  .withField("int_vector", &State::int_vector)
                  .withField("my_struct_vector", &State::my_struct_vector)
                  .withField("int_int_map", &State::int_int_map)
                  .withField("enum_struct_map", &State::enum_struct_map)
                  .withField("int_int_int_map", &State::int_int_int_map)
                  .withField("glm", &State::glm))
        .with(ConstructibleTrait::typed<State>().withDefaultConstructor().build());
}

int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};

    cubos.plugin(settingsPlugin);
    cubos.plugin(windowPlugin);
    cubos.plugin(renderTargetPlugin);
    /// [Adding the plugin]
    cubos.plugin(imguiPlugin);
    /// [Adding the plugin]

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
