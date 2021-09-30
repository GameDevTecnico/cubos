#include <gtest/gtest.h>
#include <cubos/settings.hpp>

TEST(CUBOS_SETTINGS_SINGLETON_TESTS, GET_SAME_INSTACE)
{
    cubos::Settings * settings1 = cubos::Settings::GetInstance();
    cubos::Settings * settings2 = cubos::Settings::GetInstance();

    EXPECT_EQ(settings1, settings2);
}

const std::string key1 = "key 1";
const std::string key2 = "key 2";
const std::string correct_value = "right";
const std::string incorrect_value = "wrong";
const std::string default_value = "default";

TEST(CUBOS_SETTINGS_SETTERS_AND_GETTERS_TESTS, GET_STRING_WHEN_SETTING_WAS_ADDED)
{
    cubos::Settings * settings = cubos::Settings::GetInstance();
    settings->set(key1, correct_value);

    const std::string& returnedValue = settings->get(key1, "");

    EXPECT_EQ(returnedValue, correct_value);
}

TEST(CUBOS_SETTINGS_SETTERS_AND_GETTERS_TESTS, GET_DEFAULT_STRING_WHEN_SETTING_WAS_NOT_ADDED)
{
    cubos::Settings * settings = cubos::Settings::GetInstance();

    const std::string& returned_value = settings->get(key1, default_value);

    EXPECT_EQ(returned_value,default_value);
}

TEST(CUBOS_SETTINGS_SETTERS_AND_GETTERS_TESTS, GET_LAST_STRING_AFTER_MULTIPLE_SETS_WITH_SAME_KEY)
{
    cubos::Settings * settings = cubos::Settings::GetInstance();
    settings->set(key1, incorrect_value);
    settings->set(key1, correct_value);

    const std::string& returned_value = settings->get(key1, default_value);

    EXPECT_EQ(returned_value,correct_value);
}

TEST(CUBOS_SETTINGS_SETTERS_AND_GETTERS_TESTS, GET_DEFAULT_STRING_WITH_WRONG_KEY)
{
    cubos::Settings * settings = cubos::Settings::GetInstance();
    settings->set(key1, incorrect_value);

    const std::string& returned_value = settings->get(key2, default_value);

    EXPECT_EQ(returned_value, default_value);
}