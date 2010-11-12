//============================================================================
// Name        : NodeMod.cpp
// Author      : Mrares
// Version     :
// Copyright   : COPY-ME!
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "node/node.h"
#include "node/node_events.h"
#include "libmemcached/memcached.h"

#include "node_memcached.h"

namespace node {

using namespace v8;

void
MemcacheClient::Initialize(Handle<Object> target)
{
    HandleScope scope;

    Local<FunctionTemplate> t =
        FunctionTemplate::New(MemcacheClient::New);

    t->Inherit(EventEmitter::constructor_template);
    t->InstanceTemplate()->SetInternalFieldCount(1);

    NODE_SET_PROTOTYPE_METHOD(t, "addServer",    AddServer);
    NODE_SET_PROTOTYPE_METHOD(t, "removeServer", RemoveServer);
    NODE_SET_PROTOTYPE_METHOD(t, "get",          Get);
    NODE_SET_PROTOTYPE_METHOD(t, "set",          Set);
    NODE_SET_PROTOTYPE_METHOD(t, "remove",       Remove);
    NODE_SET_PROTOTYPE_METHOD(t, "dispose",      Dispose);

    target->Set(String::NewSymbol("Client"), t->GetFunction());
}

Handle<Value>
MemcacheClient::AddServer(const Arguments& args)
{
    MemcacheClient* client = ObjectWrap::Unwrap<MemcacheClient>(args.This());

    HandleScope scope;

    if (args.Length() < 2) {
        return ThrowException(String::New("Not enough arguments"));
    }

    String::Utf8Value hostname(args[0]->ToString());
    uint32_t port = args[1]->Uint32Value();

    return Boolean::New(client->mc->addServer(*hostname, port));
}

Handle<Value>
MemcacheClient::RemoveServer(const Arguments& args)
{
    MemcacheClient* client = ObjectWrap::Unwrap<MemcacheClient>(args.This());

    HandleScope scope;

    if (args.Length() < 2) {
        return ThrowException(String::New("Not enough arguments"));
    }

    String::Utf8Value hostname(args[0]->ToString());
    uint32_t port = args[1]->Uint32Value();

    return Boolean::New(client->mc->removeServer(*hostname, port));
}

Handle<Value>
MemcacheClient::Get(const Arguments& args)
{
    MemcacheClient* client = ObjectWrap::Unwrap<MemcacheClient>(args.This());

    HandleScope scope;

    if (args.Length() < 1) {
        return ThrowException(String::New("Not enough arguments"));
    }

    String::Utf8Value key(args[0]->ToString());
    std::vector<char> data;

    // XXX this might throw an exception: we *must* catch it
    bool found = client->mc->get(*key, data);

    if (found) {
        // this works because std::vector<char>::[] returns a reference!
        //
        // XXX: this will only work for ASCIIZ data! Need Blob type in v8.
        // See here: http://code.google.com/p/v8/issues/detail?id=270
        //
        return String::New((const char*)&data[0], data.size());
    }
    else {
        return Undefined();
    }
}

static void
string_to_vector(Local<String> s, std::vector<char>& vec)
{
    HandleScope scope;

    String::Utf8Value value(s);
    int len = s->Utf8Length();
    vec.reserve(len);
    vec.assign(*value, (*value)+len);
}

Handle<Value>
MemcacheClient::Set(const Arguments& args)
{
    MemcacheClient* client = ObjectWrap::Unwrap<MemcacheClient>(args.This());

    HandleScope scope;

    if (args.Length() < 3) {
        return ThrowException(String::New("Not enough arguments"));
    }

    //
    // XXX: this will only work for ASCIIZ data! Need Blob type in v8 in order
    //      to correctly handle binary values in memcached.
    //
    //      See here: http://code.google.com/p/v8/issues/detail?id=270
    //
    String::Utf8Value key(args[0]->ToString());
    String::Utf8Value value(args[1]->ToString());
    std::vector<char> value_vec;
    string_to_vector(args[1]->ToString(), value_vec);
    time_t expire = args[2]->Uint32Value();
    uint32_t flags = 0;
    if (args.Length() >= 4) {
        flags = args[3]->Uint32Value();
    }

    //
    // XXX the call to set may throw an exception. we *must* catch it
    //
    return Boolean::New(client->mc->set(*key, value_vec, expire, flags));
}

Handle<Value>
MemcacheClient::Remove(const Arguments& args)
{
    MemcacheClient* client = ObjectWrap::Unwrap<MemcacheClient>(args.This());

    if (args.Length() < 1)
        return ThrowException(String::New("Not enough arguments"));

    String::Utf8Value key(args[0]->ToString());

    return Boolean::New(client->mc->remove(*key));
}

Handle<Value>
MemcacheClient::Dispose(const Arguments& args)
{
    MemcacheClient* client = ObjectWrap::Unwrap<MemcacheClient>(args.This());

    if (client->mc != NULL) {
        delete client->mc;
        client->mc = NULL;
    }

    return Undefined();
}

MemcacheClient::MemcacheClient()
    : EventEmitter()
{
    mc = new memcache::Memcache();
}

MemcacheClient::~MemcacheClient()
{
    delete mc;
    mc = NULL;
}

Handle<Value>
MemcacheClient::New(const Arguments& args)
{
    HandleScope scope;

    MemcacheClient* client = new MemcacheClient();
    client->Wrap(args.This());

    return args.This();
}

};

extern "C" void
init(v8::Handle<v8::Object> target)
{
    v8::HandleScope scope;
    node::MemcacheClient::Initialize(target);
}
