#include <gtest/gtest.h>
#include <cubos/core/settings.hpp>

const std::string key1 = "key 1";
const std::string key2 = "key 2";
const std::string correct_string_value = "right";
const std::string incorrect_string_value = "wrong";
const std::string default_string_value = "default";

const int correct_integer_value = 10;
const int incorrect_integer_value = 20;
const int default_integer_value = 30;

const double correct_double_value = 1.05;
const double incorrect_double_value = 2.05;
const double default_double_value = 3.05;

TEST(Cubos_String_Settings_Setters_and_Getters_Tests, Get_String_When_Setting_Was_Added)
{
    auto settings = cubos::core::Settings();
    settings.setString(key1, correct_string_value);

    const std::string& returnedValue = settings.getString(key1, "");

    EXPECT_EQ(returnedValue, correct_string_value);
}

TEST(Cubos_String_Settings_Setters_and_Getters_Tests, Get_Default_String_When_Setting_Was_Not_Added)
{
    auto settings = cubos::core::Settings();

    const std::string& returned_value = settings.getString(key1, default_string_value);

    EXPECT_EQ(returned_value, default_string_value);
}

TEST(Cubos_String_Settings_Setters_and_Getters_Tests, Get_Last_String_After_Multiple_Sets_With_Same_Key)
{
    auto settings = cubos::core::Settings();
    settings.setString(key1, incorrect_string_value);
    settings.setString(key1, correct_string_value);

    const std::string& returned_value = settings.getString(key1, default_string_value);

    EXPECT_EQ(returned_value, correct_string_value);
}

TEST(Cubos_String_Settings_Setters_and_Getters_Tests, Get_Default_String_With_Wrong_Key)
{
    auto settings = cubos::core::Settings();
    settings.setString(key1, incorrect_string_value);

    const std::string& returned_value = settings.getString(key2, default_string_value);

    EXPECT_EQ(returned_value, default_string_value);
}

TEST(Cubos_Integer_Settings_Setters_and_Getters_Tests, Get_String_When_Setting_Was_Added)
{
    auto settings = cubos::core::Settings();
    settings.setInteger(key1, correct_integer_value);

    const int returnedValue = settings.getInteger(key1, default_integer_value);

    EXPECT_EQ(returnedValue, correct_integer_value);
}

TEST(Cubos_Integer_Settings_Setters_and_Getters_Tests, Get_Default_String_When_Setting_Was_Not_Added)
{
    auto settings = cubos::core::Settings();

    const int returned_value = settings.getInteger(key1, default_integer_value);

    EXPECT_EQ(returned_value, default_integer_value);
}

TEST(Cubos_Integer_Settings_Setters_and_Getters_Tests, Get_Last_String_After_Multiple_Sets_With_Same_Key)
{
    auto settings = cubos::core::Settings();
    settings.setInteger(key1, incorrect_integer_value);
    settings.setInteger(key1, correct_integer_value);

    const int returned_value = settings.getInteger(key1, default_integer_value);

    EXPECT_EQ(returned_value, correct_integer_value);
}

TEST(Cubos_Integer_Settings_Setters_and_Getters_Tests, Get_Default_String_With_Wrong_Key)
{
    auto settings = cubos::core::Settings();
    settings.setInteger(key1, incorrect_integer_value);

    const int returned_value = settings.getInteger(key2, default_integer_value);

    EXPECT_EQ(returned_value, default_integer_value);
}

TEST(Cubos_Double_Settings_Setters_and_Getters_Tests, Get_String_When_Setting_Was_Added)
{
    auto settings = cubos::core::Settings();
    settings.setDouble(key1, correct_double_value);

    const double returnedValue = settings.getDouble(key1, default_double_value);

    EXPECT_EQ(returnedValue, correct_double_value);
}

TEST(Cubos_Double_Settings_Setters_and_Getters_Tests, Get_Default_String_When_Setting_Was_Not_Added)
{
    auto settings = cubos::core::Settings();

    const double returned_value = settings.getDouble(key1, default_double_value);

    EXPECT_EQ(returned_value, default_double_value);
}

TEST(Cubos_Double_Settings_Setters_and_Getters_Tests, Get_Last_String_After_Multiple_Sets_With_Same_Key)
{
    auto settings = cubos::core::Settings();
    settings.setDouble(key1, incorrect_double_value);
    settings.setDouble(key1, correct_double_value);

    const double returned_value = settings.getDouble(key1, default_double_value);

    EXPECT_EQ(returned_value, correct_double_value);
}

TEST(Cubos_Double_Settings_Setters_and_Getters_Tests, Get_Default_String_With_Wrong_Key)
{
    auto settings = cubos::core::Settings();
    settings.setDouble(key1, incorrect_double_value);

    const double returned_value = settings.getDouble(key2, default_double_value);

    EXPECT_EQ(returned_value, default_double_value);
}

TEST(Cubos_Settings_Merge_Tests, Get_Original_Settings_String_After_Merge_With_Empty_Settings)
{
    auto original_settings = cubos::core::Settings();
    auto to_join_settings = cubos::core::Settings();

    original_settings.setString(key1, correct_string_value);
    original_settings.merge(to_join_settings);

    const std::string& returned_value = original_settings.getString(key1, default_string_value);

    EXPECT_EQ(returned_value, correct_string_value);
}

TEST(Cubos_Settings_Merge_Tests, Get_New_Settings_String_After_Merge_With_Populated_Settings)
{
    auto original_settings = cubos::core::Settings();
    auto to_join_settings = cubos::core::Settings();

    original_settings.setString(key1, incorrect_string_value);
    to_join_settings.setString(key1, correct_string_value);

    original_settings.merge(to_join_settings);

    const std::string& returned_value = original_settings.getString(key1, default_string_value);

    EXPECT_EQ(returned_value, correct_string_value);
}

TEST(Cubos_Settings_Merge_Tests, Get_Updated_Settings_String_After_Merge_With_Populated_Settings)
{
    auto original_settings = cubos::core::Settings();
    auto to_join_settings = cubos::core::Settings();

    original_settings.setString(key1, incorrect_string_value);
    to_join_settings.setString(key1, incorrect_string_value);

    original_settings.merge(to_join_settings);

    original_settings.setString(key1, correct_string_value);

    const std::string& returned_value = original_settings.getString(key1, default_string_value);

    EXPECT_EQ(returned_value, correct_string_value);
}

TEST(Cubos_Settings_Merge_Tests, Get_Merged_Settings_String_After_Changed_Merged_Settings)
{
    auto original_settings = cubos::core::Settings();
    auto to_join_settings = cubos::core::Settings();

    original_settings.setString(key1, incorrect_string_value);
    to_join_settings.setString(key1, correct_string_value);

    original_settings.merge(to_join_settings);

    to_join_settings.setString(key1, incorrect_string_value);

    const std::string& returned_value = original_settings.getString(key1, default_string_value);

    EXPECT_EQ(returned_value, correct_string_value);
}

TEST(Cubos_Settings_Global_Settings_Tests, Get_Empty_Global_Settings)
{
    const std::string& returned_value = cubos::core::Settings::global.getString(key1, default_string_value);

    EXPECT_EQ(returned_value, default_string_value);
}

TEST(Cubos_Settings_Global_Settings_Tests, Get_String_When_Setting_Was_Added)
{

    cubos::core::Settings::global.clear();
    cubos::core::Settings::global.setString(key1, correct_string_value);

    const std::string& returned_value = cubos::core::Settings::global.getString(key1, default_string_value);

    EXPECT_EQ(returned_value, correct_string_value);
}

TEST(Cubos_Settings_Global_Settings_Tests, Get_String_After_Merge_With_Empty_Settings)
{
    cubos::core::Settings::global.clear();

    auto settings_to_merge = cubos::core::Settings();

    const std::string& returned_value = cubos::core::Settings::global.getString(key1, default_string_value);

    EXPECT_EQ(returned_value, default_string_value);
}

TEST(Cubos_Settings_Global_Settings_Tests, Get_String_After_Merge_With_Populated_Settings)
{
    cubos::core::Settings::global.clear();

    auto settings_to_merge = cubos::core::Settings();
    settings_to_merge.setString(key1, correct_string_value);
    cubos::core::Settings::global.merge(settings_to_merge);

    const std::string& returned_value = cubos::core::Settings::global.getString(key1, default_string_value);

    EXPECT_EQ(returned_value, correct_string_value);
}
