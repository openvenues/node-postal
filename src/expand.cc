#include <libpostal/libpostal.h>
#include <nan.h>
#include <string.h>

#define EXPAND_USAGE "Usage: expand_address(address, options)"

void ExpandAddress(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate *isolate = info.GetIsolate();
	v8::Local<v8::Context> context = isolate->GetCurrentContext();

    if (info.Length() < 1) {
        Nan::ThrowTypeError(EXPAND_USAGE);
        return;
    }

    if (!(info[0]->IsString()) || (info.Length() > 1 && !(info[1]->IsObject()))) {
        Nan::ThrowTypeError(EXPAND_USAGE);
        return;
    }

    Nan::Utf8String address_utf8(info[0]);
    char *address = *address_utf8;

    if (address == NULL) {
        Nan::ThrowTypeError("Could not convert first arg to string");
        return;
    }

    libpostal_normalize_options_t options = libpostal_get_default_options();

    char **languages = NULL;
    size_t num_languages = 0;

    uint64_t i, j;

    if (info.Length() > 1) {
        v8::Local<v8::Object> props = info[1]->ToObject(context).ToLocalChecked();

        v8::Local<v8::Array> prop_names = Nan::GetPropertyNames(props).ToLocalChecked();

        for (i = 0; i < prop_names->Length(); i++) {
            v8::Local<v8::Value> key = prop_names->Get(context, i).ToLocalChecked();

            if (key->IsString()) {
                Nan::Utf8String utf8_key(key);
                char *key_string = *utf8_key;

                v8::Local<v8::Value> value = Nan::Get(props, key).ToLocalChecked();
                if (strcmp(key_string, "languages") == 0) {
                    if (value->IsArray()) {
                        v8::Local<v8::Array> langs_value = v8::Local<v8::Array>::Cast(value);
                        size_t len = langs_value->Length() * sizeof(char *);
                        languages = (char **)malloc(len);
                        
                        for (j = 0; j < langs_value->Length(); j++) {
                            Nan::Utf8String lang_utf8(langs_value->Get(context, j).ToLocalChecked());
                            char *lang = *lang_utf8;
                            if (lang != NULL && strlen(lang) < LIBPOSTAL_MAX_LANGUAGE_LEN) {
                                languages[num_languages++] = lang;
                            }
                        }
                    }

                } else if (strcmp(key_string, "address_components") == 0) {
                    if (value->IsNumber()) {
                        options.address_components = (uint16_t) Nan::To<uint32_t>(value).FromJust();
                    }
                } else if (strcmp(key_string, "latin_ascii") == 0) {
                    options.latin_ascii = Nan::To<bool>(value).FromJust();
                } else if (strcmp(key_string, "transliterate") == 0) {
                    options.transliterate = Nan::To<bool>(value).FromJust();
                } else if (strcmp(key_string, "strip_accents") == 0) {
                    options.strip_accents = Nan::To<bool>(value).FromJust();
                } else if (strcmp(key_string, "decompose") == 0) {
                    options.decompose = Nan::To<bool>(value).FromJust();
                } else if (strcmp(key_string, "lowercase") == 0) {
                    options.lowercase = Nan::To<bool>(value).FromJust();
                } else if (strcmp(key_string, "trim_string") == 0) {
                    options.trim_string = Nan::To<bool>(value).FromJust();
                } else if (strcmp(key_string, "replace_word_hyphens") == 0) {
                    options.replace_word_hyphens = Nan::To<bool>(value).FromJust();
                } else if (strcmp(key_string, "delete_word_hyphens") == 0) {
                    options.delete_word_hyphens = Nan::To<bool>(value).FromJust();
                } else if (strcmp(key_string, "replace_numeric_hyphens") == 0) {
                    options.replace_numeric_hyphens = Nan::To<bool>(value).FromJust();
                } else if (strcmp(key_string, "delete_numeric_hyphens") == 0) {
                    options.delete_numeric_hyphens = Nan::To<bool>(value).FromJust();
                } else if (strcmp(key_string, "split_alpha_from_numeric") == 0) {
                    options.split_alpha_from_numeric = Nan::To<bool>(value).FromJust();
                } else if (strcmp(key_string, "delete_final_periods") == 0) {
                    options.delete_final_periods = Nan::To<bool>(value).FromJust();
                } else if (strcmp(key_string, "delete_acronym_periods") == 0) {
                    options.delete_acronym_periods = Nan::To<bool>(value).FromJust();
                } else if (strcmp(key_string, "drop_english_possessives") == 0) {
                    options.drop_english_possessives = Nan::To<bool>(value).FromJust();
                } else if (strcmp(key_string, "delete_apostrophes") == 0) {
                    options.delete_apostrophes = Nan::To<bool>(value).FromJust();
                } else if (strcmp(key_string, "expand_numex") == 0) {
                    options.expand_numex = Nan::To<bool>(value).FromJust();
                } else if (strcmp(key_string, "roman_numerals") == 0) {
                    options.roman_numerals = Nan::To<bool>(value).FromJust();
                }
            }
        }
    }

    options.languages = languages;
    options.num_languages = num_languages;

    size_t num_expansions = 0;

    char **expansions = libpostal_expand_address(address, options, &num_expansions);

    if (languages != NULL) {
        free(languages);
    }

    v8::Local<v8::Array> ret = Nan::New<v8::Array>(num_expansions);

    for (i = 0; i < num_expansions; i++) {
        v8::Local<v8::String> e = Nan::New(expansions[i]).ToLocalChecked();
        ret->Set(context, i, e);
        free(expansions[i]);
    }
    free(expansions);

    info.GetReturnValue().Set(ret);
}

static void cleanup(void*) {
    libpostal_teardown();
    libpostal_teardown_language_classifier();
}

void init(v8::Local<v8::Object> exports) {
    // Check Node.js version
    #if NODE_MAJOR_VERSION >= 16
        v8::Local<v8::Context> context = exports->GetCreationContext().ToLocalChecked();
    #else
        v8::Local<v8::Context> context = exports->CreationContext();
    #endif
    
    if (!libpostal_setup() || !libpostal_setup_language_classifier()) {
        Nan::ThrowError("Could not load libpostal");
        return;
    }

    exports->Set(
        context, 
        Nan::New("expand_address").ToLocalChecked(),
        Nan::New<v8::FunctionTemplate>(ExpandAddress)->GetFunction(context).ToLocalChecked()
    );

    exports->Set(
        context, 
        Nan::New("expandAddress").ToLocalChecked(),
        Nan::New<v8::FunctionTemplate>(ExpandAddress)->GetFunction(context).ToLocalChecked()
    );

    exports->Set(context, Nan::New("ADDRESS_NONE").ToLocalChecked(), Nan::New(LIBPOSTAL_ADDRESS_NONE));
    exports->Set(context, Nan::New("ADDRESS_ANY").ToLocalChecked(), Nan::New(LIBPOSTAL_ADDRESS_ANY));
    exports->Set(context, Nan::New("ADDRESS_NAME").ToLocalChecked(), Nan::New(LIBPOSTAL_ADDRESS_NAME));
    exports->Set(context, Nan::New("ADDRESS_HOUSE_NUMBER").ToLocalChecked(), Nan::New(LIBPOSTAL_ADDRESS_HOUSE_NUMBER));
    exports->Set(context, Nan::New("ADDRESS_STREET").ToLocalChecked(), Nan::New(LIBPOSTAL_ADDRESS_STREET));
    exports->Set(context, Nan::New("ADDRESS_UNIT").ToLocalChecked(), Nan::New(LIBPOSTAL_ADDRESS_UNIT));
    exports->Set(context, Nan::New("ADDRESS_LEVEL").ToLocalChecked(), Nan::New(LIBPOSTAL_ADDRESS_LEVEL));
    exports->Set(context, Nan::New("ADDRESS_STAIRCASE").ToLocalChecked(), Nan::New(LIBPOSTAL_ADDRESS_STAIRCASE));
    exports->Set(context, Nan::New("ADDRESS_ENTRANCE").ToLocalChecked(), Nan::New(LIBPOSTAL_ADDRESS_ENTRANCE));

    exports->Set(context, Nan::New("ADDRESS_CATEGORY").ToLocalChecked(), Nan::New(LIBPOSTAL_ADDRESS_CATEGORY));
    exports->Set(context, Nan::New("ADDRESS_NEAR").ToLocalChecked(), Nan::New(LIBPOSTAL_ADDRESS_NEAR));

    exports->Set(context, Nan::New("ADDRESS_TOPONYM").ToLocalChecked(), Nan::New(LIBPOSTAL_ADDRESS_TOPONYM));
    exports->Set(context, Nan::New("ADDRESS_POSTAL_CODE").ToLocalChecked(), Nan::New(LIBPOSTAL_ADDRESS_POSTAL_CODE));
    exports->Set(context, Nan::New("ADDRESS_PO_BOX").ToLocalChecked(), Nan::New(LIBPOSTAL_ADDRESS_PO_BOX));
    exports->Set(context, Nan::New("ADDRESS_ALL").ToLocalChecked(), Nan::New(LIBPOSTAL_ADDRESS_ALL));

    #if NODE_MAJOR_VERSION >= 12
        node::Environment* env = node::GetCurrentEnvironment(Nan::GetCurrentContext());
        node::AtExit(env, cleanup, NULL);
    #else
        node::AtExit(cleanup);
    #endif
}

NODE_MODULE(expand, init)
