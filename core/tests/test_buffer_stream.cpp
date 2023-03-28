#include <gtest/gtest.h>
#include <cubos/core/memory/buffer_stream.hpp>

#include <random>
#include <string>
#include <array>

TEST(Cubos_Memory_Buffer_Stream, Parse_Printed_Integers)
{
    using namespace cubos::core::memory;

    srand(1); // Seed the number random generation, so that the tests always produce the same results

    char buf[256];
    for (size_t i = 0; i < 1000; ++i)
    {
        int64_t x = (static_cast<int64_t>(rand()) * static_cast<int64_t>(rand())) -
                    (static_cast<int64_t>(rand()) * static_cast<int64_t>(rand()));
        int64_t y = 0;

        auto stream = BufferStream(buf, sizeof(buf));
        auto base = i % 14 + 2; // Bases between 2 and 16
        stream.print(x, base);
        stream.put('\0');
        stream.seek(0, SeekOrigin::Begin);
        stream.parse(y, base);

        EXPECT_EQ(x, y);
    }
}

TEST(Cubos_Memory_Buffer_Stream, Parse_Printed_Floats)
{
    using namespace cubos::core::memory;

    srand(1); // Seed the number random generation, so that the tests always produce the same results

    char buf[256];
    for (size_t i = 0; i < 1000; ++i)
    {
        double x = static_cast<double>(static_cast<int64_t>(rand()) * static_cast<int64_t>(rand())) /
                   static_cast<double>(static_cast<int64_t>(rand()) * static_cast<int64_t>(rand())) *
                   ((rand() % 2) ? -1.0 : 1.0);
        double y = 0.0;

        // Decimal places between 0 and 9
        auto places = rand() % 10;

        auto stream = BufferStream(buf, sizeof(buf));
        stream.print(x, static_cast<size_t>(places));
        stream.put('\0');
        stream.seek(0, SeekOrigin::Begin);
        stream.parse(y);

        EXPECT_NEAR(x, y, pow(10.0, static_cast<double>(-places)));
    }
}

TEST(Cubos_Memory_Buffer_Stream, Print_String)
{
    using namespace cubos::core::memory;

    const char* str = "test string\n";
    char buf[256];
    auto stream = BufferStream(buf, sizeof(buf));

    stream.print(str);
    stream.put('\0');
    EXPECT_STREQ(str, buf);

    stream.seek(0, SeekOrigin::Begin);
    stream.print(str, 4);
    EXPECT_EQ(strncmp(str, buf, 4), 0);
}

TEST(Cubos_Memory_Buffer_Stream, Print_Formatted_String)
{
    using namespace cubos::core::memory;

    char buf[256];
    auto stream = BufferStream(buf, sizeof(buf));

    stream.printf("Escaped \\{}, non-escaped {}", 1, 2, 3);
    stream.put('\0');
    EXPECT_STREQ("Escaped {}, non-escaped 1", buf);
}

TEST(Cubos_Memory_Buffer_Stream, Read_Until)
{
    using namespace cubos::core::memory;

    const char* str = "testbananastr\ning\nsdaad";
    auto stream = BufferStream(str, strlen(str));

    std::string result;
    stream.readUntil(result, "banana");
    EXPECT_STREQ(result.c_str(), "test");
    stream.readUntil(result, "\n");
    EXPECT_STREQ(result.c_str(), "str");
    stream.readUntil(result, nullptr);
    EXPECT_STREQ(result.c_str(), "ing\nsdaad");
}
