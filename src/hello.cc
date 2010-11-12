#include <node/v8.h>

using namespace v8;

extern "C" void
init (Handle<Object> target)
{
  HandleScope scope;
//  target->New(1);
//  target->Set(0, String::New("Hello"));
//  target->Set(1, String::New("world!"));
//  target->Set(Int32::New(1),String::New('World'));
  target->Set(String::New("hello"), String::New("World"));
  target->Set(String::New("Atlantic"), String::New("Ocean"));
}

