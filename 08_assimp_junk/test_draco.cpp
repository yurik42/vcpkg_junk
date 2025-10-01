//
//
//

#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

#include "test_draco.h"

#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdint.h>
#include <vector>

// Include Draco decoder headers
#include "draco/compression/decode.h"
#include "draco/point_cloud/point_cloud.h"

#include <json-c/json.h>
#include <json-c/printbuf.h>

namespace cesium_pnts {
#if 0
    }
#endif

// Cesium PNTS file format structures
#pragma pack(push, 1)
struct PntsHeader {
    char magic[4];       // "pnts"
    uint32_t version;    // Version number (usually 1)
    uint32_t byteLength; // Total byte length of the file
    uint32_t featureTableJSONByteLength;
    uint32_t featureTableBinaryByteLength;
    uint32_t batchTableJSONByteLength;
    uint32_t batchTableBinaryByteLength;
};
#pragma pack(pop)

struct PointCloudData {
    std::vector<float> positions;   // x, y, z coordinates
    std::vector<uint8_t> colors;    // RGB or RGBA colors
    std::vector<float> normals;     // Optional normals
    std::vector<uint32_t> batchIds; // Optional batch IDs
    uint32_t pointCount;

    PointCloudData() : pointCount(0) {}
};

class CesiumPntsDecoder {
public:
    PntsHeader header;
    std::string last_error;

public:
    // Decode a PNTS file from memory buffer
    bool decodePnts(const std::vector<uint8_t> &fileData,
                           PointCloudData &output) {
        if (fileData.size() < sizeof(PntsHeader)) {
            last_error = "File too small to contain PNTS header";
            return false;
        }

        std::memcpy(&header, fileData.data(), sizeof(PntsHeader));

        // Validate magic number
        if (std::strncmp(header.magic, "pnts", 4) != 0) {
            last_error = "Invalid PNTS magic number";
            return false;
        }
#if _DEBUG
        std::cout << "PNTS Header:\n";
        std::cout << "  Version: " << header.version << "\n";
        std::cout << "  Byte Length: " << header.byteLength << "\n";
        std::cout << "  Feature Table JSON Length: "
                  << header.featureTableJSONByteLength << "\n";
        std::cout << "  Feature Table Binary Length: "
                  << header.featureTableBinaryByteLength << "\n";
#endif
        // Calculate offsets
        size_t offset = sizeof(PntsHeader);
        size_t jsonStart = offset;
        size_t jsonEnd = jsonStart + header.featureTableJSONByteLength;
        size_t binaryStart = jsonEnd;
        size_t binaryEnd = binaryStart + header.featureTableBinaryByteLength;

        // Extract feature table JSON (optional - parse to get metadata)
        std::string featureTableJSON;
        if (header.featureTableJSONByteLength > 0) {
            featureTableJSON = std::string(
                reinterpret_cast<const char *>(fileData.data() + jsonStart),
                header.featureTableJSONByteLength);
            std::cout << "\nFeature Table JSON:\n"
                      << featureTableJSON << "\n\n";
        }

        // Check if data is Draco compressed
        // Look for Draco magic bytes in binary section
        if (binaryEnd > fileData.size()) {
            last_error = "Invalid binary data range";
            return false;
        }

        const uint8_t *binaryData = fileData.data() + binaryStart;
        size_t binarySize = header.featureTableBinaryByteLength;

        // Check for Draco magic number "DRACO" at the start of binary data
        bool isDraco = (binarySize > 5 && binaryData[0] == 'D' &&
                        binaryData[1] == 'R' && binaryData[2] == 'A' &&
                        binaryData[3] == 'C' && binaryData[4] == 'O');

        if (isDraco) {
            std::cout << "Detected Draco compression\n";
            return decodeDracoPointCloud(binaryData, binarySize, output);
        }

        /* non-draco */
        {
            if (!decodeRawPointCloud(featureTableJSON, binaryData, binarySize,
                                     output))
                return false;
            /* parse batch part */
            if (size_t batchSize = header.batchTableBinaryByteLength)
            {
                const uint8_t *batchData =
                    fileData.data() + binaryStart + binarySize;
                std::string batchJSON((const char *)batchData,
                                      header.batchTableJSONByteLength);
                std::cout << "batchJSON: " << batchJSON << "\n";
            }
        }
        return true;
    }

    // Decode Draco compressed point cloud
    bool decodeDracoPointCloud(const uint8_t *data, size_t size,
                                      PointCloudData &output) {
        // Create Draco decoder buffer
        draco::DecoderBuffer buffer;
        buffer.Init(reinterpret_cast<const char *>(data), size);

        // Decode point cloud
        draco::Decoder decoder;
        auto statusor = decoder.DecodePointCloudFromBuffer(&buffer);

        if (!statusor.ok()) {
            std::cerr << "Failed to decode Draco point cloud: "
                      << statusor.status().error_msg_string() << "\n";
            return false;
        }

        std::unique_ptr<draco::PointCloud> pointCloud =
            std::move(statusor).value();
        output.pointCount = pointCloud->num_points();

        std::cout << "Successfully decoded " << output.pointCount
                  << " points\n";

        // Extract position attribute
        const draco::PointAttribute *posAttr =
            pointCloud->GetNamedAttribute(draco::GeometryAttribute::POSITION);

        if (posAttr) {
            std::cout << "Found POSITION attribute\n";
            output.positions.reserve(output.pointCount * 3);

            for (uint32_t i = 0; i < output.pointCount; ++i) {
                float pos[3];
                posAttr->GetMappedValue(draco::PointIndex(i), pos);
                output.positions.push_back(pos[0]);
                output.positions.push_back(pos[1]);
                output.positions.push_back(pos[2]);
            }
        }

        // Extract color attribute
        const draco::PointAttribute *colorAttr =
            pointCloud->GetNamedAttribute(draco::GeometryAttribute::COLOR);

        if (colorAttr) {
            std::cout << "Found COLOR attribute\n";
            int numComponents = colorAttr->num_components();
            output.colors.reserve(output.pointCount * numComponents);

            for (uint32_t i = 0; i < output.pointCount; ++i) {
                if (numComponents == 3) {
                    uint8_t rgb[3];
                    colorAttr->GetMappedValue(draco::PointIndex(i), rgb);
                    output.colors.push_back(rgb[0]);
                    output.colors.push_back(rgb[1]);
                    output.colors.push_back(rgb[2]);
                } else if (numComponents == 4) {
                    uint8_t rgba[4];
                    colorAttr->GetMappedValue(draco::PointIndex(i), rgba);
                    output.colors.push_back(rgba[0]);
                    output.colors.push_back(rgba[1]);
                    output.colors.push_back(rgba[2]);
                    output.colors.push_back(rgba[3]);
                }
            }
        }

        // Extract normal attribute (if present)
        const draco::PointAttribute *normalAttr =
            pointCloud->GetNamedAttribute(draco::GeometryAttribute::NORMAL);

        if (normalAttr) {
            std::cout << "Found NORMAL attribute\n";
            output.normals.reserve(output.pointCount * 3);

            for (uint32_t i = 0; i < output.pointCount; ++i) {
                float normal[3];
                normalAttr->GetMappedValue(draco::PointIndex(i), normal);
                output.normals.push_back(normal[0]);
                output.normals.push_back(normal[1]);
                output.normals.push_back(normal[2]);
            }
        }

        // Extract generic attributes (like BATCH_ID)
        for (int32_t i = 0; i < pointCloud->num_attributes(); ++i) {
            const draco::PointAttribute *attr = pointCloud->attribute(i);

            // Check for custom attributes by metadata or unique_id
            if (attr->attribute_type() == draco::GeometryAttribute::GENERIC) {
                std::cout << "Found GENERIC attribute at index " << i << "\n";

                // Try to extract as batch IDs (assuming uint32_t)
                if (attr->num_components() == 1) {
                    output.batchIds.reserve(output.pointCount);
                    for (uint32_t j = 0; j < output.pointCount; ++j) {
                        uint32_t batchId;
                        attr->GetMappedValue(draco::PointIndex(j), &batchId);
                        output.batchIds.push_back(batchId);
                    }
                }
            }
        }

        return true;
    }

    bool false_because(std::string s) {
        last_error = s;
        return false;
    }

    struct feature_table_props_t {
        std::optional<unsigned> POINTS_LENGTH,
            POSITION_byteOffset,
            RGBA_byteOffset, 
            POSITION_QUANTIZED_byteOffset,
            RGB_byteOffset;
        std::vector<float> RTC_CENTER;
        std::map<std::string, std::string> extensions;
    } ;

    bool parse_feature_table_json(const char* json_text, feature_table_props_t& props)
    {
        auto root = json_tokener_parse(json_text);
        if (!root)
            return false_because("Cannot parse featureTableJSON");

        // destroy `root` object on exit of the scope
        std::unique_ptr<json_object, std::function<void(json_object *)>>
            on_return(root, [](json_object *r) { json_object_put(r); });

        {
            auto o = json_object_object_get(root, "POINTS_LENGTH");
            if (!o)
                return false_because("POINTS_LENGTH is not defined");
            props.POINTS_LENGTH = json_object_get_int(o);
        }

        if (auto eo = json_object_object_get(root, "extensions")) {
            if (auto e_draco = json_object_object_get(
                    eo, "3DTILES_draco_point_compression")) {
                // TODO: parse the draco extensions parameters
                props.extensions.insert(
                    {"3DTILES_draco_point_compression", "1"});
            }
        }

        if (auto po = json_object_object_get(root, "POSITION")) {
            unsigned byteOffset = 0;
            if (auto bo = json_object_object_get(root, "byteOffset"))
                byteOffset = json_object_get_int(bo);
            props.POSITION_byteOffset = byteOffset;
        }
        if (auto o = json_object_object_get(root, "RTC_CENTER")) {
            if (json_type_array != json_object_get_type(o))
                return false_because("RTC_CENTER must be array");
            array_list *arr = json_object_get_array(o);
            if (arr->length != 3)
                return false_because("RTC_CENTER must be array of 3");
            props.RTC_CENTER.resize(3);
            for (int i = 0; i < arr->length; ++i)
                props.RTC_CENTER[i] =
                    static_cast<float>(json_object_get_double(
                        (json_object *)arr->array[i]));
        }
        if (auto po = json_object_object_get(root, "RGB")) {
            unsigned byteOffset = 0;
            if (auto bo = json_object_object_get(root, "byteOffset"))
                byteOffset = json_object_get_int(bo);
            props.RGB_byteOffset = byteOffset;
        }
        if (auto po = json_object_object_get(root, "RGBA")) {
            unsigned byteOffset = 0;
            if (auto bo = json_object_object_get(root, "byteOffset"))
                byteOffset = json_object_get_int(bo);
            props.RGBA_byteOffset = byteOffset;
        }

        if (!props.POSITION_byteOffset.has_value() &&
            !props.POSITION_QUANTIZED_byteOffset.has_value())
            return false_because(
                "Nether POSITION nor POSITION_QUANTIZED defined");

        return true;
    }

    // Decode raw (uncompressed) point cloud data
    bool decodeRawPointCloud(std::string const &featureTableJSON,
                             const uint8_t *data, size_t size,
                             PointCloudData &output) {

        feature_table_props_t features;

        if (!parse_feature_table_json(featureTableJSON.c_str(), features))
            return false;

        output.pointCount = features.POINTS_LENGTH.value();
        if (features.POSITION_byteOffset.has_value()) {
            auto pos = reinterpret_cast<float const *>(
                data + features.POSITION_byteOffset.value());
            output.positions.assign(pos, pos + 3 * output.pointCount);
        } else {
            // TODO: implement POSITION_QUANTIZED support
            return false_because("only POSITION currently supported");
        }

        if (features.RGB_byteOffset.has_value()) {
            auto pos = reinterpret_cast<uint8_t const *>(
                data + features.RGB_byteOffset.value());
            output.colors.assign(pos, pos + 3 * output.pointCount);
        } else if (features.RGBA_byteOffset.has_value()) {
            auto pos = reinterpret_cast<uint8_t const *>(
                data + features.RGBA_byteOffset.value());
            output.colors.assign(pos, pos + 4 * output.pointCount);
        }

        return true;
    }

    // Load PNTS file from disk
    bool loadPntsFile(const std::string &filename,
                             PointCloudData &output) {
        std::ifstream file(filename, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filename << "\n";
            return false;
        }

        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<uint8_t> buffer(size);
        if (!file.read(reinterpret_cast<char *>(buffer.data()), size)) {
            std::cerr << "Failed to read file\n";
            return false;
        }

        return decodePnts(buffer, output);
    }

    // Helper function to print point cloud statistics
    void printStatistics(const PointCloudData &data) {
        std::cout << "\n=== Point Cloud Statistics ===\n";
        std::cout << "Point Count: " << data.pointCount << "\n";
        std::cout << "Has Positions: "
                  << (!data.positions.empty() ? "Yes" : "No") << "\n";
        std::cout << "Has Colors: " << (!data.colors.empty() ? "Yes" : "No")
                  << "\n";
        std::cout << "Has Normals: " << (!data.normals.empty() ? "Yes" : "No")
                  << "\n";
        std::cout << "Has Batch IDs: "
                  << (!data.batchIds.empty() ? "Yes" : "No") << "\n";

        // Print sample data
        if (!data.positions.empty() && data.pointCount > 0) {
            std::cout << "\nFirst point position: (" << data.positions[0]
                      << ", " << data.positions[1] << ", " << data.positions[2]
                      << ")\n";
        }

        if (!data.colors.empty() && data.pointCount > 0) {
            std::cout << "First point color: (" << (int)data.colors[0] << ", "
                      << (int)data.colors[1] << ", " << (int)data.colors[2]
                      << ")\n";
        }
    }
};

} // namespace cesium_pnts

class DracoF : public testing::Test {
protected:
    std::string test_name() const {
        return ::testing::UnitTest::GetInstance()->current_test_info()->name();
    }

    std::string test_case_name() const {
        return ::testing::UnitTest::GetInstance()
            ->current_test_info()
            ->test_case_name();
    }

    fs::path create_ws() {
        auto ws = fs::absolute("out") / test_case_name() / test_name();
        fs::remove_all(ws);
        fs::create_directories(ws);
        return ws;
    }

    void save_as(std::string const &text, fs::path const &location) {
        std::ofstream os(location);
        if (!os.good())
            throw std::runtime_error("Cannot create file: " +
                                     location.string());
        os << text;
        os.close();
    }

    auto test_data(const char *relative_path) {
        auto test_data_dir = fs::absolute(__FILE__).parent_path() / "test_data";
        return test_data_dir / relative_path;
    }

    bool has_failure() const { return ::testing::Test::HasFailure(); }

protected:
    void SetUp() override {}
};

/// @brief
/// @param --gtest_filter=DracoF.t0
/// @param
TEST_F(DracoF, t0) {
    auto zero_pnts = test_data("cesium/pnts/0-draco.pnts");
    ASSERT_TRUE(fs::is_regular_file(zero_pnts))
        << "File: " << zero_pnts.string();
}

/// @brief 
/// @param --gtest_filter=DracoF.t0_no_draco
/// @param  
TEST_F(DracoF, t0_no_draco) {
    auto zero_pnts = test_data("cesium/pnts/0.pnts");
    ASSERT_TRUE(fs::is_regular_file(zero_pnts))
        << "File: " << zero_pnts.string();
}

/// @brief
/// @param --gtest_filter=DracoF.decodePnts
/// @param
TEST_F(DracoF, decodePnts) {
    using namespace cesium_pnts;

    auto zero_pnts = test_data("cesium/pnts/0-draco.pnts");
    ASSERT_TRUE(fs::is_regular_file(zero_pnts))
        << "File: " << zero_pnts.string();

    CesiumPntsDecoder sot;
    PointCloudData actual;
    ASSERT_TRUE(sot.loadPntsFile(zero_pnts.string(), actual));
#if _DEBUG
    sot.printStatistics(actual);
#endif
    EXPECT_EQ(50779, actual.pointCount);
    EXPECT_EQ(0, actual.normals.size());
    EXPECT_EQ(50779 * 3, actual.colors.size());
    EXPECT_EQ(50779 * 3, actual.batchIds.size());
    EXPECT_EQ(50779 * 3, actual.positions.size());
}

/// @brief
/// @param --gtest_filter=DracoF.decodePnts_no_draco
/// @param
TEST_F(DracoF, decodePnts_no_draco) {
    using namespace cesium_pnts;

    auto zero_pnts = test_data("cesium/pnts/0.pnts");
    ASSERT_TRUE(fs::is_regular_file(zero_pnts))
        << "File: " << zero_pnts.string();

    CesiumPntsDecoder sot;
    PointCloudData actual;
    ASSERT_TRUE(sot.loadPntsFile(zero_pnts.string(), actual));
#if _DEBUG
    sot.printStatistics(actual);
#endif

    EXPECT_EQ(1016024, sot.header.byteLength);
    EXPECT_EQ(156, sot.header.featureTableJSONByteLength);
    EXPECT_EQ(761688, sot.header.featureTableBinaryByteLength);
    EXPECT_EQ(256, sot.header.batchTableJSONByteLength);
    EXPECT_EQ(253896, sot.header.batchTableBinaryByteLength);

    EXPECT_EQ(50779, actual.pointCount);
    EXPECT_EQ(0, actual.normals.size());
    EXPECT_EQ(50779 * 3, actual.colors.size());
    EXPECT_EQ(50779 * 3, actual.batchIds.size());
    EXPECT_EQ(50779 * 3, actual.positions.size());
}
