#include <cstring>

#include "c_api.h"
#include "draco/compression/decode.h"

draco_encoded_geometry_type dracoGetEncodedGeometryType(const char *data, size_t data_size) {
  draco::DecoderBuffer buffer;
  buffer.Init(data, data_size);
  auto type = draco::Decoder::GetEncodedGeometryType(&buffer);
  if (!type.ok()) {
    return DRACO_EGT_INVALID;
  }
  return static_cast<draco_encoded_geometry_type>(type.value());
}

void dracoStatusRelease(draco_status *status) {
  free(status);
}

int dracoStatusCode(const draco_status *status) {
  return reinterpret_cast<const draco::Status*>(status)->code();
}

bool dracoStatusOk(const draco_status *status) {
  return reinterpret_cast<const draco::Status*>(status)->ok();
}

size_t dracoStatusErrorMsgLength(const draco_status *status) {
  auto msg = reinterpret_cast<const draco::Status*>(status)->error_msg_string();
  return msg.size() + 1;
}

size_t dracoStatusErrorMsg(const draco_status *status, char *msg, size_t length) {
  if (msg == nullptr) {
    return 0;
  }
  auto msg_ = reinterpret_cast<const draco::Status*>(status)->error_msg_string();
  if (msg_.size() >= length) {
    return 0;
  }
  memcpy(msg, msg_.c_str(), length);
  return msg_.size() + 1;
}

draco_decoder* dracoNewDecoder() {
  return reinterpret_cast<draco_decoder*>(new draco::Decoder());
}

void dracoDecoderRelease(draco_decoder *decoder) {
  free(decoder);
}

draco_status* dracoDecoderDecodeMesh(draco_decoder *decoder, 
                                     const char *data, 
                                     size_t data_size,
                                     draco_mesh *out_mesh) {
  draco::DecoderBuffer buffer;
  buffer.Init(data, data_size);
  auto m = reinterpret_cast<draco::Mesh*>(out_mesh);
  const auto &last_status_ = reinterpret_cast<draco::Decoder*>(decoder)->DecodeBufferToGeometry(&buffer, m);
  return reinterpret_cast<draco_status*>(new draco::Status(last_status_));
}

draco_status* dracoDecoderDecodePointCloud(draco_decoder *decoder, 
                                           const char *data, 
                                           size_t data_size,
                                           draco_point_cloud *out_pc) {
  draco::DecoderBuffer buffer;
  buffer.Init(data, data_size);
  auto m = reinterpret_cast<draco::PointCloud*>(out_pc);
  const auto &last_status_ = reinterpret_cast<draco::Decoder*>(decoder)->DecodeBufferToGeometry(&buffer, m);
  return reinterpret_cast<draco_status*>(new draco::Status(last_status_));
}
