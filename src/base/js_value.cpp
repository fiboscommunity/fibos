#include "js_value.h"
#include "fibjs.h"
#include "Buffer.h"
#include "v8_api.h"
#include <fc/variant_object.hpp>

using namespace fibjs;

namespace eosio {
namespace chain {

    JSValue js_value(Isolate* isolate, const fc::variant& var)
    {
        JSValue v;
        v8::Local<v8::Context> context = isolate->context();

        switch (var.get_type()) {
        case fc::variant::type_id::null_type:
            return v8::Null(isolate->m_isolate);
        case fc::variant::type_id::int64_type:
            return v8::BigInt::New(isolate->m_isolate, var.as_int64());
        case fc::variant::type_id::uint64_type:
            return v8::BigInt::NewFromUnsigned(isolate->m_isolate, var.as_uint64());
        case fc::variant::type_id::double_type:
            return v8::Number::New(isolate->m_isolate, var.as_double());
        case fc::variant::type_id::bool_type:
            return var.as_bool() ? v8::True(isolate->m_isolate) : v8::False(isolate->m_isolate);
        case fc::variant::type_id::string_type: {
            std::string str = var.as_string();
            return isolate->NewString(str.c_str(), str.length());
        }
        case fc::variant::type_id::array_type: {
            size_t sz = var.size();
            size_t i;
            JSArray arr = v8::Array::New(isolate->m_isolate, (int32_t)sz);

            for (i = 0; i < sz; i++)
                arr->Set(context, (int32_t)i, js_value(isolate, var[i])).IsJust();

            return arr;
        }
        case fc::variant::type_id::object_type: {
            const fc::variant_object& o = var.get_object();
            v8::Local<v8::Object> obj = v8::Object::New(isolate->m_isolate);

            for (fc::variant_object::iterator it = o.begin(); it != o.end(); it++) {
                const std::string& key = it->key();
                obj->Set(context, isolate->NewString(key.c_str(), key.length()), js_value(isolate, it->value())).IsJust();
            }

            return obj;
        }
        case fc::variant::type_id::blob_type: {
            const fc::blob& data = var.get_blob();
            obj_ptr<Buffer_base> buf = new Buffer(data.data.data(), (int32_t)data.data.size());
            return buf->wrap();
        }
        }

        return v;
    }

    Variant js_variant(const fc::variant& var)
    {
        Variant v;

        switch (var.get_type()) {
        case fc::variant::type_id::null_type:
            v.setNull();
            break;
        case fc::variant::type_id::int64_type:
            v = var.as_int64();
            break;
        case fc::variant::type_id::uint64_type:
            v = var.as_int64();
            break;
        case fc::variant::type_id::double_type:
            v = var.as_double();
            break;
        case fc::variant::type_id::bool_type:
            v = var.as_bool();
            break;
        case fc::variant::type_id::string_type: {
            v = exlib::string(var.as_string());
            break;
        }
        case fc::variant::type_id::array_type: {
            size_t sz = var.size();
            size_t i;
            obj_ptr<NArray> arr = new NArray();

            for (i = 0; i < sz; i++)
                arr->append(js_variant(var[i]));

            v = arr;
            break;
        }
        case fc::variant::type_id::object_type: {
            const fc::variant_object& o = var.get_object();
            obj_ptr<NObject> obj = new NObject();

            for (fc::variant_object::iterator it = o.begin(); it != o.end(); it++) {
                const std::string& key = it->key();
                obj->add(key, js_variant(it->value()));
            }

            v = obj;
            break;
        }
        case fc::variant::type_id::blob_type: {
            const fc::blob& data = var.get_blob();
            obj_ptr<Buffer_base> buf = new Buffer(data.data.data(), (int32_t)data.data.size());
            v = buf;
            break;
        }
        }

        return v;
    }

    fc::variant fc_value(Isolate* isolate, const JSValue& var)
    {
        v8::Local<v8::Context> context = isolate->context();
        if (var->IsUndefined() || var->IsNull())
            return fc::variant();
        else if (var->IsString() || var->IsStringObject()) {
            return fc::variant(isolate->toString(var));
        } else if (var->IsBigInt() || var->IsBigIntObject()) {
            return fc::variant(var->ToBigInt(isolate->context()).FromMaybe(v8::Local<v8::BigInt>())->Int64Value());
        } else if (var->IsNumber() || var->IsNumberObject()) {
            return fc::variant(var->NumberValue(context).FromMaybe(0));
        } else if (var->IsBoolean() || var->IsBooleanObject()) {
            return fc::variant(var->BooleanValue(isolate->m_isolate));
        } else if (var->IsArray()) {
            const JSArray arr = JSArray::Cast(var);
            int32_t sz = arr->Length();
            fc::variants vars;

            vars.resize(sz);

            for (int32_t i = 0; i < sz; i++) {
                JSValue v = arr->Get(context, i);
                vars[i] = fc_value(isolate, v);
            }

            return fc::variant(vars);
        } else if (var->IsObject()) {
            obj_ptr<Buffer> buf = Buffer::getInstance(var);
            if (buf != NULL) {
                fc::blob data;

                data.data.resize(buf->length());
                memcpy(data.data.data(), buf->data(), buf->length());

                return fc::variant(data);
            } else {
                const v8::Local<v8::Object> obj = v8::Local<v8::Object>::Cast(var);
                JSArray ks = obj->GetPropertyNames(context);
                int32_t len = ks->Length();
                fc::mutable_variant_object o;

                for (int32_t i = 0; i < len; i++) {
                    JSValue k = ks->Get(context, i);
                    o.set(isolate->toString(k), fc_value(isolate, obj->Get(context, k)));
                }

                return fc::variant(o);
            }
        }

        return fc::variant();
    }

} // namespace chain
} // namespace eosio