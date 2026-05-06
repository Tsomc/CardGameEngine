#include <gtest/gtest.h>
#include "module_manager.h"

using namespace engine;

class TestModuleA : public IModule {
public:
    bool initCalled = false;
    bool startCalled = false;
    bool shutdownCalled = false;
    float lastDt = 0.0f;

    bool ModuleInit() override { initCalled = true; return true; }
    void ModuleStart() override { startCalled = true; }
    void ModuleUpdate(float dt) override { lastDt = dt; }
    void ModuleShutdown() override { shutdownCalled = true; }
    const char* ModuleGetName() const override { return "TestModuleA"; }
};

class TestModuleB : public IModule {
public:
    bool initCalled = false;

    bool ModuleInit() override { initCalled = true; return true; }
    void ModuleStart() override {}
    void ModuleUpdate(float) override {}
    void ModuleShutdown() override {}
    const char* ModuleGetName() const override { return "TestModuleB"; }
};

class TestModuleFail : public IModule {
public:
    bool ModuleInit() override { return false; }
    void ModuleStart() override {}
    void ModuleUpdate(float) override {}
    void ModuleShutdown() override {}
    const char* ModuleGetName() const override { return "TestModuleFail"; }
};

TEST(ModuleManagerTest, RegisterAndInit)
{
    ModuleManager mm;
    mm.ModuleRegister<TestModuleA>();
    mm.ModuleRegister<TestModuleB>();

    EXPECT_TRUE(mm.ModuleInitAll());

    TestModuleA* a = mm.ModuleGet<TestModuleA>();
    ASSERT_NE(a, nullptr);
    EXPECT_TRUE(a->initCalled);

    TestModuleB* b = mm.ModuleGet<TestModuleB>();
    ASSERT_NE(b, nullptr);
    EXPECT_TRUE(b->initCalled);
}

TEST(ModuleManagerTest, ModuleStart)
{
    ModuleManager mm;
    int idx = mm.ModuleRegister<TestModuleA>();
    (void)idx;
    EXPECT_TRUE(mm.ModuleInitAll());
    mm.ModuleStartAll();

    TestModuleA* a = mm.ModuleGet<TestModuleA>();
    ASSERT_NE(a, nullptr);
    EXPECT_TRUE(a->startCalled);
}

TEST(ModuleManagerTest, ModuleUpdate)
{
    ModuleManager mm;
    mm.ModuleRegister<TestModuleA>();
    EXPECT_TRUE(mm.ModuleInitAll());

    mm.ModuleUpdateAll(0.016f);

    TestModuleA* a = mm.ModuleGet<TestModuleA>();
    ASSERT_NE(a, nullptr);
    EXPECT_FLOAT_EQ(a->lastDt, 0.016f);
}

TEST(ModuleManagerTest, ModuleShutdown)
{
    ModuleManager mm;
    mm.ModuleRegister<TestModuleA>();
    EXPECT_TRUE(mm.ModuleInitAll());
    mm.ModuleShutdownAll();

    TestModuleA* a = mm.ModuleGet<TestModuleA>();
    ASSERT_NE(a, nullptr);
    EXPECT_TRUE(a->shutdownCalled);
}

TEST(ModuleManagerTest, InitFailure)
{
    ModuleManager mm;
    mm.ModuleRegister<TestModuleA>();
    mm.ModuleRegister<TestModuleFail>();
    mm.ModuleRegister<TestModuleB>();

    EXPECT_FALSE(mm.ModuleInitAll());
}

TEST(ModuleManagerTest, GetNullForUnregistered)
{
    ModuleManager mm;
    TestModuleA* a = mm.ModuleGet<TestModuleA>();
    EXPECT_EQ(a, nullptr);
}
