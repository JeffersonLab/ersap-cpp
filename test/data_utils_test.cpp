#include "data_utils.hpp"

#include <gmock/gmock.h>

using namespace ersap::util;
using namespace std::string_literals;
using namespace testing;

const auto topic = xmsg::Topic::build("topic");

xmsg::Message request(const std::string& data)
{
    auto bytes = std::vector<std::uint8_t>{data.begin(), data.end()};
    return xmsg::Message{topic, "text/string", std::move(bytes)};
}


TEST(RequestParser, ParseContainerRequest)
{
    auto r = "startContainer?master?5?undefined"s;
    auto m = request(r);
    auto p = RequestParser::build(m);

    EXPECT_THAT(p.next_string(),  Eq("startContainer"));
    EXPECT_THAT(p.next_string(),  Eq("master"));
    EXPECT_THAT(p.next_integer(), Eq(5));
    EXPECT_THAT(p.next_string(),  Eq("undefined"));

    EXPECT_THAT(p.request(), Eq(r));
}


TEST(RequestParser, ParseServiceRequest)
{
    auto r = "startService?master?E1?org.jlab.ersap.examples.engines.E1?3?undefined?undefined"s;
    auto m = request(r);
    auto p = RequestParser::build(m);

    EXPECT_THAT(p.next_string(),  Eq("startService"));
    EXPECT_THAT(p.next_string(),  Eq("master"));
    EXPECT_THAT(p.next_string(),  Eq("E1"));
    EXPECT_THAT(p.next_string(),  Eq("org.jlab.ersap.examples.engines.E1"));
    EXPECT_THAT(p.next_integer(), Eq(3));
    EXPECT_THAT(p.next_string(),  Eq("undefined"));
    EXPECT_THAT(p.next_string(),  Eq("undefined"));

    EXPECT_THAT(p.request(), Eq(r));
}


TEST(RequestParser, InvalidMimeType)
{
    auto m = xmsg::make_message(topic, 34.8);

    ASSERT_THROW(RequestParser::build(m), InvalidRequest);
}


TEST(RequestParser, EmptyRequest)
{
    auto m = request("");
    auto p = RequestParser::build(m);

    ASSERT_THROW(p.next_string(), InvalidRequest);
}


TEST(RequestParser, SingleStringElement)
{
    auto m = request("pingDpe");
    auto p = RequestParser::build(m);

    EXPECT_THAT(p.next_string(), Eq("pingDpe"));
}


TEST(RequestParser, MissingStringElement)
{
    auto m = request("stopService?master?E1");
    auto p = RequestParser::build(m);

    p.next_string();
    p.next_string();
    p.next_string();

    ASSERT_THROW(p.next_string(), InvalidRequest);
}


TEST(RequestParser, EmptyStringElement)
{
    auto m = request("stopService??E1");
    auto p = RequestParser::build(m);

    p.next_string();

    ASSERT_THROW(p.next_string(), InvalidRequest);
}


TEST(RequestParser, DefaultMissingStringElement)
{
    auto m = request("stopService?master");
    auto p = RequestParser::build(m);

    p.next_string();
    p.next_string();

    EXPECT_THAT(p.next_string("E1"), Eq("E1"));
}


TEST(RequestParser, DefaultEmptyStringElement)
{
    auto m = request("stopService??E1");
    auto p = RequestParser::build(m);

    p.next_string();

    EXPECT_THAT(p.next_string("master"), Eq("master"));
    EXPECT_THAT(p.next_string(), Eq("E1"));
}


TEST(RequestParser, MissingIntegerElement)
{
    auto m = request("stopService?master?E1");
    auto p = RequestParser::build(m);

    p.next_string();
    p.next_string();
    p.next_string();

    ASSERT_THROW(p.next_integer(), InvalidRequest);
}


TEST(RequestParser, EmptyIntegerElement)
{
    auto m = request("stopService??E1");
    auto p = RequestParser::build(m);

    p.next_string();

    ASSERT_THROW(p.next_integer(), InvalidRequest);
}


TEST(RequestParser, NotAnInteger)
{
    auto m = request("stopService?master?E1");
    auto p = RequestParser::build(m);

    ASSERT_THROW(p.next_integer(), InvalidRequest);
}


TEST(RequestParser, OnlySeparators)
{
    auto m = request("???");
    auto p = RequestParser::build(m);

    ASSERT_THROW(p.next_string(), InvalidRequest);
}


int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
