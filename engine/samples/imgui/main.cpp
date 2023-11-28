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
    int integer;
    Person person;
    std::vector<Person> persons;
    std::vector<int32_t> vec;
    std::map<int32_t, int32_t> map;
};
/// [Creating a dummy resource]

/// [DataInspector window example]
static void exampleDataInspectorSystem(Write<DataInspector> inspector, Write<DummyResource> data)
{
    ImGui::Begin("Data Inspector");
    ImGui::BeginTable("id1", 2);

    inspector->show("data->integer", data->integer);
    inspector->edit("data->person", data->person);
    inspector->edit("data->persons", data->persons);
    inspector->edit("data->vec", data->vec);
    inspector->edit("data->map", data->map);

    ImGui::EndTable();
    ImGui::End();
}
/// [DataInspector window example]

/// [ImGui window example]
static void exampleWindowSystem()
{
    ImGui::Begin("Dear ImGui + CUBOS.");
    ImGui::Text("Hello world!");
    ImGui::End();

    ImGui::ShowDemoWindow();
}
/// [ImGui window example]

int main()
{
    Cubos cubos{};

    /// [Adding the plugin]
    cubos.addPlugin(imguiPlugin);
    /// [Adding the plugin]

    /// [Adding the system]
    cubos.system(exampleWindowSystem).tagged("cubos.imgui");
    /// [Adding the system]

    /// [Filling the dummy resource 1]
    cubos.addResource<DummyResource>(
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
    /// [Filling the dummy resource 1]

    /// [Adding the system and a dummy resource with data]
    cubos.system(exampleDataInspectorSystem).tagged("cubos.imgui");
    /// [Adding the system and a dummy resource with data]

    cubos.run();
}
