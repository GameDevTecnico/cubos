/// [Including plugin headers]
#include <iostream>
#include <map>
#include <vector>

#include <imgui.h>

#include <cubos/core/reflection/external/map.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/vector.hpp>
#include <cubos/core/reflection/reflect.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/traits/nullable.hpp>

#include <cubos/engine/imgui/data_inspector.hpp>
#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/render/target/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

/// [Including plugin headers]

using cubos::core::reflection::ConstructibleTrait;
using cubos::core::reflection::FieldsTrait;
using cubos::core::reflection::NullableTrait;
using cubos::core::reflection::reflect;
using cubos::core::reflection::Type;

using cubos::engine::DataInspector;

using namespace cubos::engine;

/// [Creating a dummy resource]
struct Person
{
    CUBOS_REFLECT;
    std::string name;
    int32_t age;
    float weight;
    bool dead;
};

CUBOS_REFLECT_IMPL(Person)
{
    return Type::create("Person")
        .with(FieldsTrait()
                  .withField("name", &Person::name)
                  .withField("age", &Person::age)
                  .withField("weight", &Person::weight)
                  .withField("dead", &Person::dead))
        .with(NullableTrait{[](const void* instance) {
                                const auto* person = static_cast<const Person*>(instance);
                                return person->dead;
                            },
                            [](void* instance) {
                                auto* person = static_cast<Person*>(instance);
                                person->dead = true;
                            }})
        .with(ConstructibleTrait::typed<Person>().withDefaultConstructor().build());
}

struct DummyResource
{
    CUBOS_REFLECT;

    int integer;
    Person person;
    std::vector<Person> persons;
    std::vector<int32_t> vec;
    std::map<int32_t, int32_t> map;
};

CUBOS_REFLECT_IMPL(DummyResource)
{
    return Type::create("DummyResource")
        .with(FieldsTrait()
                  .withField("integer", &DummyResource::integer)
                  .withField("person", &DummyResource::person)
                  .withField("persons", &DummyResource::persons)
                  .withField("vec", &DummyResource::vec)
                  .withField("v", &DummyResource::map))
        .with(ConstructibleTrait::typed<DummyResource>().withDefaultConstructor().build());
}
/// [Creating a dummy resource]

int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};

    /// [Adding the plugin]
    cubos.plugin(settingsPlugin);
    cubos.plugin(windowPlugin);
    cubos.plugin(renderTargetPlugin);
    cubos.plugin(imguiPlugin);
    /// [Adding the plugin]

    /// [ImGui initialization]
    cubos.startupSystem("set ImGui context").after(imguiInitTag).call([](ImGuiContextHolder& holder) {
        ImGui::SetCurrentContext(holder.context);
    });
    /// [ImGui initialization]

    /// [ImGui Demo]
    cubos.system("show ImGui demo").tagged(imguiTag).call([]() {
        ImGui::Begin("Dear ImGui + Cubos");
        ImGui::Text("Hello world!");
        ImGui::End();

        ImGui::ShowDemoWindow();
    });
    /// [ImGui Demo]

    /// [Filling the dummy resource]
    cubos.resource<DummyResource>(
        DummyResource{.integer = 1337,
                      .person = Person{"roby", 1337, 666.5F, false},
                      .persons{Person{"roby", 1337, 666.5F, false}, Person{"riscado", 123, 321.0F, false}},
                      .vec = {12, 59, 25},
                      .map = {
                          {1, 2},
                          {2, 4},
                          {3, 6},
                          {4, 8},
                      }});
    /// [Filling the dummy resource]

    /// [DataInspector window example]
    cubos.system("data inspector example").tagged(imguiTag).call([](DataInspector& inspector, DummyResource& data) {
        ImGui::Begin("Data Inspector");
        inspector.edit(data);
        ImGui::End();
    });
    /// [DataInspector window example]

    cubos.run();
}
