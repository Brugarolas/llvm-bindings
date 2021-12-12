#include "IR/IR.h"
#include "Util/Util.h"

void MDNode::Init(Napi::Env env, Napi::Object &exports) {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "MDNode", {
    });
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
    Inherit(env, constructor.Value(), Metadata::constructor.Value());
    exports.Set("MDNode", func);
}

Napi::Value MDNode::New(Napi::Env env, llvm::MDNode *node) {
    if (llvm::isa<llvm::DILocation>(node)) {
        return DILocation::New(env, llvm::cast<llvm::DILocation>(node));
    } else if (llvm::isa<llvm::DINode>(node)) {
        return DINode::New(env, llvm::cast<llvm::DINode>(node));
    }
    return constructor.New({Napi::External<llvm::MDNode>::New(env, node)});
}

bool MDNode::IsClassOf(const Napi::Value &value) {
    return value.IsNull() || value.As<Napi::Object>().InstanceOf(constructor.Value());
}

llvm::MDNode *MDNode::Extract(const Napi::Value &value) {
    if (value.IsNull()) {
        return nullptr;
    }
    return Unwrap(value.As<Napi::Object>())->getLLVMPrimitive();
}

MDNode::MDNode(const Napi::CallbackInfo &info) : ObjectWrap(info) {
    Napi::Env env = info.Env();
    if (!info.IsConstructCall() || info.Length() == 0 || !info[0].IsExternal()) {
        throw Napi::TypeError::New(env, ErrMsg::Class::MDNode::constructor);
    }
    auto external = info[0].As<Napi::External<llvm::MDNode>>();
    node = external.Data();
}

llvm::MDNode *MDNode::getLLVMPrimitive() {
    return node;
}