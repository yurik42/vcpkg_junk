//
//
//

#include <gtest/gtest.h>

#include <filesystem>
namespace fs = std::filesystem;

#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/scene.h>

#include "../common/DebuggingConsole.h"

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

#include "Cesium3d_meshtoolbox.h"


namespace {
#if 0
}
#endif
std::ostream &operator<<(std::ostream &ss, aiVector3D const &v) {
    ss << "{" << v.x << ", " << v.y << ", " << v.z << "}";
    return ss;
}

#ifndef ASSIMP_DOUBLE_PRECISION
std::ostream &operator<<(std::ostream &ss, aiVector3d const &v) {
    ss << "{" << std::setprecision(20) << v.x << ", " << v.y << ", " << v.z << "}";
    return ss;
}
#endif

std::ostream &operator<<(std::ostream &ss, aiAABB const &v) {
    ss << "{" << v.mMin << ", " << v.mMax << "}";
    return ss;
}

std::ostream &operator<<(std::ostream &ss, aiMatrix4x4t<double> const &v) {
    ss << "{" << "\n"
       << std::setprecision(20) << "\t" << v.a1 << ", " << v.a2 << ", " << v.a3 << ", " << v.a4 << "\n"
       << "\t" << v.b1 << ", " << v.b2 << ", " << v.b3 << ", " << v.b4 << "\n"
       << "\t" << v.c1 << ", " << v.c2 << ", " << v.c3 << ", " << v.c4 << "\n"
       << "\t" << v.d1 << ", " << v.d2 << ", " << v.d3 << ", " << v.d4 << "\n"
       << "}";
    return ss;
}
} // namespace

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

    /// @brief Find an appropriate testdata directory
    /// @return return the path string or empty string
    virtual std::string data_directory() const {
        auto this_directory = fs::path(__FILE__).parent_path();
        auto data_directory = this_directory / "data";
        if (fs::is_directory(data_directory))
            return data_directory.string();
        return std::string();
    }
#pragma endregion
protected:
    // SetUp() ...
};

TEST_F(mesh_3d_tiles, t0) {
    CONSOLE("Hello there");
    auto ws = create_workspace();
    ASSERT_TRUE(fs::is_directory(ws)) << "ws == " << ws;
    ASSERT_TRUE(fs::is_directory(data_directory()));
}

TEST_F(mesh_3d_tiles, load_tb_2_actual_glb) {
    // Construct path to the test GLB file
    auto glb_path = fs::path(data_directory()) / "tb_2" / "actual.glb";

    // Verify the file exists
    ASSERT_TRUE(fs::is_regular_file(glb_path)) << "File not found: " << glb_path;

    // Load the GLB file using assimp
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(glb_path.string(), 0);

    // Verify the scene loaded successfully
    ASSERT_NE(nullptr, scene) << "Failed to load GLB file: " << importer.GetErrorString();
    auto aabb = meshtoolbox::compute_aabb_with_transform(scene);

    // Log some basic scene information
    CONSOLE("Scene loaded successfully");
    CONSOLE("  Meshes: " << scene->mNumMeshes);
    CONSOLE("  Materials: " << scene->mNumMaterials);
    CONSOLE("  Textures: " << scene->mNumTextures);
    CONSOLE("  min: " << aabb.first << ", max: " << aabb.second);
}

TEST_F(mesh_3d_tiles, load_tb_3_actual_glb) {
    // Construct path to the test GLB file
    auto glb_path = fs::path(data_directory()) / "tb_3" / "actual.glb";

    // Verify the file exists
    ASSERT_TRUE(fs::is_regular_file(glb_path)) << "File not found: " << glb_path;

    // Load the GLB file using assimp
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(glb_path.string(), 0);

    // Verify the scene loaded successfully
    ASSERT_NE(nullptr, scene) << "Failed to load GLB file: " << importer.GetErrorString();
    auto aabb = meshtoolbox::compute_aabb_with_transform(scene);

    // Log some basic scene information
    CONSOLE("Scene loaded successfully");
    CONSOLE("  Meshes: " << scene->mNumMeshes);
    CONSOLE("  Materials: " << scene->mNumMaterials);
    CONSOLE("  Textures: " << scene->mNumTextures);
    CONSOLE("  min: " << aabb.first << ", max: " << aabb.second);
}

/// @brief Create a georeferenced "graveyard" scene (the test data)
/// @param --gtest_filter=AssetImportFormatExporterF.georeferenced_graveyard_scene
///
TEST_F(mesh_3d_tiles, georeferenced_graveyard_scene) {

    auto ws = create_workspace();

    meshtoolbox::Toolbox tb;

    // A "graveyard"
    ai_real max_X = 2000 * 4;
    ai_real max_Z = 1000 * 4;
    ai_real step = 50; // 50m interval between "obelisks"

    std::vector<meshtoolbox::box_t> boxes;
    boxes.reserve(size_t(max_X / step + 1) * size_t(max_Z / step + 1));
    for (int i = 0; i < max_X / step; ++i) {
        for (int j = 0; j < max_Z / step; ++j) {
            boxes.emplace_back(meshtoolbox::box_t{{i * step, 0, j * step}, {step / 5, step, 4}});
        }
    }

    boxes.emplace_back(meshtoolbox::box_t{{max_X, 0, max_Z}, {step, step * 10, step}});
    boxes.emplace_back(meshtoolbox::box_t{{max_X, 0, 0}, {step, step * 10, step}});
    boxes.emplace_back(meshtoolbox::box_t{{0, 0, max_Z}, {step, step * 10, step}});

    double longitude = -69.9390136341, latitude = 43.8942174868;

    auto actual = std::unique_ptr<aiScene>(tb.make_boxes(boxes));
    ASSERT_TRUE((bool)actual);

#if 0
    // UTM-19 is EPSG:32619
    auto UTM19 = std::unique_ptr<Projection>(EpsgCodeMapper_t::getProjFromEpsgCode(32619));
    ASSERT_TRUE(UTM19);
    aiMatrix4x4 translate = translate_from_geo(*UTM19, longitude, latitude);
    // place the scene at (latitude, longitude)
    actual->mRootNode->mTransformation = translate * actual->mRootNode->mTransformation;
#endif

    Assimp::Exporter exp;
    exp.Export(actual.get(), "glb2", (ws / "actual.glb").string());

    auto aabb = meshtoolbox::compute_aabb_with_transform(actual.get());
    CONSOLE("  min: " << aabb.first << ", max: " << aabb.second);
}
