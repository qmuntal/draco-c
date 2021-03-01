#include <cstring>

#include "c_api.h"
#include "draco/attributes/point_attribute.h"

size_t dracoPointAttrSize(const draco_point_attr* attr) {
  return reinterpret_cast<const draco::PointAttribute*>(attr)->size();
}

draco_geometry_attr_type dracoPointAttrType(const draco_point_attr* attr) {
  auto attr_type = reinterpret_cast<const draco::PointAttribute*>(attr)->attribute_type();
  return static_cast<draco_geometry_attr_type>(attr_type);
}

draco_data_type dracoPointAttrDataType(const draco_point_attr* attr) {
  auto attr_type = reinterpret_cast<const draco::PointAttribute*>(attr)->data_type();
  return static_cast<draco_data_type>(attr_type);
}

int8_t dracoPointAttrNumComponents(const draco_point_attr* attr) {
  return reinterpret_cast<const draco::PointAttribute*>(attr)->num_components();
}

bool dracoPointAttrNormalized(const draco_point_attr* attr) {
  return reinterpret_cast<const draco::PointAttribute*>(attr)->normalized();
}

int64_t dracoPointAttrByteStride(const draco_point_attr* attr) {
  return reinterpret_cast<const draco::PointAttribute*>(attr)->byte_stride();
}

int64_t dracoPointAttrByteOffset(const draco_point_attr* attr) {
  return reinterpret_cast<const draco::PointAttribute*>(attr)->byte_offset();
}

uint32_t dracoPointAttrUniqueId(const draco_point_attr* attr) {
  return reinterpret_cast<const draco::PointAttribute*>(attr)->unique_id();
}
