#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "gtest/gtest.h"

#include "c_api.h"

namespace {

static constexpr char kTestDataDir[] = DRACO_C_TEST_DATA_DIR;

std::string GetTestFileFullPath(const std::string &file_name) {
  return std::string(kTestDataDir) + std::string("/") + file_name;
}

std::vector<char> ReadFile(const std::string &file_name) {
  std::ifstream input_file(GetTestFileFullPath(file_name),
                           std::ios::binary);
  // Read the file stream into a buffer.
  std::streampos file_size = 0;
  input_file.seekg(0, std::ios::end);
  file_size = input_file.tellg() - file_size;
  input_file.seekg(0, std::ios::beg);
  std::vector<char> data(file_size);
  input_file.read(data.data(), file_size);
  return data;
}

TEST(DracoCAPITest, TestDecode) {
  auto data = ReadFile("test_nm.obj.edgebreaker.cl4.2.2.drc");
  auto egt_type = dracoGetEncodedGeometryType(data.data(), data.size());
  ASSERT_EQ(egt_type, DRACO_EGT_TRIANGULAR_MESH);
  auto mesh = dracoNewMesh();
  auto decoder = dracoNewDecoder();
  dracoDecoderDecodeMesh(decoder, data.data(), data.size(), mesh);
  dracoDecoderRelease(decoder);
  ASSERT_NE(mesh, nullptr);
  auto num_faces = dracoMeshNumFaces(mesh);
  ASSERT_EQ(num_faces, 170);
  ASSERT_EQ(dracoPointCloudNumPoints(mesh), 99);
  auto num_attrs = dracoPointCloudNumAttrs(mesh);
  ASSERT_EQ(num_attrs, 2);

  auto indices_size = 3 * num_faces * sizeof(uint32_t);
  uint32_t *indices = (uint32_t *)malloc(indices_size);
  ASSERT_TRUE(dracoMeshGetTrianglesUint32(mesh, indices_size, indices));
  free(indices);

  auto pa1 = dracoPointCloudGetAttribute(mesh, 0);
  ASSERT_EQ(dracoPointAttrType(pa1), DRACO_GAT_POSITION);
  ASSERT_EQ(dracoPointAttrDataType(pa1), DRACO_DT_FLOAT32);
  ASSERT_EQ(dracoPointAttrNumComponents(pa1), 3);
  ASSERT_FALSE(dracoPointAttrNormalized(pa1));
  ASSERT_EQ(dracoPointAttrByteStride(pa1), 12);
  ASSERT_EQ(dracoPointAttrUniqueId(pa1), 0);

  auto pa2 = dracoPointCloudGetAttribute(mesh, 1);
  ASSERT_EQ(dracoPointAttrType(pa2), DRACO_GAT_NORMAL);
  ASSERT_EQ(dracoPointAttrDataType(pa2), DRACO_DT_FLOAT32);
  ASSERT_EQ(dracoPointAttrNumComponents(pa2), 3);
  ASSERT_FALSE(dracoPointAttrNormalized(pa2));
  ASSERT_EQ(dracoPointAttrByteStride(pa2), 12);
  ASSERT_EQ(dracoPointAttrUniqueId(pa2), 1);

  auto arr_size = 3*99*sizeof(float);
  auto arr = (float *)malloc(arr_size);
  ASSERT_TRUE(dracoPointCloudGetAttributeData(mesh, pa2, DRACO_DT_FLOAT32, arr_size, arr));
  ASSERT_FALSE(dracoPointCloudGetAttributeData(mesh, pa2, DRACO_DT_FLOAT32, arr_size+1, arr));
  ASSERT_FALSE(dracoPointCloudGetAttributeData(mesh, pa2, DRACO_DT_FLOAT32, arr_size-1, arr));

  dracoMeshRelease(mesh);
}

}  // namespace
