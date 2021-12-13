#include "IR/IR.h"
#include "Util/Util.h"

void IntegerType::Init(Napi::Env env, Napi::Object &exports) {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "IntegerType", {
            StaticMethod("get", &IntegerType::get),
            InstanceMethod("isStructTy", &IntegerType::isStructTy),
            InstanceMethod("isIntegerTy", &IntegerType::isIntegerTy),
            InstanceMethod("isVoidTy", &IntegerType::isVoidTy),
            InstanceMethod("getTypeID", &IntegerType::getTypeID)
    });
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
    Inherit(env, constructor.Value(), Type::constructor.Value());
    exports.Set("IntegerType", func);
}

Napi::Object IntegerType::New(Napi::Env env, llvm::IntegerType *type) {
    return constructor.New({Napi::External<llvm::IntegerType>::New(env, type)});
}

bool IntegerType::IsClassOf(const Napi::Value &value) {
    return value.As<Napi::Object>().InstanceOf(constructor.Value());
}

llvm::IntegerType *IntegerType::Extract(const Napi::Value &value) {
    if (value.IsNull()) {
        return nullptr;
    }
    return Unwrap(value.As<Napi::Object>())->getLLVMPrimitive();
}

IntegerType::IntegerType(const Napi::CallbackInfo &info) : ObjectWrap(info) {
    Napi::Env env = info.Env();
    if (!info.IsConstructCall() || info.Length() == 0 || !info[0].IsExternal()) {
        throw Napi::TypeError::New(env, ErrMsg::Class::IntegerType::constructor);
    }
    auto external = info[0].As<Napi::External<llvm::IntegerType>>();
    integerType = external.Data();
}

llvm::IntegerType *IntegerType::getLLVMPrimitive() {
    return integerType;
}

Napi::Value IntegerType::get(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    if (info.Length() < 2 || !LLVMContext::IsClassOf(info[0]) || !info[1].IsNumber()) {
        throw Napi::TypeError::New(env, ErrMsg::Class::IntegerType::get);
    }
    llvm::LLVMContext &context = LLVMContext::Extract(info[0]);
    unsigned numBits = info[1].As<Napi::Number>();
    llvm::IntegerType *integerType = llvm::IntegerType::get(context, numBits);
    return IntegerType::New(env, integerType);
}

Napi::Value IntegerType::isStructTy(const Napi::CallbackInfo &info) {
    return Napi::Boolean::New(info.Env(), integerType->isStructTy());
}

Napi::Value IntegerType::isIntegerTy(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    if (info.Length() == 0 || !info[0].IsNumber()) {
        throw Napi::TypeError::New(env, ErrMsg::Class::IntegerType::isIntegerTy);
    }
    bool result = info.Length() == 0 ? integerType->isIntegerTy() : integerType->isIntegerTy(info[0].As<Napi::Number>());
    return Napi::Boolean::New(env, result);
}

Napi::Value IntegerType::isVoidTy(const Napi::CallbackInfo &info) {
    return Napi::Boolean::New(info.Env(), integerType->isVoidTy());
}

Napi::Value IntegerType::getTypeID(const Napi::CallbackInfo &info) {
    return Napi::Number::New(info.Env(), integerType->getTypeID());
}
