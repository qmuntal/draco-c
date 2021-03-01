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

bool GetTrianglesArray(const draco::Mesh *m, const size_t out_size,
                       uint32_t *out_values) {
  const uint32_t num_faces = m->num_faces();
  if (num_faces * 3 * sizeof(uint32_t) != out_size) {
    return false;
  }

  for (uint32_t face_id = 0; face_id < num_faces; ++face_id) {
    const draco::Mesh::Face &face = m->face(draco::FaceIndex(face_id));
    out_values[face_id * 3 + 0] = face[0].value();
    out_values[face_id * 3 + 1] = face[1].value();
    out_values[face_id * 3 + 2] = face[2].value();
  }
  return true;
}

bool dracoMeshGetIndices(const draco_mesh *mesh, const size_t out_size,
                         uint32_t *out_values) {
  auto m = reinterpret_cast<const draco::Mesh*>(mesh);
  if (m->num_points() > std::numeric_limits<uint32_t>::max()) {
    return false;
  }

  return GetTrianglesArray(m, out_size, out_values);
}
