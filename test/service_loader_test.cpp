#include "service_loader.hpp"

#include <gmock/gmock.h>

using namespace testing;

TEST(ServiceLoader, LoadEngine)
{
    auto engine1 = ersap::ServiceLoader{"evio_generator_service"};
    auto engine2 = ersap::ServiceLoader{"evio_printer_service"};

    EXPECT_THAT(engine1.get(), Ne(nullptr));
    EXPECT_THAT(engine2.get(), Ne(nullptr));

    auto input = ersap::EngineData{};
    input.set_data(ersap::type::STRING.mime_type(), "next");

    auto r1 = engine1->execute(input);
    auto r2 = engine2->execute(r1);
}


int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
