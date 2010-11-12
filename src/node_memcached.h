#ifndef node_memcached_h
#define node_memcached_h

#include "node.h"
#include "libmemcached/memcached.hpp"

namespace node {

class MemcacheClient : public EventEmitter {
public:
    static void
        Initialize(v8::Handle<v8::Object> target);
    static v8::Handle<v8::Value>
        AddServer(const v8::Arguments& args);
    static v8::Handle<v8::Value>
        RemoveServer(const v8::Arguments& args);
    static v8::Handle<v8::Value>
        Get(const v8::Arguments& args);
    static v8::Handle<v8::Value>
        Set(const v8::Arguments& args);
    static v8::Handle<v8::Value>
        Remove(const v8::Arguments& args);
    static v8::Handle<v8::Value>
        Dispose(const v8::Arguments& args);

private:
    MemcacheClient();
    virtual ~MemcacheClient();

    static v8::Handle<v8::Value>
        New(const v8::Arguments& args);

    memcache::Memcache* mc;
};

};

#endif
