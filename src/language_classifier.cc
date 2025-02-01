#include <libpostal/libpostal.h>
#include <nan.h>
#include <string.h>

void LanguageClassifier(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate *isolate = info.GetIsolate();

    if (info.Length() < 1) {
        Nan::ThrowTypeError("Usage: language_classifier(text)");
        return;
    }

    if (!info[0]->IsString()) {
        Nan::ThrowTypeError("First argument must be a string");
        return;
    }

    Nan::Utf8String text_utf8(info[0]);
    char *text = *text_utf8;

    if (text == NULL) {
        Nan::ThrowTypeError("Could not convert first argument to string");
        return;
    }

    libpostal_language_classifier_response_t *response = libpostal_classify_language(text);

    if (response != NULL) {
        v8::Local<v8::Array> lang_array = Nan::New<v8::Array>(response->num_languages);

        for (size_t i = 0; i < response->num_languages; ++i) {
            const char *language = response->languages[i];
            const double probability = response->probs[i];
            
            v8::Local<v8::Object> lang_obj = Nan::New<v8::Object>();
            Nan::Set(lang_obj, Nan::New("language").ToLocalChecked(), Nan::New(language).ToLocalChecked());
            Nan::Set(lang_obj, Nan::New("probability").ToLocalChecked(), Nan::New(probability));
            
            Nan::Set(lang_array, i, lang_obj);
        }
        libpostal_language_classifier_response_destroy(response);
        info.GetReturnValue().Set(lang_array);
    }
}

void cleanup(void*) {
    libpostal_teardown();
    libpostal_teardown_language_classifier();
}

void init(v8::Local<v8::Object> exports) {
    if (!libpostal_setup() || !libpostal_setup_language_classifier()) {
        Nan::ThrowError("Could not load libpostal");
        return;
    }

    // Check Node.js version
    #if NODE_MAJOR_VERSION >= 16
        v8::Local<v8::Context> context = exports->GetCreationContext().ToLocalChecked();
    #else
        v8::Local<v8::Context> context = exports->CreationContext();
    #endif

    exports->Set(
        context, 
        Nan::New("language_classifier").ToLocalChecked(),
        Nan::New<v8::FunctionTemplate>(LanguageClassifier)->GetFunction(context).ToLocalChecked()
    );

    #if NODE_MAJOR_VERSION >= 12
        node::Environment* env = node::GetCurrentEnvironment(Nan::GetCurrentContext());
        node::AtExit(env, cleanup, NULL);
    #else
        node::AtExit(cleanup);
    #endif
}

NODE_MODULE(language_classifier, init)
