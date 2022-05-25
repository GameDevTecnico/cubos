#include <gtest/gtest.h>

#include <cubos/core/memory/buffer_stream.hpp>
#include <cubos/core/data/yaml_serializer.hpp>
#include <cubos/core/data/yaml_deserializer.hpp>
#include <cubos/core/data/serialization_map.hpp>

#include <random>

using namespace cubos::core::data;
using namespace cubos::core::memory;

struct Human
{
    std::string name;
    int age;
    double weight;
    bool dead;
    std::vector<Human*> children;
};

namespace cubos::core::data
{
    static void serialize(Serializer& s, const Human& human, const char* name)
    {
        s.beginObject(name);
        s.write(human.name, "name");
        s.write(human.age, "age");
        s.write(human.weight, "weight");
        s.write(human.dead, "dead");
        s.beginArray(human.children.size(), "children");
        for (auto& child : human.children)
            s.write(*child, "child");
        s.endArray();
        s.endObject();
    }

    static void serialize(Serializer& s, const Human& human, SerializationMap<Human*, size_t>* map, const char* name)
    {
        s.beginObject(name);
        s.write(human.name, "name");
        s.write(human.age, "age");
        s.write(human.weight, "weight");
        s.write(human.dead, "dead");
        s.beginArray(human.children.size(), "children");
        for (auto& child : human.children)
            s.write(static_cast<uint64_t>(map->getId(child)), "child");
        s.endArray();
        s.endObject();
    }

    static void deserialize(Deserializer& s, Human& human)
    {
        s.beginObject();
        s.read(human.name);
        s.read(human.age);
        s.read(human.weight);
        s.read(human.dead);
        human.children.resize(s.beginArray(), nullptr);
        for (auto& child : human.children)
        {
            child = new Human();
            s.read(*child);
        }
        s.endArray();
        s.endObject();
    }

    static void deserialize(Deserializer& s, Human& human, SerializationMap<Human*, size_t>* map)
    {
        s.beginObject();
        s.read(human.name);
        s.read(human.age);
        s.read(human.weight);
        s.read(human.dead);
        human.children.resize(s.beginArray());
        for (auto& child : human.children)
        {
            uint64_t id;
            s.read(id);
            child = map->getRef(static_cast<size_t>(id));
        }
        s.endArray();
        s.endObject();
    }
} // namespace cubos::core::data

TEST(Cubos_Memory_YAML_Serialization_And_Deserialization, Trivial_Types)
{
    Human src = {"John",
                 30,
                 80.5,
                 false,
                 {new Human{"Jack",
                            20,
                            70.5,
                            false,
                            {new Human{"Jill", 10, 60.5, false, {}}, new Human{"Jill", 10, 60.5, false, {}}}}}};
    Human dst;

    char buf[4096];

    {

        BufferStream stream(buf, sizeof(buf));
        auto serializer = (Serializer*)new YAMLSerializer(stream);
        serializer->write(src, "human");
        delete serializer;
    }

    {
        BufferStream stream(buf, sizeof(buf));
        auto deserializer = (Deserializer*)new YAMLDeserializer(stream);
        deserializer->read(dst, "human");
        delete deserializer;
    }

    EXPECT_EQ(src.name, dst.name);
    EXPECT_EQ(src.age, dst.age);
    EXPECT_EQ(src.weight, dst.weight);
    EXPECT_EQ(src.dead, dst.dead);
    EXPECT_EQ(src.children.size(), dst.children.size());
    EXPECT_EQ(src.children[0]->name, dst.children[0]->name);
    EXPECT_EQ(src.children[0]->age, dst.children[0]->age);
    EXPECT_EQ(src.children[0]->weight, dst.children[0]->weight);
    EXPECT_EQ(src.children[0]->dead, dst.children[0]->dead);
    EXPECT_EQ(src.children[0]->children.size(), dst.children[0]->children.size());
    EXPECT_EQ(src.children[0]->children[0]->name, dst.children[0]->children[0]->name);
    EXPECT_EQ(src.children[0]->children[0]->age, dst.children[0]->children[0]->age);
    EXPECT_EQ(src.children[0]->children[0]->weight, dst.children[0]->children[0]->weight);
    EXPECT_EQ(src.children[0]->children[0]->dead, dst.children[0]->children[0]->dead);
    EXPECT_EQ(src.children[0]->children[0]->children.size(), dst.children[0]->children[0]->children.size());
    EXPECT_EQ(src.children[0]->children[1]->name, dst.children[0]->children[1]->name);
    EXPECT_EQ(src.children[0]->children[1]->age, dst.children[0]->children[1]->age);
    EXPECT_EQ(src.children[0]->children[1]->weight, dst.children[0]->children[1]->weight);
    EXPECT_EQ(src.children[0]->children[1]->dead, dst.children[0]->children[1]->dead);
    EXPECT_EQ(src.children[0]->children[1]->children.size(), dst.children[0]->children[1]->children.size());
}

TEST(Cubos_Memory_YAML_Serialization_And_Deserialization, Context_Types)
{
    std::vector<Human> src = {
        {"John", 30, 80.5, false, {}},
        {"Jack", 20, 70.5, false, {}},
        {"Jill", 10, 60.5, false, {}},
        {"Jill", 10, 60.5, false, {}},
    };
    src[0].children.push_back(&src[1]);
    src[1].children.push_back(&src[2]);
    src[1].children.push_back(&src[3]);
    std::vector<Human> dst;

    char buf[4096];

    {
        auto map = SerializationMap<Human*, size_t>();
        for (size_t i = 0; i < src.size(); ++i)
            map.add(&src[i], i);

        BufferStream stream(buf, sizeof(buf));
        auto serializer = (Serializer*)new YAMLSerializer(stream);
        serializer->beginDictionary(src.size(), "humans");
        for (size_t i = 0; i < src.size(); ++i)
        {
            serializer->write(static_cast<uint64_t>(i), nullptr);
            serializer->write(src[i], &map, nullptr);
        }
        serializer->endDictionary();
        delete serializer;
    }

    {
        BufferStream stream(buf, sizeof(buf));
        auto deserializer = (Deserializer*)new YAMLDeserializer(stream);
        auto map = SerializationMap<Human*, size_t>();

        dst.resize(deserializer->beginDictionary());
        for (size_t i = 0; i < dst.size(); ++i)
            map.add(&dst[i], i);
        for (size_t i = 0; i < dst.size(); ++i)
        {
            uint64_t id;
            deserializer->read(id);
            deserializer->read(dst[id], &map);
        }

        delete deserializer;
    }

    EXPECT_EQ(src[0].name, dst[0].name);
    EXPECT_EQ(src[0].age, dst[0].age);
    EXPECT_EQ(src[0].weight, dst[0].weight);
    EXPECT_EQ(src[0].dead, dst[0].dead);
    EXPECT_EQ(src[0].children.size(), dst[0].children.size());
    EXPECT_EQ(src[0].children[0]->name, dst[0].children[0]->name);
    EXPECT_EQ(src[0].children[0]->age, dst[0].children[0]->age);
    EXPECT_EQ(src[0].children[0]->weight, dst[0].children[0]->weight);
    EXPECT_EQ(src[0].children[0]->dead, dst[0].children[0]->dead);
    EXPECT_EQ(src[0].children[0]->children.size(), dst[0].children[0]->children.size());
    EXPECT_EQ(src[0].children[0]->children[0]->name, dst[0].children[0]->children[0]->name);
    EXPECT_EQ(src[0].children[0]->children[0]->age, dst[0].children[0]->children[0]->age);
    EXPECT_EQ(src[0].children[0]->children[0]->weight, dst[0].children[0]->children[0]->weight);
    EXPECT_EQ(src[0].children[0]->children[0]->dead, dst[0].children[0]->children[0]->dead);
    EXPECT_EQ(src[0].children[0]->children[0]->children.size(), dst[0].children[0]->children[0]->children.size());
    EXPECT_EQ(src[0].children[0]->children[1]->name, dst[0].children[0]->children[1]->name);
    EXPECT_EQ(src[0].children[0]->children[1]->age, dst[0].children[0]->children[1]->age);
    EXPECT_EQ(src[0].children[0]->children[1]->weight, dst[0].children[0]->children[1]->weight);
    EXPECT_EQ(src[0].children[0]->children[1]->dead, dst[0].children[0]->children[1]->dead);
    EXPECT_EQ(src[0].children[0]->children[1]->children.size(), dst[0].children[0]->children[1]->children.size());
}
