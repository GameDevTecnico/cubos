#include <gtest/gtest.h>
#include <cubos/core/settings.hpp>

using namespace cubos::core;

const std::string Key1 = "key 1";
const std::string Key2 = "key 2";
const std::string CorrectStringValue = "right";
const std::string IncorrectStringValue = "wrong";
const std::string DefaultStringValue = "default";

const int CorrectIntegerValue = 10;
const int IncorrectIntegerValue = 20;
const int DefaultIntegerValue = 30;

const double CorrectDoubleValue = 1.05;
const double IncorrectDoubleValue = 2.05;
const double DefaultDoubleValue = 3.05;

TEST(Cubos_String_Settings_Setters_and_Getters_Tests, Get_String_When_Setting_Was_Added)
{
    auto settings = Settings();
    settings.setString(Key1, CorrectStringValue);

    const std::string& returnedValue = settings.getString(Key1, "");
    EXPECT_EQ(returnedValue, CorrectStringValue);
}

TEST(Cubos_String_Settings_Setters_and_Getters_Tests, Get_Default_String_When_Setting_Was_Not_Added)
{
    auto settings = Settings();

    const std::string& returnedValue = settings.getString(Key1, DefaultStringValue);

    EXPECT_EQ(returnedValue, DefaultStringValue);
}

TEST(Cubos_String_Settings_Setters_and_Getters_Tests, Get_Last_String_After_Multiple_Sets_With_Same_Key)
{
    auto settings = Settings();
    settings.setString(Key1, IncorrectStringValue);
    settings.setString(Key1, CorrectStringValue);

    const std::string& returnedValue = settings.getString(Key1, DefaultStringValue);

    EXPECT_EQ(returnedValue, CorrectStringValue);
}

TEST(Cubos_String_Settings_Setters_and_Getters_Tests, Get_Default_String_With_Wrong_Key)
{
    auto settings = Settings();
    settings.setString(Key1, IncorrectStringValue);

    const std::string& returnedValue = settings.getString(Key2, DefaultStringValue);

    EXPECT_EQ(returnedValue, DefaultStringValue);
}

TEST(Cubos_Integer_Settings_Setters_and_Getters_Tests, Get_String_When_Setting_Was_Added)
{
    auto settings = Settings();
    settings.setInteger(Key1, CorrectIntegerValue);

    const int returnedValue = settings.getInteger(Key1, DefaultIntegerValue);

    EXPECT_EQ(returnedValue, CorrectIntegerValue);
}

TEST(Cubos_Integer_Settings_Setters_and_Getters_Tests, Get_Default_String_When_Setting_Was_Not_Added)
{
    auto settings = Settings();

    const int returnedValue = settings.getInteger(Key1, DefaultIntegerValue);

    EXPECT_EQ(returnedValue, DefaultIntegerValue);
}

TEST(Cubos_Integer_Settings_Setters_and_Getters_Tests, Get_Last_String_After_Multiple_Sets_With_Same_Key)
{
    auto settings = Settings();
    settings.setInteger(Key1, IncorrectIntegerValue);
    settings.setInteger(Key1, CorrectIntegerValue);

    const int returnedValue = settings.getInteger(Key1, DefaultIntegerValue);

    EXPECT_EQ(returnedValue, CorrectIntegerValue);
}

TEST(Cubos_Integer_Settings_Setters_and_Getters_Tests, Get_Default_String_With_Wrong_Key)
{
    auto settings = Settings();
    settings.setInteger(Key1, IncorrectIntegerValue);

    const int returnedValue = settings.getInteger(Key2, DefaultIntegerValue);

    EXPECT_EQ(returnedValue, DefaultIntegerValue);
}

TEST(Cubos_Double_Settings_Setters_and_Getters_Tests, Get_String_When_Setting_Was_Added)
{
    auto settings = Settings();
    settings.setDouble(Key1, CorrectDoubleValue);

    const double returnedValue = settings.getDouble(Key1, DefaultDoubleValue);

    EXPECT_EQ(returnedValue, CorrectDoubleValue);
}

TEST(Cubos_Double_Settings_Setters_and_Getters_Tests, Get_Default_String_When_Setting_Was_Not_Added)
{
    auto settings = Settings();

    const double returnedValue = settings.getDouble(Key1, DefaultDoubleValue);

    EXPECT_EQ(returnedValue, DefaultDoubleValue);
}

TEST(Cubos_Double_Settings_Setters_and_Getters_Tests, Get_Last_String_After_Multiple_Sets_With_Same_Key)
{
    auto settings = Settings();
    settings.setDouble(Key1, IncorrectDoubleValue);
    settings.setDouble(Key1, CorrectDoubleValue);

    const double returnedValue = settings.getDouble(Key1, DefaultDoubleValue);

    EXPECT_EQ(returnedValue, CorrectDoubleValue);
}

TEST(Cubos_Double_Settings_Setters_and_Getters_Tests, Get_Default_String_With_Wrong_Key)
{
    auto settings = Settings();
    settings.setDouble(Key1, IncorrectDoubleValue);

    const double returnedValue = settings.getDouble(Key2, DefaultDoubleValue);

    EXPECT_EQ(returnedValue, DefaultDoubleValue);
}

TEST(Cubos_Settings_Merge_Tests, Get_Original_Settings_String_After_Merge_With_Empty_Settings)
{
    auto originalSettings = Settings();
    auto toJoinSettings = Settings();

    originalSettings.setString(Key1, CorrectStringValue);
    originalSettings.merge(toJoinSettings);

    const std::string& returnedValue = originalSettings.getString(Key1, DefaultStringValue);

    EXPECT_EQ(returnedValue, CorrectStringValue);
}

TEST(Cubos_Settings_Merge_Tests, Get_New_Settings_String_After_Merge_With_Populated_Settings)
{
    auto originalSettings = Settings();
    auto toJoinSettings = Settings();

    originalSettings.setString(Key1, IncorrectStringValue);
    toJoinSettings.setString(Key1, CorrectStringValue);

    originalSettings.merge(toJoinSettings);

    const std::string& returnedValue = originalSettings.getString(Key1, DefaultStringValue);

    EXPECT_EQ(returnedValue, CorrectStringValue);
}

TEST(Cubos_Settings_Merge_Tests, Get_Updated_Settings_String_After_Merge_With_Populated_Settings)
{
    auto originalSettings = Settings();
    auto toJoinSettings = Settings();

    originalSettings.setString(Key1, IncorrectStringValue);
    toJoinSettings.setString(Key1, IncorrectStringValue);

    originalSettings.merge(toJoinSettings);

    originalSettings.setString(Key1, CorrectStringValue);

    const std::string& returnedValue = originalSettings.getString(Key1, DefaultStringValue);

    EXPECT_EQ(returnedValue, CorrectStringValue);
}

TEST(Cubos_Settings_Merge_Tests, Get_Merged_Settings_String_After_Changed_Merged_Settings)
{
    auto originalSettings = Settings();
    auto toJoinSettings = Settings();

    originalSettings.setString(Key1, IncorrectStringValue);
    toJoinSettings.setString(Key1, CorrectStringValue);

    originalSettings.merge(toJoinSettings);

    toJoinSettings.setString(Key1, IncorrectStringValue);

    const std::string& returnedValue = originalSettings.getString(Key1, DefaultStringValue);

    EXPECT_EQ(returnedValue, CorrectStringValue);
}
