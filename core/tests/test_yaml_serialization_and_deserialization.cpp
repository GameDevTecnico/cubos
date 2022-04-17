#include <gtest/gtest.h>

#include <cubos/memory/buffer_stream.hpp>
#include <cubos/memory/yaml_serializer.hpp>
#include <cubos/memory/yaml_deserializer.hpp>
#include <cubos/memory/serialization_map.hpp>

#include <random>

using namespace cubos::core::memory;

struct Human
{
    std::string name;
    int age;
    double weight;
    bool dead;
    std::vector<Human*> children;

    void serialize(Serializer& s) const
    {
        s.write(this->name, "name");
        s.write(this->age, "age");
        s.write(this->weight, "weight");
        s.write(this->dead, "dead");
        s.beginArray(this->children.size(), "children");
        for (auto& child : this->children)
            s.write(*child, "child");
        s.endArray();
    }

    void serialize(Serializer& s, SerializationMap<Human*, size_t>* map) const
    {
        s.write(this->name, "name");
        s.write(this->age, "age");
        s.write(this->weight, "weight");
        s.write(this->dead, "dead");
        s.beginArray(this->children.size(), "children");
        for (auto& child : this->children)
            s.write(static_cast<uint64_t>(map->getId(child)), "child");
        s.endArray();
    }

    void deserialize(Deserializer& s)
    {
        s.read(this->name);
        s.read(this->age);
        s.read(this->weight);
        s.read(this->dead);
        this->children.resize(s.beginArray(), nullptr);
        for (auto& child : this->children)
        {
            child = new Human();
            s.read(*child);
        }
        s.endArray();
    }

    void deserialize(Deserializer& s, SerializationMap<Human*, size_t>* map)
    {
        s.read(this->name);
        s.read(this->age);
        s.read(this->weight);
        s.read(this->dead);
        this->children.resize(s.beginArray());
        for (auto& child : this->children)
        {
            uint64_t id;
            s.read(id);
            child = map->getRef(static_cast<size_t>(id));
        }
        s.endArray();
    }
};

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
