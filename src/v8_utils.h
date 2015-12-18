#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <string.h>
#include <node.h>

char *v8_to_c_string(v8::Local<v8::Value> value);

#endif