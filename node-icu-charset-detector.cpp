#include <node.h>
#include <nan.h>

#include <node_buffer.h>
#include <node_object_wrap.h>

#include <unicode/ucsdet.h>

#include <cstring>
#include <iostream>

class CharsetMatch : public node::ObjectWrap
{
public:

    static void
    Initialize(const v8::Handle<v8::Object> target) {
        NanScope();

        v8::Local<v8::FunctionTemplate> constructorTemplate
            = NanNew<v8::FunctionTemplate>(CharsetMatch::New);

        constructorTemplate->InstanceTemplate()->SetInternalFieldCount(1);

        // setup methods
        NODE_SET_PROTOTYPE_METHOD(constructorTemplate, "getName", CharsetMatch::GetName);
        NODE_SET_PROTOTYPE_METHOD(constructorTemplate, "getLanguage", CharsetMatch::GetLanguage);
        NODE_SET_PROTOTYPE_METHOD(constructorTemplate, "getConfidence", CharsetMatch::GetConfidence);

        // export class
        target->Set(NanNew<v8::String>("CharsetMatch"), constructorTemplate->GetFunction());
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
        NanScope();

        if (args.Length() < 1)
            NanThrowError("Not enough arguments");

        v8::Handle<v8::Object> buffer = args[0]->ToObject();

        if (!node::Buffer::HasInstance(buffer))
            NanThrowTypeError("Expected Buffer for the argument");

        try {
            CharsetMatch::FromBuffer(buffer)->Wrap(args.This()); // under GC
        } catch (const char* errorMessage) {
            NanThrowError(errorMessage);
        }

        NanReturnValue(args.This());
    }

private:
    static
    NAN_METHOD(GetName) {
        NanScope();
        CharsetMatch* self = node::ObjectWrap::Unwrap<CharsetMatch>(args.This());
        if (!self->charsetMatch_) {
            NanReturnNull();
        }
        UErrorCode icuError = U_ZERO_ERROR;
        const char* detectedCharsetName = ucsdet_getName(self->charsetMatch_, &icuError);
        NanReturnValue(NanNew<v8::String>(detectedCharsetName, strlen(detectedCharsetName)));
    }

    static
    NAN_METHOD(GetLanguage) {
        NanScope();
        CharsetMatch* self = node::ObjectWrap::Unwrap<CharsetMatch>(args.This());
        if (!self->charsetMatch_) {
            NanReturnNull();
        }
        UErrorCode icuError = U_ZERO_ERROR;
        const char* detectedLanguage = ucsdet_getLanguage(self->charsetMatch_, &icuError);
        NanReturnValue(NanNew<v8::String>(detectedLanguage, strlen(detectedLanguage)));
    }

    static
    NAN_METHOD(GetConfidence) {
        NanScope();
        CharsetMatch* self = node::ObjectWrap::Unwrap<CharsetMatch>(args.This());
        if (!self->charsetMatch_) {
            NanReturnNull();
        }
        UErrorCode icuError = U_ZERO_ERROR;
        int32_t detectionConfidence = ucsdet_getConfidence(self->charsetMatch_, &icuError);
        NanReturnValue(NanNew<v8::Number>(static_cast<double>(detectionConfidence)));
    }

    UCharsetDetector* charsetDetector_;
    const UCharsetMatch* charsetMatch_;
};

extern "C"
void
init(v8::Handle<v8::Object> target)
{
    CharsetMatch::Initialize(target);
}

NODE_MODULE(node_icu_charset_detector, init)
