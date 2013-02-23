#include <node.h>
#include <node_buffer.h>
#include <node_object_wrap.h>

#include <unicode/ucsdet.h>

#include <cstring>
#include <iostream>

#define EXCEPTION(type, message) \
    ThrowException(v8::Exception::type(v8::String::New(message)))

class CharsetMatch : public node::ObjectWrap
{
public:

    static void
    Initialize(const v8::Handle<v8::Object> target) {
        v8::HandleScope scope;

        v8::Local<v8::FunctionTemplate> constructorTemplate
            = v8::FunctionTemplate::New(CharsetMatch::New);

        constructorTemplate->InstanceTemplate()->SetInternalFieldCount(1);

        // setup methods
        NODE_SET_PROTOTYPE_METHOD(constructorTemplate, "getName", CharsetMatch::GetName);
        NODE_SET_PROTOTYPE_METHOD(constructorTemplate, "getLanguage", CharsetMatch::GetLanguage);
        NODE_SET_PROTOTYPE_METHOD(constructorTemplate, "getConfidence", CharsetMatch::GetConfidence);

        // export class
        target->Set(v8::String::NewSymbol("CharsetMatch"), constructorTemplate->GetFunction());
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
            throw "Failed to detect cahrset";
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
    static v8::Handle<v8::Value>
    New(const v8::Arguments& args) {
        if (args.Length() < 1 || !node::Buffer::HasInstance(args[0]))
            return EXCEPTION(TypeError, "Expected Buffer for the argument");

        try {
            CharsetMatch::FromBuffer(args[0]->ToObject())->Wrap(args.This()); // under GC
        } catch (const char* errorMessage) {
            return EXCEPTION(Error, errorMessage);
        }

        return args.This();
    }

private:
    static v8::Handle<v8::Value>
    GetName(const v8::Arguments& args) {
        v8::HandleScope scope;
        CharsetMatch* self = node::ObjectWrap::Unwrap<CharsetMatch>(args.This());
        UErrorCode icuError = U_ZERO_ERROR;
        const char* detectedCharsetName = ucsdet_getName(self->charsetMatch_, &icuError);
        return scope.Close(v8::String::New(detectedCharsetName, strlen(detectedCharsetName)));
    }

    static v8::Handle<v8::Value>
    GetLanguage(const v8::Arguments& args) {
        v8::HandleScope scope;
        CharsetMatch* self = node::ObjectWrap::Unwrap<CharsetMatch>(args.This());
        UErrorCode icuError = U_ZERO_ERROR;
        const char* detectedLanguage = ucsdet_getLanguage(self->charsetMatch_, &icuError);
        return scope.Close(v8::String::New(detectedLanguage, strlen(detectedLanguage)));
    }

    static v8::Handle<v8::Value>
    GetConfidence(const v8::Arguments& args) {
        v8::HandleScope scope;
        CharsetMatch* self = node::ObjectWrap::Unwrap<CharsetMatch>(args.This());
        UErrorCode icuError = U_ZERO_ERROR;
        int32_t detectionConfidence = ucsdet_getConfidence(self->charsetMatch_, &icuError);
        return scope.Close(v8::Number::New(static_cast<double>(detectionConfidence)));
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

#undef EXCEPTION
