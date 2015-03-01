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
*   limitations under the License.
*/

#include <vector>

#include "clang/clang_tool.hpp"
#include "bindings.hpp"

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
    NODE_SET_PROTOTYPE_METHOD(node_tool::constructor, "indexTouchUnsaved",   indexTouchUnsaved);
    NODE_SET_PROTOTYPE_METHOD(node_tool::constructor, "indexStatus",         indexStatus);
    NODE_SET_PROTOTYPE_METHOD(node_tool::constructor, "indexClear",          indexClear);
    NODE_SET_PROTOTYPE_METHOD(node_tool::constructor, "fileOutline",         fileOutline);
    NODE_SET_PROTOTYPE_METHOD(node_tool::constructor, "fileDiagnose",        fileDiagnose);
    NODE_SET_PROTOTYPE_METHOD(node_tool::constructor, "cursorCandidatesAt",  cursorCandidatesAt);
    NODE_SET_PROTOTYPE_METHOD(node_tool::constructor, "cursorTypeAt",        cursorTypeAt);
    NODE_SET_PROTOTYPE_METHOD(node_tool::constructor, "cursorDeclarationAt", cursorDeclarationAt);
    NODE_SET_PROTOTYPE_METHOD(node_tool::constructor, "cursorDefinitionAt",  cursorDefinitionAt);

    // Add constructor to our addon
    target->Set(String::NewSymbol("object"), node_tool::constructor->GetFunction(), ReadOnly);

    // Add all completion types to the addon
    target->Set(String::NewSymbol("namespace_t"),
        Number::New(static_cast<uint32_t>(clang::completion_type::namespace_t)),
        ReadOnly);

    target->Set(String::NewSymbol("class_t"),
        Number::New(static_cast<uint32_t>(clang::completion_type::class_t)),
        ReadOnly);

    target->Set(String::NewSymbol("attribute_t"),
        Number::New(static_cast<uint32_t>(clang::completion_type::attribute_t)),
        ReadOnly);

    target->Set(String::NewSymbol("method_t"),
        Number::New(static_cast<uint32_t>(clang::completion_type::method_t)),
        ReadOnly);

    target->Set(String::NewSymbol("parameter_t"),
        Number::New(static_cast<uint32_t>(clang::completion_type::parameter_t)),
        ReadOnly);

    target->Set(String::NewSymbol("struct_t"),
        Number::New(static_cast<uint32_t>(clang::completion_type::struct_t)),
        ReadOnly);

    target->Set(String::NewSymbol("function_t"),
        Number::New(static_cast<uint32_t>(clang::completion_type::function_t)),
        ReadOnly);

    target->Set(String::NewSymbol("enum_t"),
        Number::New(static_cast<uint32_t>(clang::completion_type::enum_t)),
        ReadOnly);

    target->Set(String::NewSymbol("enum_static_t"),
        Number::New(static_cast<uint32_t>(clang::completion_type::enum_static_t)),
        ReadOnly);

    target->Set(String::NewSymbol("union_t"),
        Number::New(static_cast<uint32_t>(clang::completion_type::union_t)),
        ReadOnly);

    target->Set(String::NewSymbol("typedef_t"),
        Number::New(static_cast<uint32_t>(clang::completion_type::typedef_t)),
        ReadOnly);

    target->Set(String::NewSymbol("variable_t"),
        Number::New(static_cast<uint32_t>(clang::completion_type::variable_t)),
        ReadOnly);

    target->Set(String::NewSymbol("macro_t"),
        Number::New(static_cast<uint32_t>(clang::completion_type::macro_t)),
        ReadOnly);

    target->Set(String::NewSymbol("unkown_t"),
        Number::New(static_cast<uint32_t>(clang::completion_type::unkown_t)),
        ReadOnly);
}

/// set arguments
Handle<Value> node_tool::setArgs(const Arguments& args) {
    node_tool* instance = node::ObjectWrap::Unwrap<node_tool>(args.This());
    HandleScope scope;

    // make sure the syntax is correct
    if (args.Length() != 1 || !args[0]->IsArray())
        return ThrowException(
            Exception::SyntaxError(String::New("Usage: setArgs(Array arguments)"))
        );

    // get arguments and relay to node_tool
    Local<Array> arr = Local<Array>::Cast(args[0]);
    std::vector<std::string> args2;
    std::vector<const char*> args2_pointers(arr->Length());

    // copy the node array to args2
    for (std::size_t i = 0; i < arr->Length(); ++i) {
        String::Utf8Value str(arr->Get(i));
        args2.push_back( *str );
    }

    // copy args2 to args2_pointer
    struct c_str { const char* operator ()(const std::string& s) { return s.c_str(); } };
    std::transform(args2.begin(), args2.end(), args2_pointers.begin(), c_str());

    // arguments set copies it so letting it go out of scope is fine
    instance->tool.arguments_set(&args2_pointers[0], args2_pointers.size());
    return scope.Close(Undefined());
}

/// add / update file
Handle<Value> node_tool::indexTouch(const Arguments& args) {
    node_tool* instance = node::ObjectWrap::Unwrap<node_tool>(args.This());
    HandleScope scope;

    // make sure the syntax is correct
    if (args.Length() != 1 || !args[0]->IsString())
        return ThrowException(
            Exception::SyntaxError(String::New("Usage: indexTouch(String path)"))
        );

    String::Utf8Value str(args[0]);
    instance->tool.index_touch(*str);
    return scope.Close(Undefined());
}

/// add temp contents
Handle<Value> node_tool::indexTouchUnsaved(const Arguments& args) {
    node_tool* instance = node::ObjectWrap::Unwrap<node_tool>(args.This());
    HandleScope scope;

    // make sure the syntax is correct
    if (args.Length() != 2 || !args[0]->IsString() || !args[1]->IsString())
        return ThrowException(
            Exception::SyntaxError(String::New("Usage: indexTouchTemp(String path, String content)"))
        );

    String::Utf8Value pStr(args[0]);
    String::Utf8Value vStr(args[1]);

    instance->tool.index_touch_unsaved(*pStr, *vStr, vStr.length());
    return scope.Close(Undefined());
}

/// memory usage
Handle<Value> node_tool::indexStatus(const Arguments& args) {
    node_tool* instance = node::ObjectWrap::Unwrap<node_tool>(args.This());
    HandleScope scope;

    Local<Array> ret = Array::New();
    auto stat = instance->tool.index_status();

    uint32_t i = 0;
    for (auto &entry : stat) {
        Local<Array> e = Array::New();
        e->Set(0, String::New(entry.first.c_str()));
        e->Set(1, Number::New(entry.second[CXTUResourceUsage_Combined]));
        ret->Set(i++, e);
    }

    return scope.Close(ret);
}

// clear cache
Handle<Value> node_tool::indexClear(const Arguments& args) {
    node_tool* instance = node::ObjectWrap::Unwrap<node_tool>(args.This());
    HandleScope scope;

    // make sure the syntax is correct
    if (args.Length() == 1 && !args[0]->IsString())
        return ThrowException(
            Exception::SyntaxError(String::New("Usage: indexClear([String path])"))
        );

    if (args.Length()) {
        String::Utf8Value str(args[0]);
        instance->tool.index_remove(*str);
    } else {
        instance->tool.index_clear();
    }

    return scope.Close(Undefined());
}

/// get outline
Handle<Value> node_tool::fileOutline(const Arguments& args) {
    node_tool* instance = node::ObjectWrap::Unwrap<node_tool>(args.This());
    HandleScope scope;

    // make sure the syntax is correct
    if (args.Length() != 1 || !args[0]->IsString())
        return ThrowException(
            Exception::SyntaxError(String::New("Usage: fileOutline(String path)"))
        );

    String::Utf8Value str(args[0]);
    auto outline = instance->tool.tu_outline(*str);
    Local<Object> ret = Object::New();
    uint32_t i, j, k = 0;

    // Includes
    i = 0;
    Local<Array> ret_includes = Array::New();
    for (auto &include: outline.includes) {
        std::cout << include << std::endl;
        ret_includes->Set(i++, String::New(include.c_str()));
    }

    // Functions
    Local<Array> ret_functions = Array::New();
    i = 0;
    j = 0;
    for (auto &function: outline.functions) {
        Local<Object> fcn = Object::New();
        Local<Array> params = Array::New();

        for (auto &param: function.params) {
            params->Set(j++, String::New(param.c_str()));
        }


        fcn->Set(String::New("name"), String::New(function.name.c_str()));
        fcn->Set(String::New("params"), params);
        ret_functions->Set(i++, fcn);
    }

    // Classes
    Local<Array> ret_classes = Array::New();
    i = 0;
    j = 0;
    k = 0;
    for (auto &class_: outline.classes) {
        Local<Object> cl = Object::New();
        Local<Array> functions = Array::New();
        Local<Array> attributes = Array::New();

        for (auto &attr : class_.attributes) {
            attributes->Set(j++, String::New(attr.c_str()));
        }

        j = 0;
        k = 0;
        for (auto &func: class_.functions) {
            Local<Object> fcn = Object::New();
            Local<Array> params = Array::New();

            for (auto &param: func.params) {
                params->Set(j++, String::New(param.c_str()));
            }

            fcn->Set(String::New("name"), String::New(func.name.c_str()));
            fcn->Set(String::New("params"), params);
            functions->Set(k++, fcn);
        }

        cl->Set(String::New("name"), String::New(class_.name.c_str()));
        cl->Set(String::New("attributes"), attributes);
        cl->Set(String::New("functions"), functions);
        ret_classes->Set(i++, cl);
    }

    ret->Set(String::New("includes"), ret_includes);
    ret->Set(String::New("functions"), ret_functions);
    ret->Set(String::New("classes"), ret_classes);
    return scope.Close(ret);
}

/// get file diagnostics
Handle<Value> node_tool::fileDiagnose(const Arguments& args) {
    node_tool* instance = node::ObjectWrap::Unwrap<node_tool>(args.This());
    HandleScope scope;

    // make sure the syntax is correct
    if (args.Length() != 1 || !args[0]->IsString())
        return ThrowException(
            Exception::SyntaxError(String::New("Usage: fileDiagnose(String path)"))
        );

    String::Utf8Value str(args[0]);
    auto diag = instance->tool.tu_diagnose(*str);

    // Convert obj to ret
    Local<Array> ret = Array::New();

    uint32_t i = 0;
    for (auto &diagnose : diag) {
        Local<Object> e = Object::New();
        e->Set(String::New("row"), Number::New(diagnose.loc.row));
        e->Set(String::New("col"), Number::New(diagnose.loc.col));
        e->Set(String::New("file"), String::New(diagnose.loc.file.c_str()));
        e->Set(String::New("severity"), Number::New(diagnose.severity));
        e->Set(String::New("text"), String::New(diagnose.text.c_str()));
        e->Set(String::New("summary"), String::New(diagnose.summary.c_str()));
        ret->Set(i++, e);
    }

    return scope.Close(ret);
}

/// code completion
Handle<Value> node_tool::cursorCandidatesAt(const Arguments& args) {
    node_tool* instance = node::ObjectWrap::Unwrap<node_tool>(args.This());
    HandleScope scope;

    // make sure the syntax is correct
    if (args.Length() != 3 || !args[0]->IsString() || !args[1]->IsNumber() || !args[2]->IsNumber())
        return ThrowException(
            Exception::SyntaxError(String::New("Usage: cursorCandidatesAt(String path, Number row, Number column)"))
        );

    String::Utf8Value str(args[0]);
    auto row = args[1]->ToNumber();
    auto col = args[2]->ToNumber();

    // get completion results
    Local<Array> ret = Array::New();
    auto comp = instance->tool.cursor_complete(*str, row->Value(), col->Value());

    uint32_t j = 0;
    for (auto &candidate : comp) {
        Local<Object> entry = Object::New();
        Local<Array> args = Array::New();
        entry->Set(String::New("name"), String::New(candidate.name.c_str()));
        entry->Set(String::New("return_type"), String::New(candidate.return_type.c_str()));
        entry->Set(String::New("type"), Number::New(static_cast<uint32_t>(candidate.type)));

        for (uint32_t i = 0; i < candidate.args.size(); ++i) {
            args->Set(i, String::New(candidate.args[i].c_str()));
        }

        entry->Set(String::New("args"), args);
        ret->Set(j++, entry);
    }

    return scope.Close(ret);
}

/// get type at
Handle<Value> node_tool::cursorTypeAt(const Arguments& args) {
    node_tool* instance = node::ObjectWrap::Unwrap<node_tool>(args.This());
    HandleScope scope;

    // make sure the syntax is correct
    if (args.Length() != 3 || !args[0]->IsString() || !args[1]->IsNumber() || !args[2]->IsNumber())
        return ThrowException(
            Exception::SyntaxError(String::New("Usage: cursorTypeAt(String path, Number row, Number column)"))
        );

    String::Utf8Value str(args[0]);
    auto row = args[1]->ToNumber();
    auto col = args[2]->ToNumber();

    return scope.Close(
        String::New(instance->tool.cursor_type(*str, row->Value(), col->Value()).c_str())
    );
}

/// get decleration for pos
Handle<Value> node_tool::cursorDeclarationAt(const Arguments& args) {
    node_tool* instance = node::ObjectWrap::Unwrap<node_tool>(args.This());
    HandleScope scope;

    // make sure the syntax is correct
    if (args.Length() != 3 || !args[0]->IsString() || !args[1]->IsNumber() || !args[2]->IsNumber())
        return ThrowException(
            Exception::SyntaxError(String::New("Usage: cursorTypeAt(String path, Number row, Number column)"))
        );

    String::Utf8Value str(args[0]);
    auto row = args[1]->ToNumber();
    auto col = args[2]->ToNumber();

    auto loc = instance->tool.cursor_declaration(*str, row->Value(), col->Value());

    Local<Object> ret = Object::New();
    ret->Set(String::New("file"), String::New(loc.file.c_str()));
    ret->Set(String::New("row"), Number::New(loc.row));
    ret->Set(String::New("col"), Number::New(loc.col));

    return scope.Close(ret);
}

/// get definition for pos
Handle<Value> node_tool::cursorDefinitionAt(const Arguments& args) {
    node_tool* instance = node::ObjectWrap::Unwrap<node_tool>(args.This());
    HandleScope scope;

    // make sure the syntax is correct
    if (args.Length() != 3 || !args[0]->IsString() || !args[1]->IsNumber() || !args[2]->IsNumber())
        return ThrowException(
            Exception::SyntaxError(String::New("Usage: cursorTypeAt(String path, Number row, Number column)"))
        );

    String::Utf8Value str(args[0]);
    auto row = args[1]->ToNumber();
    auto col = args[2]->ToNumber();

    auto loc = instance->tool.cursor_definition(*str, row->Value(), col->Value());

    Local<Object> ret = Object::New();
    ret->Set(String::New("file"), String::New(loc.file.c_str()));
    ret->Set(String::New("row"), Number::New(loc.row));
    ret->Set(String::New("col"), Number::New(loc.col));

    return scope.Close(ret);
}

/// module initialization
void initAll(Handle<Object> exports) {
    node_tool::Init(exports);
}

/// export node module
NODE_MODULE(clang_tool, initAll);