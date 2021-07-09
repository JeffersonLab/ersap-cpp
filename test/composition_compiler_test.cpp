#include "composition.hpp"

#include <gmock/gmock.h>

using namespace ersap::composition;
using namespace testing;

using output_set = std::set<std::string>;

const std::string composition = R"(10.10.10.1_java:C:S1+)"
                                R"(10.10.10.1_java:C:S2+)"
                                R"(10.10.10.1_java:C:S3+)"
                                R"(10.10.10.1_java:C:S4;)";

TEST(SimpleCompiler, ServiceAtTheBeginning)
{
    auto cc = SimpleCompiler{"10.10.10.1_java:C:S1"};

    cc.compile(composition);

    auto expected = output_set{"10.10.10.1_java:C:S2"};
    ASSERT_THAT(cc.outputs(), ContainerEq(expected));
}


TEST(SimpleCompiler, ServiceAtTheMiddle)
{
    auto cc = SimpleCompiler{"10.10.10.1_java:C:S2"};

    cc.compile(composition);

    auto expected = output_set{"10.10.10.1_java:C:S3"};
    ASSERT_THAT(cc.outputs(), ContainerEq(expected));
}


TEST(SimpleCompiler, ServiceAtTheEnd)
{
    auto cc = SimpleCompiler{"10.10.10.1_java:C:S4"};

    cc.compile(composition);

    ASSERT_THAT(cc.outputs(), IsEmpty());
}


TEST(SimpleCompiler, MultipleCalls)
{
    auto cc = SimpleCompiler{"10.10.10.1_java:C:S3"};

    cc.compile(composition);

    std::string composition2 = R"(10.10.10.1_java:C:S1+)"
                               R"(10.10.10.1_java:C:S3+)"
                               R"(10.10.10.1_java:C:S5;)";
    cc.compile(composition2);

    auto expected = output_set{"10.10.10.1_java:C:S5"};
    ASSERT_THAT(cc.outputs(), ContainerEq(expected));
}


int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
