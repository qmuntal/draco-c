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
