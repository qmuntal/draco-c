#include <cstring>

#include "c_api.h"
#include "draco/compression/decode.h"

void dracoStatusRelease(draco_status *status) {
  free(status);
}

int dracoStatusCode(const draco_status *status) {
  return reinterpret_cast<const draco::Status*>(status)->code();
}

bool dracoStatusOk(const draco_status *status) {
  return reinterpret_cast<const draco::Status*>(status)->ok();
}

draco_string dracoStatusErrorMsg(const draco_status *status) {
  auto msg = reinterpret_cast<const draco::Status*>(status)->error_msg();
  return reinterpret_cast<draco_string>(msg);
}

draco_decoder* dracoNewDecoder() {
  return reinterpret_cast<draco_decoder*>(new draco::Decoder());
}

void dracoDecoderRelease(draco_decoder *decoder) {
  free(decoder);
}

draco_status* dracoDecoderArrayToMesh(draco_decoder *decoder, 
                                  const char *data, 
                                  size_t data_size,
                                  draco_mesh *out_mesh) {
  draco::DecoderBuffer buffer;
  buffer.Init(data, data_size);
  auto m = reinterpret_cast<draco::Mesh*>(out_mesh);
  const auto &last_status_ = reinterpret_cast<draco::Decoder*>(decoder)->DecodeBufferToGeometry(&buffer, m);
  return reinterpret_cast<draco_status*>(new draco::Status(last_status_));
}
