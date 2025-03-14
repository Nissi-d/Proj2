#include <gtest/gtest.h>
#include "StringUtils.h"



TEST(StringUtilsTest, Capitalize){
    EXPECT_EQ(StringUtils::Capitalize("hello"), "Hello");
    EXPECT_EQ(StringUtils::Capitalize("WORLD"), "World");
    EXPECT_EQ(StringUtils::Capitalize("3"), "3");
}

TEST(StringUtilsTest, Upper){
    EXPECT_EQ(StringUtils::Upper("hello"), "HELLO");
    EXPECT_EQ(StringUtils::Upper("HELLO"), "HELLO");
}

TEST(StringUtilsTest, Lower){
    EXPECT_EQ(StringUtils::Lower("hello"), "hello");
    EXPECT_EQ(StringUtils::Lower("HELLO"), "hello");
}

TEST(StringUtilsTest, LStrip){
    EXPECT_EQ(StringUtils::LStrip("   hello"), "hello");
    EXPECT_EQ(StringUtils::LStrip("hello   "), "hello   ");
}

TEST(StringUtilsTest, RStrip){
    EXPECT_EQ(StringUtils::RStrip("hello  "), "hello");
    EXPECT_EQ(StringUtils::RStrip("   hello"), "   hello");
}

TEST(StringUtilsTest, Strip){
    EXPECT_EQ(StringUtils::Strip("  hello  "), "hello");
    EXPECT_EQ(StringUtils::Strip("   hello   "), "hello");
}



TEST(StringUtilsTest, LJust){
    EXPECT_EQ(StringUtils::LJust("hello", 10), "hello     ");
}

TEST(StringUtilsTest, RJust){
    EXPECT_EQ(StringUtils::RJust("hello", 10), "     hello");
}

TEST(StringUtilsTest, Replace){
    EXPECT_EQ(StringUtils::Replace("hello world", "hello", "hi"), "hi world");
}

TEST(StringUtilsTest, Split){
    EXPECT_EQ(StringUtils::Split("hello world", ""), (std::vector<std::string>{"hello", "world"}));
}

TEST(StringUtilsTest, Join){
    EXPECT_EQ(StringUtils::Join(",", {"a", "b", "c"}), "a,b,c");
}

TEST(StringUtilsTest, ExpandTabs){
    EXPECT_EQ(StringUtils::ExpandTabs("hello\tworld", 4), "hello   world");
}

TEST(StringUtilsTest, EditDistance){
    EXPECT_EQ(StringUtils::EditDistance("hello", "hello", false), 0);
}