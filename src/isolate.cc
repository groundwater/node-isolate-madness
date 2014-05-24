#include <v8.h>
#include <node.h>
#include <stdio.h>
#include <uv.h>
#include <string.h>

using namespace v8;
using namespace node;

typedef struct campjs {
  char * result;
  char * javascript;
  Persistent<Function, CopyablePersistentTraits<Function> > callback;
  Isolate * isolate;
} campjs;

namespace NodeOS
{
  char * doSomeThing(void* args) {
    Isolate* isolate = Isolate::New();
    {
        Locker locker(isolate);
        Isolate::Scope isolateScope(isolate);
        HandleScope handleScope(isolate);
        Handle<Context> context = Context::New(isolate);
        {
            Context::Scope contextScope(context);
            Handle<String> code = String::NewFromUtf8(isolate, (char *) args);
            Handle<String> file = String::NewFromUtf8(isolate, "isolate.js");
            Handle<Script> script = Script::Compile(code, file);

            // run script
            script->Run();

            Handle<Value> v = context->Global()->Get(String::NewFromUtf8(isolate, "exports"));

            String::Utf8Value val(v->ToString());

            return *val;
        }

    }
    isolate->Dispose();
  }

  void doing_work (uv_work_t *req) {
    campjs *a = (campjs*) req->data;
    a->result = doSomeThing(a->javascript);
  }

  void after_doing_work (uv_work_t *req, int i) {
    campjs *a = (campjs*) req->data;
    HandleScope scope(a->isolate);

    Local<Function> local = Local<Function>::New(a->isolate, a->callback);
    Local<Value> results[1];
    results[0] = String::NewFromUtf8(a->isolate, a->result);

    String::Utf8Value name(local->ToString());

    local->Call(Undefined(a->isolate), 1, results);
  }

  void Demo(const FunctionCallbackInfo<Value>& args) {
    String::Utf8Value arg(args[0]->ToString());

    uv_work_t *req = new uv_work_t;

    campjs * cjs = new campjs;
    cjs->javascript = strdup(*arg);
    // cjs->callback = Persistent<Function>::Cast(args[1]);
    req->data = cjs;

    Local<Function> cb = Local<Function>::Cast(args[1]);
    Persistent<Function, CopyablePersistentTraits<Function> > value(args.GetIsolate(), cb);

    cjs->callback = value;
    cjs->isolate  = args.GetIsolate();
//    Persistent<v8::Number> context = Persistent<v8::Number>(args.GetIsolate(), args[1]->ToNumber());

    uv_queue_work(uv_default_loop(), req, doing_work, after_doing_work);
  }

  void init(Handle<Object> target,
            Handle<Value> unused,
            Handle<Context> context) {
    NODE_SET_METHOD(target, "demo", Demo);
  }

}

NODE_MODULE(binding, NodeOS::init)
