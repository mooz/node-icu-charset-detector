#include <node.h>
#include <nan.h>

#include <node_buffer.h>
#include <node_object_wrap.h>

#include <unicode/ucsdet.h>

#include <cstring>
#include <iostream>

class CharsetMatch : public Nan::ObjectWrap
{
public:

    static void
    RegisterClass(const v8::Handle<v8::Object> target) {
        Nan::HandleScope scope;
        const char* ClassName = "CharsetMatch";

        v8::Local<v8::FunctionTemplate> constructorTemplate
            = Nan::New<v8::FunctionTemplate>(CharsetMatch::New);

        constructorTemplate->SetClassName(Nan::New(ClassName).ToLocalChecked());
        constructorTemplate->InstanceTemplate()->SetInternalFieldCount(1);

        // setup methods
        Nan::SetPrototypeMethod(constructorTemplate, "getName", CharsetMatch::GetName);
        Nan::SetPrototypeMethod(constructorTemplate, "getLanguage", CharsetMatch::GetLanguage);
        Nan::SetPrototypeMethod(constructorTemplate, "getConfidence", CharsetMatch::GetConfidence);

        // export class
        target->Set(Nan::New(ClassName).ToLocalChecked(), constructorTemplate->GetFunction());
    }

    CharsetMatch(const char* bufferData, size_t bufferLength) {
        UErrorCode icuError = U_ZERO_ERROR;

        charsetDetector_ = ucsdet_open(&icuError);
        if (U_FAILURE(icuError))
            throw "Failed to open detector";

        // send text
        ucsdet_setText(charsetDetector_, bufferData, bufferLength, &icuError);
        if (U_FAILURE(icuError))
            throw "Failed to set text";

        // detect language
        charsetMatch_ = ucsdet_detect(charsetDetector_, &icuError);
        if (U_FAILURE(icuError))
            throw "Failed to detect charset";
    }

    ~CharsetMatch () {
        ucsdet_close(charsetDetector_);
    }

    // Internal API
    static CharsetMatch*
    FromBuffer(v8::Handle<v8::Object> bufferObject) {
        return new CharsetMatch(node::Buffer::Data(bufferObject),
                                node::Buffer::Length(bufferObject));
    }

    // JS Constructor
    static
    NAN_METHOD(New) {

        if (info.Length() < 1)
            Nan::ThrowError("Not enough arguments");

        v8::Handle<v8::Object> buffer = info[0]->ToObject();

        if (!node::Buffer::HasInstance(buffer))
            Nan::ThrowTypeError("Expected Buffer for the argument");

        try {
            CharsetMatch::FromBuffer(buffer)->Wrap(info.This()); // under GC
        } catch (const char* errorMessage) {
            Nan::ThrowError(errorMessage);
        }

        info.GetReturnValue().Set(info.This());
    }

private:
    static
    NAN_METHOD(GetName) {
        CharsetMatch* self = Nan::ObjectWrap::Unwrap<CharsetMatch>(info.This());
        if (!self->charsetMatch_) {
            info.GetReturnValue().SetNull();
        }
        UErrorCode icuError = U_ZERO_ERROR;
        const char* detectedCharsetName = ucsdet_getName(self->charsetMatch_, &icuError);
        info.GetReturnValue().Set(Nan::New<v8::String>(detectedCharsetName, strlen(detectedCharsetName)).ToLocalChecked());
    }

    static
    NAN_METHOD(GetLanguage) {
        CharsetMatch* self = Nan::ObjectWrap::Unwrap<CharsetMatch>(info.This());
        if (!self->charsetMatch_) {
            info.GetReturnValue().SetNull();
        }
        UErrorCode icuError = U_ZERO_ERROR;
        const char* detectedLanguage = ucsdet_getLanguage(self->charsetMatch_, &icuError);
        info.GetReturnValue().Set(Nan::New<v8::String>(detectedLanguage, strlen(detectedLanguage)).ToLocalChecked());
    }

    static
    NAN_METHOD(GetConfidence) {
        CharsetMatch* self = Nan::ObjectWrap::Unwrap<CharsetMatch>(info.This());
        if (!self->charsetMatch_) {
            info.GetReturnValue().SetNull();
        }
        UErrorCode icuError = U_ZERO_ERROR;
        int32_t detectionConfidence = ucsdet_getConfidence(self->charsetMatch_, &icuError);
        info.GetReturnValue().Set(Nan::New(static_cast<double>(detectionConfidence)));
    }

    UCharsetDetector* charsetDetector_;
    const UCharsetMatch* charsetMatch_;
};

NAN_MODULE_INIT(RegisterModule) {
    CharsetMatch::RegisterClass(target);
}

NODE_MODULE(node_icu_charset_detector, RegisterModule);
