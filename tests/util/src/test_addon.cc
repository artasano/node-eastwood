#include <node.h>
#include "addon_class.h"
#include "event_callback_class.h"

namespace addon_test {

using v8::Local;
using v8::Object;

void InitAll(Local<Object> exports) {
  AddonClass::Init(exports);
  EventCallbackClass::Init(exports);
}

NODE_MODULE(test_addon, InitAll)

}  // namespace addon_test
