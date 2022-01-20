#include <nan.h>

using namespace v8;

// Casting -1 to an uint will give the max uint value
const uint32_t maxPossibleUint32 = -1;
const uint64_t maxPossibleUint64 = -1;
const uint32_t minPossibleUint32 = 0;

uv_check_t check_handle;

// The minimum event loop duration since the last sense() call.
uint32_t min;
// The maximum event loop duration since the last sense() call.
uint32_t max;
// The sum of event loop durations the last sense() call.
uint32_t sum;
// The number of event loop iterations since the last sense() call.
uint32_t num;

uint64_t previous_now = maxPossibleUint64;

// This will be called after each sense call.
void reset() {
  min = maxPossibleUint32;
  max = minPossibleUint32;
  sum = 0;
  num = 0;
}

// This will be called once per event loop iteration, right after polling
// for i/o. See http://docs.libuv.org/en/v1.x/design.html#the-i-o-loop for a
// general overview and http://docs.libuv.org/en/v1.x/check.html for details.
void on_check(uv_check_t* handle) {
  // An earlier incarnation of event-loop-stats checked uv_hrtime() against
  // uv_now(handle->loop), which doesn't seem to work -- for example, it does
  // not detect long event loop iterations caused by a synchronous block due to
  // long running for- or while-loops. Checking against the time of the same
  // point at the last iteration of the event loop also covers these cases.

  const uint64_t now = uv_hrtime();
  uint32_t duration;

  if (previous_now >= now) {
    // This only happens on the very first call on_check call. Since we have no
    // timestamp to compare to from an earlier on_check call, we start by
    // assuming an event loop lag of zero.
    duration = 0;
  } else {
    // Calculate the duration since the last on_check call - this is the
    // event loop lag.
    // And convert to milliseconds (uv_hrtime yields nanos).
    duration = (now - previous_now) / static_cast<uint64_t>(1e6);
  }

  // save min/max values
  if (duration < min) {
    min = duration;
  }
  if (duration > max) {
    max = duration;
  }

  // Sum up all durations between two consecutive sense() calls.
  sum += duration;

  // Simply count all event loop iterations between two sense() calls.
  num += 1;

  // Save the current timestamp for the next on_check call for comparison.
  previous_now = now;
}


static NAN_METHOD(sense) {
  // Reset min and max counters when there were no calls.
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
