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

#include <algorithm>
#include <vector>

#include "clang/clang_tool.hpp"
#include "bindings.hpp"

/// persistance between calls
Nan::Persistent<FunctionTemplate> node_tool::constructor;

/// constructor
node_tool::node_tool() : Nan::ObjectWrap() {}

/// destructor
node_tool::~node_tool() {}

/// new
NAN_METHOD(node_tool::New) {

    node_tool *ntool = new node_tool();
    ntool->Wrap(info.This());

    info.GetReturnValue().Set(info.This());
}

/// initializes the njs obj
void node_tool::Init(Handle<Object> target) {
    // Wrap new and make it persistend
    Local<FunctionTemplate> local_function_template = Nan::New<FunctionTemplate>(New);
    node_tool::constructor.Reset(local_function_template);

    local_function_template->InstanceTemplate()->SetInternalFieldCount(1);
    local_function_template->SetClassName(Nan::New<String>("object").ToLocalChecked());

    // Make our methods available to Node
    Nan::SetPrototypeMethod(local_function_template, "setArgs",             setArgs);
    Nan::SetPrototypeMethod(local_function_template, "indexTouch",          indexTouch);
    Nan::SetPrototypeMethod(local_function_template, "indexTouchUnsaved",   indexTouchUnsaved);
    Nan::SetPrototypeMethod(local_function_template, "indexStatus",         indexStatus);
    Nan::SetPrototypeMethod(local_function_template, "indexClear",          indexClear);
    Nan::SetPrototypeMethod(local_function_template, "fileAst",             fileAst);
    Nan::SetPrototypeMethod(local_function_template, "fileDiagnose",        fileDiagnose);
    Nan::SetPrototypeMethod(local_function_template, "cursorCandidatesAt",  cursorCandidatesAt);
    Nan::SetPrototypeMethod(local_function_template, "cursorTypeAt",        cursorTypeAt);
    Nan::SetPrototypeMethod(local_function_template, "cursorDeclarationAt", cursorDeclarationAt);
    Nan::SetPrototypeMethod(local_function_template, "cursorDefinitionAt",  cursorDefinitionAt);

    // Add constructor to our addon
    target->Set(Nan::New("object").ToLocalChecked(), local_function_template->GetFunction());

    // Add all completion types to the addon
    Nan::Set(target, Nan::New<String>("namespace_t").ToLocalChecked(),
        Nan::New<Number>(static_cast<uint32_t>(clang::completion_type::namespace_t)));

    Nan::Set(target, Nan::New<String>("class_t").ToLocalChecked(),
        Nan::New<Number>(static_cast<uint32_t>(clang::completion_type::class_t)));

    Nan::Set(target, Nan::New<String>("attribute_t").ToLocalChecked(),
        Nan::New<Number>(static_cast<uint32_t>(clang::completion_type::attribute_t)));

    Nan::Set(target, Nan::New<String>("method_t").ToLocalChecked(),
        Nan::New<Number>(static_cast<uint32_t>(clang::completion_type::method_t)));

    Nan::Set(target, Nan::New<String>("parameter_t").ToLocalChecked(),
        Nan::New<Number>(static_cast<uint32_t>(clang::completion_type::parameter_t)));

    Nan::Set(target, Nan::New<String>("struct_t").ToLocalChecked(),
        Nan::New<Number>(static_cast<uint32_t>(clang::completion_type::struct_t)));

    Nan::Set(target, Nan::New<String>("function_t").ToLocalChecked(),
        Nan::New<Number>(static_cast<uint32_t>(clang::completion_type::function_t)));

    Nan::Set(target, Nan::New<String>("enum_t").ToLocalChecked(),
        Nan::New<Number>(static_cast<uint32_t>(clang::completion_type::enum_t)));

    Nan::Set(target, Nan::New<String>("enum_static_t").ToLocalChecked(),
        Nan::New<Number>(static_cast<uint32_t>(clang::completion_type::enum_static_t)));

    Nan::Set(target, Nan::New<String>("union_t").ToLocalChecked(),
        Nan::New<Number>(static_cast<uint32_t>(clang::completion_type::union_t)));

    Nan::Set(target, Nan::New<String>("typedef_t").ToLocalChecked(),
        Nan::New<Number>(static_cast<uint32_t>(clang::completion_type::typedef_t)));

    Nan::Set(target, Nan::New<String>("variable_t").ToLocalChecked(),
        Nan::New<Number>(static_cast<uint32_t>(clang::completion_type::variable_t)));

    Nan::Set(target, Nan::New<String>("macro_t").ToLocalChecked(),
        Nan::New<Number>(static_cast<uint32_t>(clang::completion_type::macro_t)));

    Nan::Set(target, Nan::New<String>("include_t").ToLocalChecked(),
        Nan::New<Number>(static_cast<uint32_t>(clang::completion_type::include_t)));

    Nan::Set(target, Nan::New<String>("unkown_t").ToLocalChecked(),
        Nan::New<Number>(static_cast<uint32_t>(clang::completion_type::unkown_t)));
}

/// set arguments
NAN_METHOD(node_tool::setArgs) {
    node_tool* instance = Nan::ObjectWrap::Unwrap<node_tool>(info.This());

    // make sure the syntax is correct
    if (info.Length() != 1 || !info[0]->IsArray())
      Nan::ThrowError("Usage: setArgs(Array arguments)");

    // get arguments and relay to node_tool
    Local<Array> arr = Local<Array>::Cast(info[0]);
    std::vector<std::string> args2;
    std::vector<const char*> args2_pointers(arr->Length());

    // copy the node array to args2
    for (std::size_t i = 0; i < arr->Length(); ++i) {
        String::Utf8Value str(arr->Get(i));;
        args2.push_back( *str );
    }

    // copy args2 to args2_pointer
    struct c_str { const char* operator ()(const std::string& s) { return s.c_str(); } };
    std::transform(args2.begin(), args2.end(), args2_pointers.begin(), c_str());

    // arguments set copies it so letting it go out of scope is fine
    instance->tool.arguments_set(&args2_pointers[0], args2_pointers.size());
    return;
}

/// add / update file
NAN_METHOD(node_tool::indexTouch) {
    node_tool* instance = Nan::ObjectWrap::Unwrap<node_tool>(info.This());

    // make sure the syntax is correct
    if (info.Length() != 1 || !info[0]->IsString())
      Nan::ThrowError("Usage: indexTouch(String path)");

    String::Utf8Value str(info[0]);
    instance->tool.index_touch(*str);
    return;
}

/// add temp contents
NAN_METHOD(node_tool::indexTouchUnsaved) {
    node_tool* instance = Nan::ObjectWrap::Unwrap<node_tool>(info.This());

    // make sure the syntax is correct
    if (info.Length() != 2 || !info[0]->IsString() || !info[1]->IsString())
        Nan::ThrowError("Usage: indexTouchTemp(String path, String content)");

    String::Utf8Value pStr(info[0]);
    String::Utf8Value vStr(info[1]);

    instance->tool.index_touch_unsaved(*pStr, *vStr, vStr.length());
    return;
}

/// memory usage
NAN_METHOD(node_tool::indexStatus) {
    node_tool* instance = Nan::ObjectWrap::Unwrap<node_tool>(info.This());

    Local<Array> ret = Nan::New<Array>();
    auto stat = instance->tool.index_status();

    uint32_t i = 0;
    for (auto &entry : stat) {
        Local<Array> e = Nan::New<Array>();
        Nan::Set(e, Nan::New(0), Nan::New<String>(entry.first.c_str()).ToLocalChecked());
        Nan::Set(e, Nan::New(1), Nan::New<Number>(entry.second[CXTUResourceUsage_Combined]));
        Nan::Set(ret, i++, e);
    }

    info.GetReturnValue().Set(ret);
}

// clear cache
NAN_METHOD(node_tool::indexClear) {
    node_tool* instance = Nan::ObjectWrap::Unwrap<node_tool>(info.This());

    // make sure the syntax is correct
    if (info.Length() == 1 && !info[0]->IsString())
        Nan::ThrowError("Usage: indexClear([String path])");

    if (info.Length()) {
        String::Utf8Value str(info[0]);
        instance->tool.index_remove(*str);
    } else {
        instance->tool.index_clear();
    }

    return;
}

/// returns file ast
NAN_METHOD(node_tool::fileAst) {
    node_tool* instance = Nan::ObjectWrap::Unwrap<node_tool>(info.This());

     // make sure the syntax is correct
    if (info.Length() != 1 || !info[0]->IsString())
        Nan::ThrowError("Usage: fileAst(String path)");

    String::Utf8Value str(info[0]);
    auto ast = instance->tool.tu_ast(*str);

    std::function<void(clang::ast_element*, Local<Object>)> astVisitor;
    astVisitor = [&](clang::ast_element *e, Local<Object> o) {
        if (e->cursor != clang::completion_type::unkown_t) {
            Nan::Set(o, Nan::New<String>("name").ToLocalChecked(), Nan::New<String>(e->name.c_str()).ToLocalChecked());
            Nan::Set(o, Nan::New<String>("type").ToLocalChecked(), Nan::New<String>(e->type.c_str()).ToLocalChecked());
            Nan::Set(o, Nan::New<String>("typedef").ToLocalChecked(), Nan::New<String>(e->typedefType.c_str()).ToLocalChecked());
            Nan::Set(o, Nan::New<String>("doc").ToLocalChecked(), Nan::New<String>(e->doc.c_str()).ToLocalChecked());
            Nan::Set(o, Nan::New<String>("cursor").ToLocalChecked(), Nan::New<Number>(static_cast<uint32_t>(e->cursor)));
            Nan::Set(o, Nan::New<String>("access").ToLocalChecked(), Nan::New<Number>(static_cast<uint32_t>(e->access)));
            Nan::Set(o, Nan::New<String>("loc_file").ToLocalChecked(), Nan::New<String>(e->loc.file.c_str()).ToLocalChecked());
            Nan::Set(o, Nan::New<String>("loc_col").ToLocalChecked(), Nan::New<Number>(static_cast<uint32_t>(e->loc.col)));
            Nan::Set(o, Nan::New<String>("loc_row").ToLocalChecked(), Nan::New<Number>(static_cast<uint32_t>(e->loc.row)));
        }

        Local<Array> children = Nan::New<Array>();
        uint32_t children_idx = 0;

        for (auto &eC : e->children) {
            Local<Object> c = Nan::New<Object>();
            astVisitor(&eC, c);
            Nan::Set(children, children_idx++, c);
        }

        Nan::Set(o, Nan::New<String>("children").ToLocalChecked(), children);
    };

    Local<Object> ret = Nan::New<Object>();
    astVisitor(&ast, ret);

    info.GetReturnValue().Set(ret);
}

/// get file diagnostics
NAN_METHOD(node_tool::fileDiagnose) {
    node_tool* instance = Nan::ObjectWrap::Unwrap<node_tool>(info.This());

    // make sure the syntax is correct
    if (info.Length() != 1 || !info[0]->IsString())
        Nan::ThrowError("Usage: fileDiagnose(String path)");

    String::Utf8Value str(info[0]);
    auto diag = instance->tool.tu_diagnose(*str);

    // Convert obj to ret
    Local<Array> ret = Nan::New<Array>();

    uint32_t i = 0;
    for (auto &diagnose : diag) {
        Local<Object> e = Nan::New<Object>();
        Nan::Set(e, Nan::New<String>("row").ToLocalChecked(), Nan::New<Number>(diagnose.loc.row));
        Nan::Set(e, Nan::New<String>("col").ToLocalChecked(), Nan::New<Number>(diagnose.loc.col));
        Nan::Set(e, Nan::New<String>("file").ToLocalChecked(), Nan::New<String>(diagnose.loc.file.c_str()).ToLocalChecked());
        Nan::Set(e, Nan::New<String>("severity").ToLocalChecked(), Nan::New<Number>(diagnose.severity));
        Nan::Set(e, Nan::New<String>("text").ToLocalChecked(), Nan::New<String>(diagnose.text.c_str()).ToLocalChecked());
        Nan::Set(e, Nan::New<String>("summary").ToLocalChecked(), Nan::New<String>(diagnose.summary.c_str()).ToLocalChecked());
        Nan::Set(ret, i++, e);
    }

    info.GetReturnValue().Set(ret);
}

/// code completion
NAN_METHOD(node_tool::cursorCandidatesAt) {
    node_tool* instance = Nan::ObjectWrap::Unwrap<node_tool>(info.This());

    // make sure the syntax is correct
    if (info.Length() != 3 || !info[0]->IsString() || !info[1]->IsNumber() || !info[2]->IsNumber())
        Nan::ThrowError("Usage: cursorCandidatesAt(String path, Number row, Number column)");

    String::Utf8Value str(info[0]);
    auto row = info[1]->ToNumber();
    auto col = info[2]->ToNumber();

    // get completion results
    Local<Array> ret = Nan::New<Array>();
    auto comp = instance->tool.cursor_complete(*str, row->Value(), col->Value());

    uint32_t j = 0;
    for (auto &candidate : comp) {
        Local<Object> entry = Nan::New<Object>();
        Local<Array> info = Nan::New<Array>();
        Nan::Set(entry, Nan::New<String>("name").ToLocalChecked(), Nan::New<String>(candidate.name.c_str()).ToLocalChecked());
        Nan::Set(entry, Nan::New<String>("return_type").ToLocalChecked(), Nan::New<String>(candidate.return_type.c_str()).ToLocalChecked());
        Nan::Set(entry, Nan::New<String>("type").ToLocalChecked(), Nan::New<Number>(static_cast<uint32_t>(candidate.type)));
        Nan::Set(entry, Nan::New<String>("brief").ToLocalChecked(), Nan::New<String>(candidate.brief.c_str()).ToLocalChecked());
        Nan::Set(entry, Nan::New<String>("priority").ToLocalChecked(), Nan::New<Number>(candidate.priority));

        for (uint32_t i = 0; i < candidate.args.size(); ++i) {
            Nan::Set(info, i, Nan::New<String>(candidate.args[i].c_str()).ToLocalChecked());
        }

        Nan::Set(entry, Nan::New<String>("info").ToLocalChecked(), info);
        Nan::Set(ret, j++, entry);
    }

    info.GetReturnValue().Set(ret);
}

/// get type at
NAN_METHOD(node_tool::cursorTypeAt) {
    node_tool* instance = Nan::ObjectWrap::Unwrap<node_tool>(info.This());

    // make sure the syntax is correct
    if (info.Length() != 3 || !info[0]->IsString() || !info[1]->IsNumber() || !info[2]->IsNumber())
        Nan::ThrowError("Usage: cursorTypeAt(String path, Number row, Number column)");

    String::Utf8Value str(info[0]);
    auto row = info[1]->ToNumber();
    auto col = info[2]->ToNumber();

    info.GetReturnValue().Set(
        Nan::New<String>(instance->tool.cursor_type(*str, row->Value(), col->Value()).c_str()).ToLocalChecked()
    );
}

/// get decleration for pos
NAN_METHOD(node_tool::cursorDeclarationAt) {
    node_tool* instance = Nan::ObjectWrap::Unwrap<node_tool>(info.This());

    // make sure the syntax is correct
    if (info.Length() != 3 || !info[0]->IsString() || !info[1]->IsNumber() || !info[2]->IsNumber())
        Nan::ThrowError("Usage: cursorTypeAt(String path, Number row, Number column)");

    String::Utf8Value str(info[0]);
    auto row = info[1]->ToNumber();
    auto col = info[2]->ToNumber();

    auto loc = instance->tool.cursor_declaration(*str, row->Value(), col->Value());

    Local<Object> ret = Nan::New<Object>();
    Nan::Set(ret, Nan::New<String>("file").ToLocalChecked(), Nan::New<String>(loc.file.c_str()).ToLocalChecked());
    Nan::Set(ret, Nan::New<String>("row").ToLocalChecked(), Nan::New<Number>(loc.row));
    Nan::Set(ret, Nan::New<String>("col").ToLocalChecked(), Nan::New<Number>(loc.col));

    info.GetReturnValue().Set(ret);
}

/// get definition for pos
NAN_METHOD(node_tool::cursorDefinitionAt) {
    node_tool* instance = Nan::ObjectWrap::Unwrap<node_tool>(info.This());

    // make sure the syntax is correct
    if (info.Length() != 3 || !info[0]->IsString() || !info[1]->IsNumber() || !info[2]->IsNumber())
        Nan::ThrowError("Usage: cursorTypeAt(String path, Number row, Number column)");

    String::Utf8Value str(info[0]);
    auto row = info[1]->ToNumber();
    auto col = info[2]->ToNumber();

    auto loc = instance->tool.cursor_definition(*str, row->Value(), col->Value());

    Local<Object> ret = Nan::New<Object>();
    Nan::Set(ret, Nan::New<String>("file").ToLocalChecked(), Nan::New<String>(loc.file.c_str()).ToLocalChecked());
    Nan::Set(ret, Nan::New<String>("row").ToLocalChecked(), Nan::New<Number>(loc.row));
    Nan::Set(ret, Nan::New<String>("col").ToLocalChecked(), Nan::New<Number>(loc.col));

    info.GetReturnValue().Set(ret);
}

/// module initialization
void initAll(Handle<Object> exports) {
    node_tool::Init(exports);
}

/// export node module
NODE_MODULE(clang_tool, initAll);
