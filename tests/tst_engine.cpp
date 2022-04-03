
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include "test_engine.h"

#include <QCoreApplication>

using namespace testing;

namespace {
int core_argc;
char** core_argv;
}

int main(int argc, char** argv)
{
    core_argc = argc;
    core_argv = argv;

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(Tionix, Engine)
{
    QCoreApplication core(core_argc, core_argv);

    TestEngine test;

    QObject::connect(&test, &TestEngine::quitTest, &core, &QCoreApplication::quit);
    core.exec();
}

