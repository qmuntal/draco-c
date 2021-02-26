#include "c_api.h"
#include "draco/mesh/mesh.h"

draco_mesh* dracoNewMesh() {
  return reinterpret_cast<draco_mesh*>(new draco::Mesh());
}

void dracoMeshRelease(draco_mesh *mesh) {
  free(mesh);
}

uint32_t dracoMeshNumFaces(const draco_mesh *mesh) {
  return reinterpret_cast<const draco::Mesh*>(mesh)->num_faces();
}


uint32_t dracoMeshNumPoints(const draco_mesh *mesh) {
  return reinterpret_cast<const draco::Mesh*>(mesh)->num_points();
}

int32_t dracoMeshNumAttrs(const draco_mesh *mesh) {
  return reinterpret_cast<const draco::Mesh*>(mesh)->num_attributes();
}

template <typename T>
bool GetTrianglesArray(const draco::Mesh *m, const size_t out_size,
                       T *out_values) {
  const uint32_t num_faces = m->num_faces();
  if (num_faces * 3 * sizeof(T) != out_size) {
    return false;
  }

  for (uint32_t face_id = 0; face_id < num_faces; ++face_id) {
    const draco::Mesh::Face &face = m->face(draco::FaceIndex(face_id));
    out_values[face_id * 3 + 0] = static_cast<T>(face[0].value());
    out_values[face_id * 3 + 1] = static_cast<T>(face[1].value());
    out_values[face_id * 3 + 2] = static_cast<T>(face[2].value());
  }
  return true;
}

bool dracoMeshGetTrianglesUint16(const draco_mesh *mesh, const size_t out_size,
                                 uint16_t *out_values) {
  auto m = reinterpret_cast<const draco::Mesh*>(mesh);
  if (m->num_points() > std::numeric_limits<uint16_t>::max()) {
    return false;
  }

  return GetTrianglesArray(m, out_size, out_values);
}

bool dracoMeshGetTrianglesUint32(const draco_mesh *mesh, const size_t out_size,
                                 uint32_t *out_values) {
  auto m = reinterpret_cast<const draco::Mesh*>(mesh);
  if (m->num_points() > std::numeric_limits<uint32_t>::max()) {
    return false;
  }

  return GetTrianglesArray(m, out_size, out_values);
}

const draco_point_attr* dracoMeshGetAttribute(const draco_mesh *mesh, int32_t att_id) {
  auto m = reinterpret_cast<const draco::Mesh*>(mesh);
  if (att_id < 0 || att_id > m->num_attributes()) {
    return nullptr;
  }
  auto attr = m->attribute(att_id);
  return reinterpret_cast<const draco_point_attr*>(attr);
}

const draco_point_attr* dracoMeshGetAttributeByUniqueId(const draco_mesh *mesh, uint32_t unique_id) {
  auto m = reinterpret_cast<const draco::Mesh*>(mesh);
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

bool dracoMeshGetAttributeData(const draco_mesh *pc,
                               const draco_point_attr *pa,
                               dracoDataType data_type,
                               const size_t out_size,
                               void *out_values) {
  auto pcc = reinterpret_cast<const draco::Mesh*>(pc);
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

int32_t dracoMeshGetNamedAttributeId(const draco_mesh *mesh, dracoDataType data_type) {
  auto m = reinterpret_cast<const draco::Mesh*>(mesh); 
  return m->GetNamedAttributeId(static_cast<draco::GeometryAttribute::Type>(data_type));
}