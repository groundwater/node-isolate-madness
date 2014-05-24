#include <v8.h>
#include <node.h>
#include <stdio.h>

using namespace v8;
using namespace node;

namespace NodeOS
{

  void Demo(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = Isolate::New();
    {
        Locker locker(isolate);
        Isolate::Scope isolateScope(isolate);
        HandleScope handleScope(isolate);
        Handle<Context> context = Context::New(isolate);
        {
            Context::Scope contextScope(context);
            Handle<String> code = String::NewFromUtf8(isolate, "exports = function (){}");
            Handle<String> file = String::NewFromUtf8(isolate, "isolate.js");
            Handle<Script> script = Script::Compile(code, file);

            // run script
            script->Run();

            Handle<Value> v = context->Global()->Get(String::NewFromUtf8(isolate, "exports"));

            String::Utf8Value val(v->ToString());

            // printf("%s\n", *val);

            Handle<String> s = String::NewFromUtf8(args.GetIsolate(), *val);
            args.GetReturnValue().Set(s);
        }

    }
    isolate->Dispose();

  }

  void init(Handle<Object> target,
            Handle<Value> unused,
            Handle<Context> context) {
    NODE_SET_METHOD(target, "demo", Demo);
  }

}

NODE_MODULE(binding, NodeOS::init)
