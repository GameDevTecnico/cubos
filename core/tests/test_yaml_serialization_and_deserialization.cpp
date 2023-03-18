#include <gtest/gtest.h>

#include <cubos/core/memory/buffer_stream.hpp>
#include <cubos/core/data/yaml_serializer.hpp>
#include <cubos/core/data/yaml_deserializer.hpp>
#include <cubos/core/data/serialization_map.hpp>

#include <random>

using namespace cubos::core::data;
using namespace cubos::core::memory;

struct HumanSerDes
{
    std::string name;
    int age;
    double weight;
    bool dead;
    std::vector<HumanSerDes*> children;
};

template <>
void cubos::core::data::serialize<HumanSerDes>(Serializer& s, const HumanSerDes& human, const char* name)
{
    s.beginObject(name);
    s.write(human.name, "name");
    s.write(human.age, "age");
    s.write(human.weight, "weight");
    s.write(human.dead, "dead");
    s.beginArray(human.children.size(), "children");
    for (auto& child : human.children)
    {
        if (s.context().has<SerializationMap<HumanSerDes*, size_t>>())
        {
            auto& map = s.context().get<SerializationMap<HumanSerDes*, size_t>>();
            s.write(static_cast<uint64_t>(map.getId(child)), "child");
        }
        else
        {
            s.write(*child, "child");
        }
    }
    s.endArray();
    s.endObject();
}

template <>
void cubos::core::data::deserialize<HumanSerDes>(Deserializer& s, HumanSerDes& human)
{
    s.beginObject();
    s.read(human.name);
    s.read(human.age);
    s.read(human.weight);
    s.read(human.dead);
    human.children.resize(s.beginArray());
    for (auto& child : human.children)
    {
        if (s.context().has<SerializationMap<HumanSerDes*, size_t>>())
        {
            auto& map = s.context().get<SerializationMap<HumanSerDes*, size_t>>();
            uint64_t id;
            s.read(id);
            child = map.getRef(static_cast<size_t>(id));
        }
        else
        {
            child = new HumanSerDes();
            s.read(*child);
        }
    }
    s.endArray();
    s.endObject();
}

TEST(Cubos_Memory_YAML_Serialization_And_Deserialization, Without_Context)
{
    HumanSerDes src = {"John",
                       30,
                       80.5,
                       false,
                       {new HumanSerDes{"Jack",
                                        20,
                                        70.5,
                                        false,
                                        {new HumanSerDes{"Jill", 10, 60.5, false, {}},
                                         new HumanSerDes{"Jill", 10, 60.5, false, {}}}}}};
    HumanSerDes dst;

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
        deserializer->read(dst);
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

TEST(Cubos_Memory_YAML_Serialization_And_Deserialization, With_Context)
{
    std::vector<HumanSerDes> src = {
        {"John", 30, 80.5, false, {}},
        {"Jack", 20, 70.5, false, {}},
        {"Jill", 10, 60.5, false, {}},
        {"Jill", 10, 60.5, false, {}},
    };
    src[0].children.push_back(&src[1]);
    src[1].children.push_back(&src[2]);
    src[1].children.push_back(&src[3]);
    std::vector<HumanSerDes> dst;

    char buf[4096];

    {
        auto map = SerializationMap<HumanSerDes*, size_t>();
        for (size_t i = 0; i < src.size(); ++i)
            map.add(&src[i], i);

        BufferStream stream(buf, sizeof(buf));
        auto serializer = (Serializer*)new YAMLSerializer(stream);
        serializer->context().push(std::move(map));
        serializer->beginDictionary(src.size(), "humans");
        for (size_t i = 0; i < src.size(); ++i)
        {
            serializer->write(static_cast<uint64_t>(i), nullptr);
            serializer->write(src[i], nullptr);
        }
        serializer->endDictionary();
        delete serializer;
    }

    {
        BufferStream stream(buf, sizeof(buf));
        auto deserializer = (Deserializer*)new YAMLDeserializer(stream);
        auto map = SerializationMap<HumanSerDes*, size_t>();

        dst.resize(deserializer->beginDictionary());
        for (size_t i = 0; i < dst.size(); ++i)
            map.add(&dst[i], i);
        deserializer->context().push(std::move(map));

        for (size_t i = 0; i < dst.size(); ++i)
        {
            uint64_t id;
            deserializer->read(id);
            deserializer->read(dst[id]);
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
