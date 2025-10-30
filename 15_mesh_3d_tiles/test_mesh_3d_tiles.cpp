//
//
//

#include <gtest/gtest.h>

#include <filesystem>
namespace fs = std::filesystem;

#include "../common/DebuggingConsole.h"

class mesh_3d_tiles : public testing::Test {
protected:
#pragma region generic gtest rig
    const char *test_case_name() const {
        // https://google.github.io/googletest/advanced.html#getting-the-current-tests-name
        // Gets information about the currently running test.
        // Do NOT delete the returned object - it's managed by the UnitTest
        // class.
        const testing::TestInfo *const test_info = testing::UnitTest::GetInstance()->current_test_info();

        return test_info->test_case_name();
    }

    const char *test_name() const {
        // https://google.github.io/googletest/advanced.html#getting-the-current-tests-name
        // Gets information about the currently running test.
        // Do NOT delete the returned object - it's managed by the UnitTest
        // class.
        const testing::TestInfo *const test_info = testing::UnitTest::GetInstance()->current_test_info();

        return test_info->name();
    }

    fs::path create_workspace() {
        auto ws = fs::path("out") / test_case_name() / test_name();
        if (fs::is_directory(ws))
            fs::remove_all(ws);
        std::error_code err;
        fs::create_directories(ws, err);
        CONSOLE("ws = " << fs::absolute(ws).string());
        return ws;
    }
#pragma endregion

protected:
    // SetUp() ...
};

TEST_F(mesh_3d_tiles, t0) {
    CONSOLE("Hello there");
    auto ws = create_workspace();
    ASSERT_TRUE(fs::is_directory(ws)) << "ws == " << ws;
}
