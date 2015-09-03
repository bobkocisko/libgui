//
// Created by sjm-kociskobd on 8/28/15.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

int main(int argc, char ** argv)
{
    ::testing::InitGoogleMock(&argc, argv);
    int ret = RUN_ALL_TESTS();
    return ret;
}

