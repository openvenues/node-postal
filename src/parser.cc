#include <node.h>
#include <libpostal/libpostal.h>

#include "v8_utils.h"

#define PARSER_USAGE "Usage: parse_address(address[, options])"

namespace libpostal {
    using v8::FunctionCallbackInfo;
    using v8::Isolate;
    using v8::Local;
    using v8::Exception;
    using v8::Object;
    using v8::Object;
    using v8::Array;
    using v8::String;
    using v8::Value;


    void ParseAddress(const FunctionCallbackInfo<Value> &args) {
        Isolate *isolate = args.GetIsolate();

        if (args.Length() < 1) {
            isolate->ThrowException(Exception::TypeError(
                String::NewFromUtf8(isolate, PARSER_USAGE)
            ));
            return;
        }

        if (!args[0]->IsString()) {
            isolate->ThrowException(Exception::TypeError(
                String::NewFromUtf8(isolate, PARSER_USAGE)
            ));
        }

        char *address = v8_to_c_string(args[0]);
        if (address == NULL) {
            isolate->ThrowException(Exception::TypeError(
                String::NewFromUtf8(isolate, "Could not convert first arg to string")
            ));
            return;
        }

        char *language = NULL;
        char *country = NULL;

        uint64_t i;

        address_parser_options_t options = LIBPOSTAL_ADDRESS_PARSER_DEFAULT_OPTIONS;

        if (args.Length() > 1 && args[1]->IsObject()) {
            Local<Object> props = args[1]->ToObject();
            Local<Array> prop_names = props->GetOwnPropertyNames();

            for (i = 0; i < prop_names->Length(); i++) {
                Local<Value> key = prop_names->Get(i);

                if (key->IsString()) {
                    v8::String::Utf8Value utf8_key(key);
                    char *key_string = *utf8_key;

                    Local<Value> value = props->Get(key);

                    if (strcmp(key_string, "language") == 0) {
                        language = v8_to_c_string(value);
                        if (language != NULL) {
                            options.language = language;
                        }
                    } else if (strcmp(key_string, "country") == 0) {
                        country = v8_to_c_string(value);
                        if (country != NULL) {
                            options.country = country;
                        }
                    }
                }

            }

        }

        address_parser_response_t *response = parse_address(address, options);

        free(address);

        if (language != NULL) {
            free(language);
        }

        if (country != NULL) {
            free(country);
        }

        if (response == NULL) {
            isolate->ThrowException(Exception::TypeError(
                String::NewFromUtf8(isolate, "Error parsing address")
            ));
        }

        Local<Array> ret = Array::New(isolate, response->num_components);

        Local<String> name_key = String::NewFromUtf8(isolate, "component");
        Local<String> label_key = String::NewFromUtf8(isolate, "label");

        for (i = 0; i < response->num_components; i++) {
            char *component = response->components[i];
            char *label = response->labels[i];

            Local<Object> o = Object::New(isolate);
            o->Set(name_key, String::NewFromUtf8(isolate, component));
            o->Set(label_key, String::NewFromUtf8(isolate, label));

            ret->Set(i, o);
        }

        address_parser_response_destroy(response);

        args.GetReturnValue().Set(ret);
    }

    static void cleanup(void*) {
        libpostal_teardown();
        libpostal_teardown_parser();
    }

    void init(Local<Object> exports) {
        Isolate* isolate = exports->GetIsolate();
        if (!libpostal_setup() || !libpostal_setup_parser()) {
            isolate->ThrowException(Exception::Error(
                String::NewFromUtf8(isolate, "Could not load libpostal")
            ));
            return;
        }

        NODE_SET_METHOD(exports, "parse_address", ParseAddress);


        node::AtExit(cleanup);
    }

    NODE_MODULE(expand, init)

}