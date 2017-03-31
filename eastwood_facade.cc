#include <node.h>
#include "eastwood.h"

namespace ew {

using v8::Local;
using v8::Object;

void InitAll(Local<Object> exports) {
  EastWood::Init(exports);
}

NODE_MODULE(eastwood, InitAll)

}  // namespace ew
