#include <cubos/core/data/serializer.hpp>
#include <cubos/core/data/serialization_map.hpp>
#include <cubos/core/data/yaml_serializer.hpp>

#include <stack>

using namespace cubos::core::memory;
using namespace cubos::core::data;

// Example serializer
class MySerializer : public Serializer
{
public:
    MySerializer(Stream& s) : stream(s)
    {
        this->dictMode.push(false);
        this->key = true;
    }

    virtual void writeU8(uint8_t x, const char* name) override
    {
        if (this->dictMode.top())
        {
            if (this->key)
                this->stream.printf("{ uint8_t: {} ; ", x);
            else
                this->stream.printf("uint8_t: {} }\n", x);
            this->key = !this->key;
        }
        else
            this->stream.printf("uint8_t {}: {}\n", name ? name : "unnamed", x);
    }

    virtual void writeU16(uint16_t x, const char* name) override
    {
        if (this->dictMode.top())
        {
            if (this->key)
                this->stream.printf("{ uint16_t: {} ; ", x);
            else
                this->stream.printf("uint16_t: {} }\n", x);
            this->key = !this->key;
        }
        else
            this->stream.printf("uint16_t {}: {}\n", name ? name : "unnamed", x);
    }

    virtual void writeU32(uint32_t x, const char* name) override
    {
        if (this->dictMode.top())
        {
            if (this->key)
                this->stream.printf("{ uint32_t: {} ; ", x);
            else
                this->stream.printf("uint32_t: {} }\n", x);
            this->key = !this->key;
        }
        else
            this->stream.printf("uint32_t {}: {}\n", name ? name : "unnamed", x);
    }

    virtual void writeU64(uint64_t x, const char* name) override
    {
        if (this->dictMode.top())
        {
            if (this->key)
                this->stream.printf("{ uint64_t: {} ; ", x);
            else
                this->stream.printf("uint64_t: {} }\n", x);
            this->key = !this->key;
        }
        else
            this->stream.printf("uint64_t {}: {}\n", name ? name : "unnamed", x);
    }

    virtual void writeI8(int8_t x, const char* name) override
    {
        if (this->dictMode.top())
        {
            if (this->key)
                this->stream.printf("{ int8_t: {} ; ", x);
            else
                this->stream.printf("int8_t: {} }\n", x);
            this->key = !this->key;
        }
        else
            this->stream.printf("int8_t {}: {}\n", name ? name : "unnamed", x);
    }

    virtual void writeI16(int16_t x, const char* name) override
    {
        if (this->dictMode.top())
        {
            if (this->key)
                this->stream.printf("{ int16_t: {} ; ", x);
            else
                this->stream.printf("int16_t: {} }\n", x);
            this->key = !this->key;
        }
        else
            this->stream.printf("int16_t {}: {}\n", name ? name : "unnamed", x);
    }

    virtual void writeI32(int32_t x, const char* name) override
    {
        if (this->dictMode.top())
        {
            if (this->key)
                this->stream.printf("{ int32_t: {} ; ", x);
            else
                this->stream.printf("int32_t: {} }\n", x);
            this->key = !this->key;
        }
        else
            this->stream.printf("int32_t {}: {}\n", name ? name : "unnamed", x);
    }

    virtual void writeI64(int64_t x, const char* name) override
    {
        if (this->dictMode.top())
        {
            if (this->key)
                this->stream.printf("{ int64_t: {} ; ", x);
            else
                this->stream.printf("int64_t: {} }\n", x);
            this->key = !this->key;
        }
        else
            this->stream.printf("int64_t {}: {}\n", name ? name : "unnamed", x);
    }

    virtual void writeF32(float x, const char* name) override
    {
        if (this->dictMode.top())
        {
            if (this->key)
                this->stream.printf("{ float: {} ; ", x);
            else
                this->stream.printf("float: {} }\n", x);
            this->key = !this->key;
        }
        else
            this->stream.printf("float {}: {}\n", name ? name : "unnamed", x);
    }

    virtual void writeF64(double x, const char* name) override
    {
        if (this->dictMode.top())
        {
            if (this->key)
                this->stream.printf("{ double: {} ; ", x);
            else
                this->stream.printf("double: {} }\n", x);
            this->key = !this->key;
        }
        else
            this->stream.printf("double {}: {}\n", name ? name : "unnamed", x);
    }

    virtual void writeBool(bool x, const char* name) override
    {
        if (this->dictMode.top())
        {
            if (this->key)
                this->stream.printf("{ bool: {} ; ", x);
            else
                this->stream.printf(" bool: {} }\n", x ? "true" : "false");
            this->key = !this->key;
        }
        else
            this->stream.printf("bool {}: {}\n", name ? name : "unnamed", x ? "true" : "false");
    }

    virtual void writeString(const char* x, const char* name) override
    {
        if (this->dictMode.top())
        {
            if (this->key)
                this->stream.printf("{ string: \"{}\" ; ", x);
            else
                this->stream.printf(" string: \"{}\" }\n", x);
            this->key = !this->key;
        }
        else
            this->stream.printf("string {}: \"{}\"\n", name ? name : "unnamed", x);
    }

    virtual void beginObject(const char* name) override
    {
        this->stream.printf("object {}: {\n", name ? name : "unnamed");
        if (this->dictMode.top())
            this->key = !this->key;
        this->dictMode.push(false);
    }

    virtual void endObject() override
    {
        this->dictMode.pop();
        if (this->dictMode.top())
            this->stream.print("}}\n");
        else
            this->stream.print("}\n");
    }

    virtual void beginArray(size_t length, const char* name) override
    {
        this->stream.printf("array[{}] {}: [\n", length, name ? name : "unnamed");
        if (this->dictMode.top())
            this->key = !this->key;
        this->dictMode.push(false);
    }

    virtual void endArray() override
    {
        this->dictMode.pop();
        if (this->dictMode.top())
            this->stream.print("]}\n");
        else
            this->stream.print("]\n");
    }

    virtual void beginDictionary(size_t length, const char* name) override
    {
        this->stream.printf("dictionary[{}] {}: [\n", length, name ? name : "unnamed");
        if (this->dictMode.top())
            this->key = !this->key;
        dictMode.push(true);
    }

    virtual void endDictionary() override
    {
        dictMode.pop();
        if (this->dictMode.top())
            this->stream.print("]}\n");
        else
            this->stream.print("]\n");
    }

private:
    Stream& stream;
    std::stack<bool> dictMode;
    bool key = false;
};

// Example serializable type which requires context
struct Human
{
    Human* father;
    Human* mother;
    int32_t age;
    std::string name;
};

// Example serializable type which doesn't require context
struct Fruit
{
    std::string name;
    int32_t weight;
};

namespace cubos::core::data
{
    void serialize(Serializer& s, const Human& human, const SerializationMap<Human*, int>* map, const char* name)
    {
        s.beginObject(name);
        s.write(map->getId(human.father), "father");
        s.write(map->getId(human.mother), "mother");
        s.write(human.age, "age");
        s.write(human.name, "name");
        s.endObject();
    }

    void serialize(Serializer& s, const Fruit& fruit, const char* name)
    {
        s.beginObject(name);
        s.write(fruit.name, "name");
        s.write(fruit.weight, "weight");
        s.endObject();
    }
} // namespace cubos::core::data

int main()
{
    Serializer* s;

    Stream::stdOut.print("Write 0 to serialize with a custom serializer, or 1 to serialize to YAML: ");
    int type;
    Stream::stdIn.parse(type);
    if (type == 0)
        s = new MySerializer(Stream::stdOut);
    else
        s = new YAMLSerializer(Stream::stdOut);

    // Dictionary of trivially serializable types (don't require context).
    std::unordered_map<int, Fruit> fruitsByPrice = {
        {50, {"Apple", 100}}, {100, {"Orange", 200}}, {150, {"Banana", 300}}, {200, {"Pear", 120}}, {30, {"Grape", 20}},
    };

    // Serialize fruitsByPrice dictionary, no context is required since its values are trivially serializable.
    Stream::stdOut.print("# Dictionary of trivially serializable types\n");
    Stream::stdOut.print("# ------------------------------------------\n");
    s->write(fruitsByPrice, "fruitsByPrice");
    s->flush();

    // Array to serialize
    std::vector<Human> humans(5);
    humans[0] = {nullptr, nullptr, 43, "John"};
    humans[1] = {&humans[0], &humans[2], 12, "Alex"};
    humans[2] = {nullptr, &humans[3], 41, "Jane"};
    humans[3] = {nullptr, nullptr, 82, "Emma"};
    humans[4] = {nullptr, &humans[4], 53, "Oliver"};

    // Prepare context (in this case its a SerializationMap, used to map references to ids)
    SerializationMap<Human*, int> sMap;
    sMap.add(nullptr, -1);
    for (size_t i = 0; i < humans.size(); ++i)
        sMap.add(&humans[i], i);

    // Serialize humans array
    Stream::stdOut.print("\n\n");
    Stream::stdOut.print("# Family tree which requires context to be serialized\n");
    Stream::stdOut.print("# ---------------------------------------------------\n");
    s->write(humans, &sMap, "family");
    s->flush();

    delete s;
    return 0;
}
