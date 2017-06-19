#include <nan.h>

using namespace v8;

const uint32_t maxPossibleNumber = 4294967295;
const uint32_t minPossibleNumber = 0;

uv_check_t check_handle;
uint32_t min;
uint32_t max;
uint32_t num;
uint32_t sum;

uint32_t previous_now;

void reset() {
  min = maxPossibleNumber;
  max = minPossibleNumber;
  num = 0;
  sum = 0;
  previous_now = maxPossibleNumber;
}

// Original version checked uv_hrtime() against uv_now(handle->loop), which
// doesn't seem to work -- it doesn't detect e.g. long event loop iterations
// caused by a large for loop. Checking against the time of the same point at
// the last iteration of the event loop does.
void on_check(uv_check_t* handle) {
  // convert to milliseconds
  const uint64_t now = uv_hrtime() / static_cast<uint64_t>(1e6);
  uint64_t duration;
  if (previous_now >= now) {
    duration = 0;
  } else {
    duration = now - previous_now;
  }

  previous_now = now;

  num += 1;
  sum += duration;
  if (duration < min) {
    min = duration;
  }
  if (duration > max) {
    max = duration;
  }
}


static NAN_METHOD(sense) {
  // reset min and max counters when there were no calls.
  if (num == 0) {
    min = 0;
    max = 0;
  }

  Local<Object> obj = Nan::New<Object>();
  Nan::Set(
    obj,
    Nan::New("min").ToLocalChecked(),
    Nan::New<Number>(static_cast<double>(min))
  );
  Nan::Set(
    obj,
    Nan::New("max").ToLocalChecked(),
    Nan::New<Number>(static_cast<double>(max))
  );
  Nan::Set(
    obj,
    Nan::New("num").ToLocalChecked(),
    Nan::New<Number>(static_cast<double>(num))
  );
  Nan::Set(
    obj,
    Nan::New("sum").ToLocalChecked(),
    Nan::New<Number>(static_cast<double>(sum))
  );

  reset();

  info.GetReturnValue().Set(obj);
}


NAN_MODULE_INIT(init) {
  reset();

  uv_check_init(uv_default_loop(), &check_handle);
  uv_check_start(&check_handle, reinterpret_cast<uv_check_cb>(on_check));
  uv_unref(reinterpret_cast<uv_handle_t*>(&check_handle));

  Nan::Set(target,
    Nan::New("sense").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(sense)).ToLocalChecked()
  );
}

NODE_MODULE(eventLoopStats, init)
