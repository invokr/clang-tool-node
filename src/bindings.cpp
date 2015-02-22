/**
* @file bindings.cpp
* @author Robin Dietrich <me (at) invokr (dot) org>
* @version 1.0
*
* @par License
*   clang-autocomplete
*   Copyright 2015 Robin Dietrich
*
*   Licensed under the Apache License, Version 2.0 (the "License");
*   you may not use this file except in compliance with the License.
*   You may obtain a copy of the License at
*
*   http://www.apache.org/licenses/LICENSE-2.0
*
*   Unless required by applicable law or agreed to in writing, software
*   distributed under the License is distributed on an "AS IS" BASIS,
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*   See the License for the specific language governing permissions and
*   limitations under the License. *
*/

#include <vector>

#include "clang/clang_tool.hpp"
#include "bindings.hpp"

/// local utility functions
namespace {
    /*
    template <typename T>
    std::vector<T> toVector(Local<Value> v) {
        if (!v->IsArray())
            return {};

        Local<Array> arr = Local<Array>::Cast(v);
        std::vector<T> ret(arr->Length());

        for (uint32_t i = 0; i < arr->Length(); ++i) {
            ret[i] = arr->Get(i);
        }

        return ret;
    }*/
}

/// persistance between calls
Persistent<FunctionTemplate> node_tool::constructor;

/// constructor
node_tool::node_tool() : ObjectWrap() {}

/// destructor
node_tool::~node_tool() {}

/// new
Handle<Value> node_tool::New(const Arguments &args) {
    HandleScope scope;

    node_tool *ntool = new node_tool();
    ntool->Wrap(args.This());

    return args.This();
}

/// initializes the njs obj
void node_tool::Init(Handle<Object> target) {
    HandleScope scope;

    // Wrap new and make it persistend
    Local<FunctionTemplate> local_function_template = FunctionTemplate::New(New);
    node_tool::constructor = Persistent<FunctionTemplate>::New(local_function_template);

    node_tool::constructor->InstanceTemplate()->SetInternalFieldCount(1);
    node_tool::constructor->SetClassName(String::NewSymbol("object"));

    // Make our methods available to Node
    NODE_SET_PROTOTYPE_METHOD(node_tool::constructor, "setArgs",             setArgs);
    NODE_SET_PROTOTYPE_METHOD(node_tool::constructor, "indexTouch",          indexTouch);
    NODE_SET_PROTOTYPE_METHOD(node_tool::constructor, "indexStatus",         indexStatus);
    NODE_SET_PROTOTYPE_METHOD(node_tool::constructor, "indexClear",          indexClear);
    NODE_SET_PROTOTYPE_METHOD(node_tool::constructor, "fileOutline",         fileOutline);
    NODE_SET_PROTOTYPE_METHOD(node_tool::constructor, "fileDiagnose",        fileDiagnose);
    NODE_SET_PROTOTYPE_METHOD(node_tool::constructor, "cursorCandidatesAt",  cursorCandidatesAt);
    NODE_SET_PROTOTYPE_METHOD(node_tool::constructor, "cursorTypeAt",        cursorTypeAt);
    NODE_SET_PROTOTYPE_METHOD(node_tool::constructor, "cursorDeclarationAt", cursorDeclarationAt);
    NODE_SET_PROTOTYPE_METHOD(node_tool::constructor, "cursorDefinitionAt",  cursorDefinitionAt);

    target->Set(String::NewSymbol("object"), node_tool::constructor->GetFunction());
}

/// set arguments
Handle<Value> node_tool::setArgs(const Arguments& args) {
    node_tool* instance = node::ObjectWrap::Unwrap<node_tool>(args.This());
    HandleScope scope;

    // make sure the syntax is correct
    if (args.Length() != 1 || args[0]->IsArray())
        return ThrowException(
            Exception::SyntaxError(String::New("Usage: setArgs(Array)"))
        );

    // get arguments and relay to node_tool
    Local<Array> arr = Local<Array>::Cast(args[0]);
    std::vector<const char*> args2;

    for (std::size_t i = 0; i < arr->Length(); ++i) {
        String::Utf8Value str(arr->Get(i));
        args2.push_back( *str );
    }

    instance->tool.arguments_set(&args2[0], args2.size());
    return scope.Close(Undefined());
}

/// add / update file
Handle<Value> node_tool::indexTouch(const Arguments& args) {
    node_tool* instance = node::ObjectWrap::Unwrap<node_tool>(args.This());
    HandleScope scope;

    return scope.Close(Undefined());
}

/// memory usage
Handle<Value> node_tool::indexStatus(const Arguments& args) {
    node_tool* instance = node::ObjectWrap::Unwrap<node_tool>(args.This());
    HandleScope scope;

    return scope.Close(Undefined());
}

// clear cache
Handle<Value> node_tool::indexClear(const Arguments& args) {
    node_tool* instance = node::ObjectWrap::Unwrap<node_tool>(args.This());
    HandleScope scope;

    return scope.Close(Undefined());
}

/// get outline
Handle<Value> node_tool::fileOutline(const Arguments& args) {
    node_tool* instance = node::ObjectWrap::Unwrap<node_tool>(args.This());
    HandleScope scope;

    return scope.Close(Undefined());
}

/// get file diagnostics
Handle<Value> node_tool::fileDiagnose(const Arguments& args) {
    node_tool* instance = node::ObjectWrap::Unwrap<node_tool>(args.This());
    HandleScope scope;

    return scope.Close(Undefined());
}

/// code completion
Handle<Value> node_tool::cursorCandidatesAt(const Arguments& args) {
    node_tool* instance = node::ObjectWrap::Unwrap<node_tool>(args.This());
    HandleScope scope;

    return scope.Close(Undefined());
}

/// get type at
Handle<Value> node_tool::cursorTypeAt(const Arguments& args) {
    node_tool* instance = node::ObjectWrap::Unwrap<node_tool>(args.This());
    HandleScope scope;

    return scope.Close(Undefined());
}

/// get decleration for pos
Handle<Value> node_tool::cursorDeclarationAt(const Arguments& args) {
    node_tool* instance = node::ObjectWrap::Unwrap<node_tool>(args.This());
    HandleScope scope;

    return scope.Close(Undefined());
}

/// get definition for pos
Handle<Value> node_tool::cursorDefinitionAt(const Arguments& args) {
    node_tool* instance = node::ObjectWrap::Unwrap<node_tool>(args.This());
    HandleScope scope;

    return scope.Close(Undefined());
}

/// module initialization
void initAll(Handle<Object> exports) {
    node_tool::Init(exports);
}

/// export node module
NODE_MODULE(node_tool, initAll);