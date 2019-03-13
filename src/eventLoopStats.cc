#include <nan.h>

using namespace v8;

uv_check_t check_handle;
uint32_t old_usage;
uint32_t min;
uint32_t max;
uint32_t num;
uint32_t sum;

void reset() {
  min = 0;
  max = 0;
  num = 0;
  sum = 0;
}

uint32_t getUsage() {
  uv_rusage_t usage;
  uv_getrusage(&usage);

  return (
    usage.ru_utime.tv_sec * 1e3 +
    usage.ru_utime.tv_usec / 1e3 +
    usage.ru_stime.tv_sec * 1e3 +
    usage.ru_stime.tv_usec / 1e3
  );
}

// See the following documentation for reference of what 'check'
// means and when it is executed + the loop now time updates:
// http://docs.libuv.org/en/v1.x/design.html#the-i-o-loop
void on_check(uv_check_t* handle) {
  const uint32_t new_usage = getUsage();
  uint32_t duration = new_usage - old_usage;

  if (num == 0) {
    min = max = duration;
  }

  num += 1;
  sum += duration;
  if (duration < min) {
    min = duration;
  }
  if (duration > max) {
    max = duration;
  }

  old_usage = new_usage;
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

  old_usage = getUsage();

  uv_check_init(uv_default_loop(), &check_handle);
  uv_check_start(&check_handle, reinterpret_cast<uv_check_cb>(on_check));
  uv_unref(reinterpret_cast<uv_handle_t*>(&check_handle));

  Nan::Set(target,
    Nan::New("sense").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(sense)).ToLocalChecked()
  );
}

NODE_MODULE(eventLoopStats, init)
