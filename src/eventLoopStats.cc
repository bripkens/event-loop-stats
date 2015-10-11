#include <nan.h>

using namespace v8;

const uint32_t maxPossibleNumber = 4294967295;
const uint32_t minPossibleNumber = 0;

uv_check_t check_handle;
uint32_t min;
uint32_t max;
uint32_t num;
uint32_t sum;

void reset() {
  min = maxPossibleNumber;
  max = minPossibleNumber;
  num = 0;
  sum = 0;
}

// See the following documentation for reference of what 'check'
// means and when it is executed + the loop now time updates:
// http://docs.libuv.org/en/v1.x/design.html#the-i-o-loop
void on_check(uv_check_t* handle) {
  const uint64_t start_time = uv_now(handle->loop);
  // uv_hrtime is expressed in nanos, but the loop start time is
  // expressed in millis.
  const uint64_t now = uv_hrtime() / static_cast<uint64_t>(1e6);
  uint64_t duration;
  if (start_time >= now) {
    duration = 0;
  } else {
    duration = now - start_time;
  }

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
