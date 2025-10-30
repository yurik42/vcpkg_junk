//
//
//

#include <gtest/gtest.h>

#include <filesystem>
namespace fs = std::filesystem;

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "../common/DebuggingConsole.h"

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

namespace assimp_toolbox {

/// @brief Compute the bounding volume of a scene with respect to node
/// transforms
/// @param scene
/// @return std::pair<aiVector3d, aiVector3d> representing the min and max corners of the axis-aligned bounding box in
/// world space
std::pair<aiVector3d, aiVector3d> compute_aabb_with_transform(const aiScene *scene) {
    aiVector3d sceneMin(std::numeric_limits<double>::max(), std::numeric_limits<double>::max(),
                        std::numeric_limits<double>::max());
    aiVector3d sceneMax(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::lowest(),
                        std::numeric_limits<double>::lowest());

    std::function<void(const aiNode *, const aiMatrix4x4 &)> traverse;
    traverse = [&](const aiNode *node, const aiMatrix4x4 &parentTransform) {
        aiMatrix4x4 transform = parentTransform * node->mTransformation;
        for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
            const aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            for (unsigned int v = 0; v < mesh->mNumVertices; ++v) {
                aiVector3D vertex = mesh->mVertices[v];
                vertex *= transform;
                sceneMin.x = std::min<double>(sceneMin.x, vertex.x);
                sceneMin.y = std::min<double>(sceneMin.y, vertex.y);
                sceneMin.z = std::min<double>(sceneMin.z, vertex.z);
                sceneMax.x = std::max<double>(sceneMax.x, vertex.x);
                sceneMax.y = std::max<double>(sceneMax.y, vertex.y);
                sceneMax.z = std::max<double>(sceneMax.z, vertex.z);
            }
        }
        for (unsigned int i = 0; i < node->mNumChildren; ++i) {
            traverse(node->mChildren[i], transform);
        }
    };

    if (scene && scene->mRootNode) {
        traverse(scene->mRootNode, aiMatrix4x4());
    }
    return {sceneMin, sceneMax};
}

} // namespace assimp_toolbox

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
    auto aabb = assimp_toolbox::compute_aabb_with_transform(scene);

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
    auto aabb = assimp_toolbox::compute_aabb_with_transform(scene);

    // Log some basic scene information
    CONSOLE("Scene loaded successfully");
    CONSOLE("  Meshes: " << scene->mNumMeshes);
    CONSOLE("  Materials: " << scene->mNumMaterials);
    CONSOLE("  Textures: " << scene->mNumTextures);
    CONSOLE("  min: " << aabb.first << ", max: " << aabb.second);
}
