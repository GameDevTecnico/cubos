#include <gtest/gtest.h>

#include <cubos/core/data/package.hpp>

using namespace cubos::core::memory;
using namespace cubos::core::data;

struct Fruit
{
    std::string name;
    uint16_t price;
    double weight;
};

namespace cubos::core::data
{
    static void serialize(Serializer& s, const Fruit& fruit, const char* name)
    {
        s.beginObject(name);
        s.write(fruit.name, "name");
        s.write(fruit.price, "price");
        s.write(fruit.weight, "weight");
        s.endObject();
    }

    static void deserialize(Deserializer& d, Fruit& fruit)
    {
        d.beginObject();
        d.read(fruit.name);
        d.read(fruit.price);
        d.read(fruit.weight);
        d.endObject();
    }
} // namespace cubos::core::data

TEST(Cubos_Core_Data_Package, Packaging_And_Unpackaging)
{
    // Package an apple.
    Fruit apple = {"apple", 100, 0.5};
    auto pkg = Package::from(apple);
    EXPECT_EQ(pkg.size(), 3);
    EXPECT_STREQ(pkg.field("name").get<std::string>().c_str(), "apple");
    EXPECT_EQ(pkg.field("price").get<uint16_t>(), 100);
    EXPECT_EQ(pkg.field("weight").get<double>(), 0.5);

    // Change it to a banana.
    pkg.field("name").set("banana");
    EXPECT_STREQ(pkg.field("name").get<std::string>().c_str(), "banana");
    Fruit banana;
    pkg.into(banana);
    EXPECT_STREQ(banana.name.c_str(), "banana");
    EXPECT_EQ(banana.price, 100);
    EXPECT_EQ(banana.weight, 0.5);

    // Package a list of fruits.
    std::vector<Fruit> list = {
        {"orange", 100, 1.5},
        {"apple", 200, 2.5},
    };
    auto listPkg = Package::from(list);
    EXPECT_EQ(listPkg.size(), 2);
    EXPECT_STREQ(listPkg.element(0).field("name").get<std::string>().c_str(), "orange");
    EXPECT_STREQ(listPkg.element(1).field("name").get<std::string>().c_str(), "apple");

    // Add the banana to the list.
    listPkg.elements().push_back(pkg);
    EXPECT_EQ(listPkg.size(), 3);
    EXPECT_STREQ(listPkg.element(0).field("name").get<std::string>().c_str(), "orange");
    EXPECT_STREQ(listPkg.element(1).field("name").get<std::string>().c_str(), "apple");
    EXPECT_STREQ(listPkg.element(2).field("name").get<std::string>().c_str(), "banana");

    // Unpackage the list.
    listPkg.into(list);
    EXPECT_EQ(list.size(), 3);
    EXPECT_STREQ(list[0].name.c_str(), "orange");
    EXPECT_STREQ(list[1].name.c_str(), "apple");
    EXPECT_STREQ(list[2].name.c_str(), "banana");
}
