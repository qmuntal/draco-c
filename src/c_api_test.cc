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

draco_mesh *DecodeToDracoMesh(const std::string &file_name) {
  std::ifstream input_file(GetTestFileFullPath(file_name),
                           std::ios::binary);
  if (!input_file) {
    return nullptr;
  }
  // Read the file stream into a buffer.
  std::streampos file_size = 0;
  input_file.seekg(0, std::ios::end);
  file_size = input_file.tellg() - file_size;
  input_file.seekg(0, std::ios::beg);
  std::vector<char> data(file_size);
  input_file.read(data.data(), file_size);
  if (data.empty()) {
    return nullptr;
  }
  
  auto mesh = dracoNewMesh();
  auto decoder = dracoNewDecoder();
  dracoDecoderArrayToMesh(decoder, data.data(), data.size(), mesh);
  dracoDecoderRelease(decoder);
  return mesh;
}

TEST(DracoCAPITest, TestDecode) {
  auto mesh = DecodeToDracoMesh("test_nm.obj.edgebreaker.cl4.2.2.drc");
  ASSERT_NE(mesh, nullptr);
  auto num_faces = dracoMeshNumFaces(mesh);
  ASSERT_EQ(num_faces, 170);
  ASSERT_EQ(dracoMeshNumPoints(mesh), 99);
  auto num_attrs = dracoMeshNumAttrs(mesh);
  ASSERT_EQ(num_attrs, 2);

  auto indices_size = 3 * num_faces * sizeof(uint32_t);
  uint32_t *indices = (uint32_t *)malloc(indices_size);
  ASSERT_TRUE(dracoMeshGetTrianglesUint32(mesh, indices_size, indices));
  free(indices);

  auto pa1 = dracoMeshGetAttribute(mesh, 0);
  ASSERT_EQ(dracoPointAttrType(pa1), GT_POSITION);
  ASSERT_EQ(dracoPointAttrDataType(pa1), DT_FLOAT32);
  ASSERT_EQ(dracoPointAttrNumComponents(pa1), 3);
  ASSERT_FALSE(dracoPointAttrNormalized(pa1));
  ASSERT_EQ(dracoPointAttrByteStride(pa1), 12);
  ASSERT_EQ(dracoPointAttrUniqueId(pa1), 0);

  auto pa2 = dracoMeshGetAttribute(mesh, 1);
  ASSERT_EQ(dracoPointAttrType(pa2), GT_NORMAL);
  ASSERT_EQ(dracoPointAttrDataType(pa2), DT_FLOAT32);
  ASSERT_EQ(dracoPointAttrNumComponents(pa2), 3);
  ASSERT_FALSE(dracoPointAttrNormalized(pa2));
  ASSERT_EQ(dracoPointAttrByteStride(pa2), 12);
  ASSERT_EQ(dracoPointAttrUniqueId(pa2), 1);

  auto arr_size = 3*99*sizeof(float);
  auto arr = (float *)malloc(arr_size);
  ASSERT_TRUE(dracoMeshGetAttributeData(mesh, pa2, DT_FLOAT32, arr_size, arr));
  ASSERT_FALSE(dracoMeshGetAttributeData(mesh, pa2, DT_FLOAT32, arr_size+1, arr));
  ASSERT_FALSE(dracoMeshGetAttributeData(mesh, pa2, DT_FLOAT32, arr_size-1, arr));

  dracoMeshRelease(mesh);
}

}  // namespace
