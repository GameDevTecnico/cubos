#include <gtest/gtest.h>
#include <cubos/settings.hpp>

const std::string key1 = "key 1";
const std::string key2 = "key 2";
const std::string correct_value = "right";
const std::string incorrect_value = "wrong";
const std::string default_value = "default";

TEST(Cubos_Settings_Setters_and_Getters_Tests, Get_String_When_Setting_Was_Added)
{
    auto settings = cubos::Settings();
    settings.setString(key1, correct_value);

    const std::string& returnedValue = settings.getString(key1, "");

    EXPECT_EQ(returnedValue, correct_value);
}

TEST(Cubos_Settings_Setters_and_Getters_Tests, Get_Default_String_When_Setting_Was_Not_Added)
{
    auto settings = cubos::Settings();

    const std::string& returned_value = settings.getString(key1, default_value);

    EXPECT_EQ(returned_value, default_value);
}

TEST(Cubos_Settings_Setters_and_Getters_Tests, Get_Last_String_After_Multiple_Sets_With_Same_Key)
{
    auto settings = cubos::Settings();
    settings.setString(key1, incorrect_value);
    settings.setString(key1, correct_value);

    const std::string& returned_value = settings.getString(key1, default_value);

    EXPECT_EQ(returned_value, correct_value);
}

TEST(Cubos_Settings_Setters_and_Getters_Tests, Get_Default_String_With_Wrong_Key)
{
    auto settings = cubos::Settings();
    settings.setString(key1, incorrect_value);

    const std::string& returned_value = settings.getString(key2, default_value);

    EXPECT_EQ(returned_value, default_value);
}
