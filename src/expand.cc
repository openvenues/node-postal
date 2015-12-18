#include <libpostal/libpostal.h>
#include <node.h>
#include <string.h>

#include "v8_utils.h"

#define EXPAND_USAGE "Usage: expand_address(address, options)"

namespace libpostal {
    using v8::FunctionCallbackInfo;
    using v8::Isolate;
    using v8::Local;
    using v8::Exception;
    using v8::Object;
    using v8::Array;
    using v8::String;
    using v8::Number;
    using v8::Value;

    void ExpandAddress(const FunctionCallbackInfo<Value> &args) {
        Isolate *isolate = args.GetIsolate();

        if (args.Length() < 2) {
            isolate->ThrowException(Exception::TypeError(
                String::NewFromUtf8(isolate, EXPAND_USAGE)
            ));
            return;
        }

        if (!(args[0]->IsString()) || !(args[1]->IsObject())) {
            isolate->ThrowException(Exception::TypeError(
                String::NewFromUtf8(isolate, EXPAND_USAGE)
            ));
            return;
        }


        char *address = v8_to_c_string(args[0]);
        if (address == NULL) {
            isolate->ThrowException(Exception::TypeError(
                String::NewFromUtf8(isolate, "Could not convert first arg to string")
            ));
            return;
        }

        normalize_options_t options = LIBPOSTAL_DEFAULT_OPTIONS;

        Local<Object> props = args[1]->ToObject();

        Local<Array> prop_names = props->GetOwnPropertyNames();

        char **languages = NULL;
        size_t num_languages = 0;

        uint64_t i, j;


        for (i = 0; i < prop_names->Length(); i++) {
            Local<Value> key = prop_names->Get(i);

            if (key->IsString()) {
                v8::String::Utf8Value utf8_key(key);
                char *key_string = *utf8_key;

                Local<Value> value = props->Get(key);
                if (strcmp(key_string, "languages") == 0) {
                    if (value->IsArray()) {
                        Local<Array> langs_value = Local<Array>::Cast(value);
                        size_t len = langs_value->Length() * sizeof(char *);
                        languages = (char **)malloc(len);
                        
                        for (j = 0; j < langs_value->Length(); j++) {
                            char *lang = v8_to_c_string(langs_value->Get(j));
                            if (lang != NULL && strlen(lang) < MAX_LANGUAGE_LEN) {
                                languages[num_languages++] = lang;
                            }
                        }
                    }

                } else if (strcmp(key_string, "address_components") == 0) {
                    if (value->IsNumber()) {
                        options.address_components = (uint16_t)value->Uint32Value();
                    }
                } else if (strcmp(key_string, "latin_ascii") == 0) {
                    options.latin_ascii = value->BooleanValue();
                } else if (strcmp(key_string, "transliterate") == 0) {
                    options.transliterate = value->BooleanValue();
                } else if (strcmp(key_string, "strip_accents") == 0) {
                    options.strip_accents = value->BooleanValue();
                } else if (strcmp(key_string, "decompose") == 0) {
                    options.decompose = value->BooleanValue();
                } else if (strcmp(key_string, "lowercase") == 0) {
                    options.lowercase = value->BooleanValue();
                } else if (strcmp(key_string, "trim_string") == 0) {
                    options.trim_string = value->BooleanValue();
                } else if (strcmp(key_string, "replace_word_hyphens") == 0) {
                    options.replace_word_hyphens = value->BooleanValue();
                } else if (strcmp(key_string, "delete_word_hyphens") == 0) {
                    options.delete_word_hyphens = value->BooleanValue();
                } else if (strcmp(key_string, "replace_numeric_hyphens") == 0) {
                    options.replace_numeric_hyphens = value->BooleanValue();
                } else if (strcmp(key_string, "delete_numeric_hyphens") == 0) {
                    options.delete_numeric_hyphens = value->BooleanValue();
                } else if (strcmp(key_string, "split_alpha_from_numeric") == 0) {
                    options.split_alpha_from_numeric = value->BooleanValue();
                } else if (strcmp(key_string, "delete_final_periods") == 0) {
                    options.delete_final_periods = value->BooleanValue();
                } else if (strcmp(key_string, "delete_acronym_periods") == 0) {
                    options.delete_acronym_periods = value->BooleanValue();
                } else if (strcmp(key_string, "drop_english_possessives") == 0) {
                    options.drop_english_possessives = value->BooleanValue();
                } else if (strcmp(key_string, "delete_apostrophes") == 0) {
                    options.delete_apostrophes = value->BooleanValue();
                } else if (strcmp(key_string, "expand_numex") == 0) {
                    options.expand_numex = value->BooleanValue();
                } else if (strcmp(key_string, "roman_numerals") == 0) {
                    options.roman_numerals = value->BooleanValue();
                }
            }
        }

        if (languages == NULL) {
            isolate->ThrowException(Exception::TypeError(
                String::NewFromUtf8(isolate, ".languages is required")
            ));
            return;
        } else if (num_languages == 0) {
            isolate->ThrowException(Exception::TypeError(
                String::NewFromUtf8(isolate, ".languages must be an array of language codes as strings")
            ));
            free(languages);
            return;
        }

        options.languages = languages;

        size_t num_expansions = 0;

        char **expansions = expand_address(address, options, &num_expansions);

        free(address);
        for (i = 0; i < num_languages; i++) {
            free(languages[i]);
        }
        free(languages);

        Local<Array> ret = Array::New(isolate, num_expansions);

        for (i = 0; i < num_expansions; i++) {
            ret->Set(i, String::NewFromUtf8(isolate, expansions[i]));
            free(expansions[i]);
        }
        free(expansions);

        args.GetReturnValue().Set(ret);
    }

    static void cleanup(void*) {
        libpostal_teardown();
    }

    void init(Local<Object> exports) {
        Isolate* isolate = exports->GetIsolate();
        if (!libpostal_setup()) {
            isolate->ThrowException(Exception::Error(
                String::NewFromUtf8(isolate, "Could not load libpostal")
            ));
            return;
        }

        NODE_SET_METHOD(exports, "expand_address", ExpandAddress);

        exports->Set(String::NewFromUtf8(isolate, "ADDRESS_NONE"), Number::New(isolate, ADDRESS_NONE));
        exports->Set(String::NewFromUtf8(isolate, "ADDRESS_ANY"), Number::New(isolate, ADDRESS_ANY));
        exports->Set(String::NewFromUtf8(isolate, "ADDRESS_NAME"), Number::New(isolate, ADDRESS_NAME));
        exports->Set(String::NewFromUtf8(isolate, "ADDRESS_HOUSE_NUMBER"), Number::New(isolate, ADDRESS_HOUSE_NUMBER));
        exports->Set(String::NewFromUtf8(isolate, "ADDRESS_STREET"), Number::New(isolate, ADDRESS_STREET));
        exports->Set(String::NewFromUtf8(isolate, "ADDRESS_UNIT"), Number::New(isolate, ADDRESS_UNIT));
        exports->Set(String::NewFromUtf8(isolate, "ADDRESS_LOCALITY"), Number::New(isolate, ADDRESS_LOCALITY));
        exports->Set(String::NewFromUtf8(isolate, "ADDRESS_ADMIN1"), Number::New(isolate, ADDRESS_ADMIN1));
        exports->Set(String::NewFromUtf8(isolate, "ADDRESS_ADMIN2"), Number::New(isolate, ADDRESS_ADMIN2));
        exports->Set(String::NewFromUtf8(isolate, "ADDRESS_ADMIN3"), Number::New(isolate, ADDRESS_ADMIN3));
        exports->Set(String::NewFromUtf8(isolate, "ADDRESS_ADMIN4"), Number::New(isolate, ADDRESS_ADMIN4));
        exports->Set(String::NewFromUtf8(isolate, "ADDRESS_ADMIN_OTHER"), Number::New(isolate, ADDRESS_ADMIN_OTHER));
        exports->Set(String::NewFromUtf8(isolate, "ADDRESS_COUNTRY"), Number::New(isolate, ADDRESS_COUNTRY));
        exports->Set(String::NewFromUtf8(isolate, "ADDRESS_POSTAL_CODE"), Number::New(isolate, ADDRESS_POSTAL_CODE));
        exports->Set(String::NewFromUtf8(isolate, "ADDRESS_NEIGHBORHOOD"), Number::New(isolate, ADDRESS_NEIGHBORHOOD));
        exports->Set(String::NewFromUtf8(isolate, "ADDRESS_ALL"), Number::New(isolate, ADDRESS_ALL));

        node::AtExit(cleanup);
    }

    NODE_MODULE(expand, init)

}