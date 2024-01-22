#include <libpostal/libpostal.h>
#include <nan.h>

#define PARSER_USAGE "Usage: parse_address(address[, options])"

void ParseAddress(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate *isolate = info.GetIsolate();
	v8::Local<v8::Context> context = isolate->GetCurrentContext();

    if (info.Length() < 1) {
        Nan::ThrowTypeError(PARSER_USAGE);
        return;
    }

    if (!info[0]->IsString()) {
        Nan::ThrowTypeError(PARSER_USAGE);
    }

    Nan::Utf8String address_utf8(info[0]);
    char *address = *address_utf8;

    if (address == NULL) {
        Nan::ThrowTypeError("Could not convert first arg to string");
        return;
    }

    char *language = NULL;
    char *country = NULL;

    uint64_t i;

    libpostal_address_parser_options_t options = libpostal_get_address_parser_default_options();

    if (info.Length() > 1 && info[1]->IsObject()) {
        v8::Local<v8::Object> props = info[1]->ToObject(context).ToLocalChecked();
        v8::Local<v8::Array> prop_names = Nan::GetPropertyNames(props).ToLocalChecked();

        for (i = 0; i < prop_names->Length(); i++) {
            v8::Local<v8::Value> key = prop_names->Get(context, i).ToLocalChecked();

            if (key->IsString()) {
                Nan::Utf8String utf8_key(key);
                char *key_string = *utf8_key;

                v8::Local<v8::Value> value = Nan::Get(props, key).ToLocalChecked();
                if (strcmp(key_string, "language") == 0) {
                    Nan::Utf8String language_utf8(value);
                    language = *language_utf8;
                    if (language != NULL) {
                        options.language = language;
                    }
                } else if (strcmp(key_string, "country") == 0) {
                    Nan::Utf8String country_utf8(value);
                    country = *country_utf8;
                    if (country != NULL) {
                        options.country = country;
                    }
                }
            }

        }

    }

    libpostal_address_parser_response_t *response = libpostal_parse_address(address, options);

    if (response == NULL) {
        Nan::ThrowError("Error parsing address");
        return;
    }

    v8::Local<v8::Array> ret = Nan::New<v8::Array>(response->num_components);

    v8::Local<v8::String> name_key = Nan::New("value").ToLocalChecked();
    v8::Local<v8::String> label_key = Nan::New("component").ToLocalChecked();

    for (i = 0; i < response->num_components; i++) {
        char *component = response->components[i];
        char *label = response->labels[i];

        v8::Local<v8::Object> o = Nan::New<v8::Object>();
        o->Set(context, name_key, Nan::New(component).ToLocalChecked());
        o->Set(context, label_key, Nan::New(label).ToLocalChecked());

        ret->Set(context, i, o);
    }

    libpostal_address_parser_response_destroy(response);

    info.GetReturnValue().Set(ret);
}

static void cleanup(void*) {
    libpostal_teardown();
    libpostal_teardown_parser();
}

void init(v8::Local<v8::Object> exports) {
    // Check Node.js version
    #if NODE_MAJOR_VERSION >= 16
        v8::Local<v8::Context> context = exports->GetCreationContext().ToLocalChecked();
    #else
        v8::Local<v8::Context> context = exports->CreationContext();
    #endif

    if (!libpostal_setup() || !libpostal_setup_parser()) {
        Nan::ThrowError("Could not load libpostal");
        return;
    }

    exports->Set(
        context,
        Nan::New("parse_address").ToLocalChecked(),
        Nan::New<v8::FunctionTemplate>(ParseAddress)->GetFunction(context).ToLocalChecked()
    );

    exports->Set(
        context,
        Nan::New("parseAddress").ToLocalChecked(),
        Nan::New<v8::FunctionTemplate>(ParseAddress)->GetFunction(context).ToLocalChecked()
    );

    #if NODE_MAJOR_VERSION >= 12
        node::Environment* env = node::GetCurrentEnvironment(Nan::GetCurrentContext());
        node::AtExit(env, cleanup, NULL);
    #else
        node::AtExit(cleanup);
    #endif
}

NODE_MODULE(parser, init)
