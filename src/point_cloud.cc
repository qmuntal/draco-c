#include "c_api.h"
#include "draco/point_cloud/point_cloud.h"

draco_point_cloud* dracoNewPointCloud() {
  return reinterpret_cast<draco_point_cloud*>(new draco::PointCloud());
}

void dracoPointCloudRelease(draco_point_cloud *pc) {
  free(pc);
}

uint32_t dracoPointCloudNumPoints(const draco_point_cloud *pc) {
  return reinterpret_cast<const draco::PointCloud*>(pc)->num_points();
}

int32_t dracoPointCloudNumAttrs(const draco_point_cloud *pc) {
  return reinterpret_cast<const draco::PointCloud*>(pc)->num_attributes();
}

const draco_point_attr* dracoPointCloudGetAttribute(const draco_point_cloud *pc, int32_t att_id) {
  auto m = reinterpret_cast<const draco::PointCloud*>(pc);
  if (att_id < 0 || att_id > m->num_attributes()) {
    return nullptr;
  }
  auto attr = m->attribute(att_id);
  return reinterpret_cast<const draco_point_attr*>(attr);
}

int32_t dracoPointCloudGetNamedAttributeId(const draco_point_cloud *pc, draco_geometry_type geo_type) {
  auto type = static_cast<draco::GeometryAttribute::Type>(geo_type);
  return reinterpret_cast<const draco::PointCloud*>(pc)->GetNamedAttributeId(type);
}

const draco_point_attr* dracoPointCloudGetAttributeByUniqueId(const draco_point_cloud *pc, uint32_t unique_id) {
  auto m = reinterpret_cast<const draco::PointCloud*>(pc);
  auto attr = m->GetAttributeByUniqueId(unique_id);
  return reinterpret_cast<const draco_point_attr*>(attr);
}


template <class T>
static bool GetAttributeDataArrayForAllPoints(const draco::PointCloud *pc,
                                              const draco::PointAttribute *pa,
                                              draco::DataType type,
                                              size_t out_size,
                                              void *out_values) {
  const int components = pa->num_components();
  const int num_points = pc->num_points();
  const int data_size = num_points * components * sizeof(T);
  if (data_size != out_size) {
    return false;
  }
  const bool requested_type_matches = pa->data_type() == type;
  if (requested_type_matches && pa->is_mapping_identity()) {
    // Copy values directly to the output vector.
    const auto ptr = pa->GetAddress(draco::AttributeValueIndex(0));
    ::memcpy(out_values, ptr, data_size);
    return true;
  }

  // Copy values one by one.
  std::vector<T> values(components);
  int entry_id = 0;

  T *const typed_output = reinterpret_cast<T *>(out_values);
  for (draco::PointIndex i(0); i < num_points; ++i) {
    const draco::AttributeValueIndex val_index = pa->mapped_index(i);
    if (requested_type_matches) {
      pa->GetValue(val_index, values.data());
    } else {
      if (!pa->ConvertValue<T>(val_index, values.data())) {
        return false;
      }
    }
    for (int j = 0; j < components; ++j) {
      typed_output[entry_id++] = values[j];
    }
  }
  return true;
}

bool dracoPointCloudGetAttributeData(const draco_point_cloud *pc,
                                     const draco_point_attr *pa,
                                     draco_data_type data_type,
                                     const size_t out_size,
                                     void *out_values) {
  auto pcc = reinterpret_cast<const draco::PointCloud*>(pc);
  auto pac = reinterpret_cast<const draco::PointAttribute*>(pa);
  switch (data_type) {
    case DT_INT8:
      return GetAttributeDataArrayForAllPoints<int8_t>(pcc, pac, draco::DT_INT8,
                                                       out_size, out_values);
    case DT_INT16:
      return GetAttributeDataArrayForAllPoints<int16_t>(pcc, pac, draco::DT_INT16,
                                                        out_size, out_values);
    case DT_INT32:
      return GetAttributeDataArrayForAllPoints<int32_t>(pcc, pac, draco::DT_INT32,
                                                        out_size, out_values);
    case DT_UINT8:
      return GetAttributeDataArrayForAllPoints<uint8_t>(pcc, pac, draco::DT_UINT8,
                                                        out_size, out_values);
    case DT_UINT16:
      return GetAttributeDataArrayForAllPoints<uint16_t>(pcc, pac, draco::DT_UINT16,
                                                         out_size, out_values);
    case DT_UINT32:
      return GetAttributeDataArrayForAllPoints<uint32_t>(pcc, pac, draco::DT_UINT32,
                                                         out_size, out_values);
    case DT_FLOAT32:
      return GetAttributeDataArrayForAllPoints<float>(pcc, pac, draco::DT_FLOAT32,
                                                      out_size, out_values);
    case DT_FLOAT64:
      return GetAttributeDataArrayForAllPoints<double>(pcc, pac, draco::DT_FLOAT64,
                                                       out_size, out_values);
    default:
      return false;
  }
}