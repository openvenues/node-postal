#include "v8_utils.h"

char *v8_to_c_string(v8::Local<v8::Value> value) {
    if (value->IsString()) {
        v8::String::Utf8Value utf8_value(value);
        char *str = (char *)malloc(utf8_value.length() + 1);
        if (str == NULL) return NULL;
        strcpy(str, *utf8_value);
        return str;
    }

    return NULL;
}