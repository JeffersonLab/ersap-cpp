#include <ersap/engine_data_type.hpp>

#include <xmsg/proto/data.h>

#include <gmock/gmock.h>

#include <algorithm>
#include <climits>
#include <functional>
#include <random>
#include <string>
#include <vector>

using namespace testing;


TEST(PrimitiveSerializer, IntegerSerialization)
{
    const auto* s = ersap::type::SINT32.serializer();

    const auto b = s->write(ersap::any{18});
    const auto d = ersap::any_cast<std::int32_t>(s->read(b));

    ASSERT_THAT(d, Eq(18));
}


TEST(PrimitiveSerializer, FloatingPointSerialization)
{
    const auto* s = ersap::type::DOUBLE.serializer();

    const auto b = s->write(ersap::any{78.98});
    const auto d = ersap::any_cast<double>(s->read(b));

    ASSERT_THAT(d, Eq(78.98));
}


TEST(PrimitiveSerializer, StringSerialization)
{
    const auto* s = ersap::type::STRING.serializer();

    const auto b = s->write(ersap::any{std::string{"Master of Puppets"}});
    const auto d = ersap::any_cast<std::string>(s->read(b));

    ASSERT_THAT(d, Eq("Master of Puppets"));
}


TEST(ArraySerializer, IntegerSerialization)
{
    const auto* s = ersap::type::ARRAY_SFIXED32.serializer();

    const auto v = std::vector<std::int32_t>{4, 5, 6};
    const auto b = s->write(ersap::any{v});
    const auto d = ersap::any_cast<decltype(v)>(s->read(b));

    ASSERT_THAT(d, ContainerEq(v));
}


TEST(ArraySerializer, FloatingPointSerialization)
{
    const auto* s = ersap::type::ARRAY_DOUBLE.serializer();

    const auto v = std::vector<double>{4.1, 5.6};
    const auto b = s->write(ersap::any{v});
    const auto d = ersap::any_cast<decltype(v)>(s->read(b));

    ASSERT_THAT(d, ContainerEq(v));
}


TEST(ArraySerializer, StringSerialization)
{
    const auto* s = ersap::type::ARRAY_STRING.serializer();

    const auto v = std::vector<std::string>{ "Ride the Lightning",
                                             "Master of Puppets",
                                             "...And Justice for All"};
    const auto b = s->write(ersap::any{v});
    const auto d = ersap::any_cast<decltype(v)>(s->read(b));

    ASSERT_THAT(d, ContainerEq(v));
}


TEST(RawBytesSerializer, RawBytesSerialization)
{
    const auto* s = ersap::type::BYTES.serializer();

    using rnd_eng = std::default_random_engine;
    using rnd_byte_eng = std::independent_bits_engine<rnd_eng, CHAR_BIT, std::uint8_t>;
    auto rbe = rnd_byte_eng{};

    auto r = std::vector<std::uint8_t>(100);
    std::generate(r.begin(), r.end(), std::ref(rbe));

    const auto b = s->write(ersap::any{r});
    const auto d = ersap::any_cast<decltype(r)>(s->read(b));

    ASSERT_THAT(d, ContainerEq(r));
}


TEST(RawBytesSerializer, MoveSemantics)
{
    const auto* s = ersap::type::BYTES.serializer();

    using rnd_eng = std::default_random_engine;
    using rnd_byte_eng = std::independent_bits_engine<rnd_eng, CHAR_BIT, std::uint8_t>;
    rnd_byte_eng rbe;

    auto r = std::vector<std::uint8_t>(100);
    std::generate(r.begin(), r.end(), std::ref(rbe));

    auto a = ersap::any{r};

    auto* m = ersap::any_cast<decltype(r)>(&a);
    ASSERT_THAT(*m, ContainerEq(r));

    auto b = s->write(std::move(a));
    auto d = ersap::any_cast<decltype(r)>(s->read(std::move(b)));

    ASSERT_THAT(*m, IsEmpty());
    ASSERT_THAT(b, IsEmpty());
    ASSERT_THAT(d, ContainerEq(r));
}


TEST(NativeSerializer, NativeSerialization)
{
    const auto* s = ersap::type::NATIVE.serializer();

    auto xd = xmsg::proto::Data{};
    xd.set_flsint32(56);
    xd.set_double_(5.6);
    xd.add_stringa("Ride the Lightning");
    xd.add_stringa("Master of Puppets");
    xd.add_stringa("...And Justice for All");

    const auto b = s->write(ersap::any{xd});
    const auto d = ersap::any_cast<decltype(xd)>(s->read(b));

    ASSERT_THAT(d, Eq(xd));
}


TEST(JSONSerializer, JSONSerialization)
{
    const auto* s = ersap::type::JSON.serializer();

    auto j = std::string{"{ \"a\": 1, \"b\": 2, \"c\": [ 3, 4, 5] }"};

    const auto b = s->write(ersap::any{j});
    const auto d = ersap::any_cast<decltype(j)>(s->read(b));

    ASSERT_THAT(d, Eq(j));
}


TEST(EngineDataType, CompareWithStringMimeType)
{
    ASSERT_THAT(ersap::type::JSON, Eq("application/json"));
    ASSERT_THAT(ersap::type::JSON, Ne("application/xml"));

    ASSERT_THAT("binary/bytes", Eq(ersap::type::BYTES));
    ASSERT_THAT("binary/bytes", Ne(ersap::type::STRING));
}


int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
