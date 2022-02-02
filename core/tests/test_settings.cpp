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

TEST(Cubos_Settings_Merge_Tests, Get_Original_Settings_String_After_Merge_With_Empty_Settings)
{
    auto original_settings = cubos::Settings();
    auto to_join_settings = cubos::Settings();

    original_settings.setString(key1, correct_value);
    original_settings.merge(to_join_settings);

    const std::string& returned_value = original_settings.getString(key1, default_value);

    EXPECT_EQ(returned_value, correct_value);
}

TEST(Cubos_Settings_Merge_Tests, Get_New_Settings_String_After_Merge_With_Populated_Settings)
{
    auto original_settings = cubos::Settings();
    auto to_join_settings = cubos::Settings();

    original_settings.setString(key1, incorrect_value);
    to_join_settings.setString(key1, correct_value);

    original_settings.merge(to_join_settings);

    const std::string& returned_value = original_settings.getString(key1, default_value);

    EXPECT_EQ(returned_value, correct_value);
}

TEST(Cubos_Settings_Merge_Tests, Get_Updated_Settings_String_After_Merge_With_Populated_Settings)
{
    auto original_settings = cubos::Settings();
    auto to_join_settings = cubos::Settings();

    original_settings.setString(key1, incorrect_value);
    to_join_settings.setString(key1, incorrect_value);

    original_settings.merge(to_join_settings);

    original_settings.setString(key1, correct_value);

    const std::string& returned_value = original_settings.getString(key1, default_value);

    EXPECT_EQ(returned_value, correct_value);
}

TEST(Cubos_Settings_Merge_Tests, Get_Merged_Settings_String_After_Changed_Merged_Settings)
{
    auto original_settings = cubos::Settings();
    auto to_join_settings = cubos::Settings();

    original_settings.setString(key1, incorrect_value);
    to_join_settings.setString(key1, correct_value);

    original_settings.merge(to_join_settings);

    to_join_settings.setString(key1, incorrect_value);

    const std::string& returned_value = original_settings.getString(key1, default_value);

    EXPECT_EQ(returned_value, correct_value);
}

TEST(Cubos_Settings_Global_Settings_Tests, Get_Empty_Global_Settings)
{
    const std::string& returned_value = cubos::Settings::global.getString(key1, default_value);

    EXPECT_EQ(returned_value, default_value);
}

TEST(Cubos_Settings_Global_Settings_Tests, Get_String_When_Setting_Was_Added)
{

    cubos::Settings::global.clear();
    cubos::Settings::global.setString(key1, correct_value);

    const std::string& returned_value = cubos::Settings::global.getString(key1, default_value);

    EXPECT_EQ(returned_value, correct_value);
}

TEST(Cubos_Settings_Global_Settings_Tests, Get_String_After_Merge_With_Empty_Settings)
{
    cubos::Settings::global.clear();

    auto settings_to_merge = cubos::Settings();

    const std::string& returned_value = cubos::Settings::global.getString(key1, default_value);

    EXPECT_EQ(returned_value, default_value);
}

TEST(Cubos_Settings_Global_Settings_Tests, Get_String_After_Merge_With_Populated_Settings)
{
    cubos::Settings::global.clear();

    auto settings_to_merge = cubos::Settings();
    settings_to_merge.setString(key1, correct_value);
    cubos::Settings::global.merge(settings_to_merge);

    const std::string& returned_value = cubos::Settings::global.getString(key1, default_value);

    EXPECT_EQ(returned_value, correct_value);
}