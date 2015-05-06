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
Persistent<FunctionTemplate> node_tool::constructor;

/// constructor
node_tool::node_tool() : ObjectWrap() {}

/// destructor
node_tool::~node_tool() {}

/// new
NAN_METHOD(node_tool::New) {
    NanScope();

    node_tool *ntool = new node_tool();
    ntool->Wrap(args.This());

    NanReturnValue(args.This());
}

/// initializes the njs obj
void node_tool::Init(Handle<Object> target) {
    // Wrap new and make it persistend
    Local<FunctionTemplate> local_function_template = NanNew<FunctionTemplate>(New);
    NanAssignPersistent(node_tool::constructor, local_function_template);

    local_function_template->InstanceTemplate()->SetInternalFieldCount(1);
    local_function_template->SetClassName(NanNew<String>("object"));

    // Make our methods available to Node
    NODE_SET_PROTOTYPE_METHOD(local_function_template, "setArgs",             setArgs);
    NODE_SET_PROTOTYPE_METHOD(local_function_template, "indexTouch",          indexTouch);
    NODE_SET_PROTOTYPE_METHOD(local_function_template, "indexTouchUnsaved",   indexTouchUnsaved);
    NODE_SET_PROTOTYPE_METHOD(local_function_template, "indexStatus",         indexStatus);
    NODE_SET_PROTOTYPE_METHOD(local_function_template, "indexClear",          indexClear);
    NODE_SET_PROTOTYPE_METHOD(local_function_template, "fileAst",             fileAst);
    NODE_SET_PROTOTYPE_METHOD(local_function_template, "fileDiagnose",        fileDiagnose);
    NODE_SET_PROTOTYPE_METHOD(local_function_template, "cursorCandidatesAt",  cursorCandidatesAt);
    NODE_SET_PROTOTYPE_METHOD(local_function_template, "cursorTypeAt",        cursorTypeAt);
    NODE_SET_PROTOTYPE_METHOD(local_function_template, "cursorDeclarationAt", cursorDeclarationAt);
    NODE_SET_PROTOTYPE_METHOD(local_function_template, "cursorDefinitionAt",  cursorDefinitionAt);

    // Add constructor to our addon
    target->Set(NanNew<String>("object"), local_function_template->GetFunction());

    // Add all completion types to the addon
    target->Set(NanNew<String>("namespace_t"),
        NanNew<Number>(static_cast<uint32_t>(clang::completion_type::namespace_t)));

    target->Set(NanNew<String>("class_t"),
        NanNew<Number>(static_cast<uint32_t>(clang::completion_type::class_t)));

    target->Set(NanNew<String>("attribute_t"),
        NanNew<Number>(static_cast<uint32_t>(clang::completion_type::attribute_t)));

    target->Set(NanNew<String>("method_t"),
        NanNew<Number>(static_cast<uint32_t>(clang::completion_type::method_t)));

    target->Set(NanNew<String>("parameter_t"),
        NanNew<Number>(static_cast<uint32_t>(clang::completion_type::parameter_t)));

    target->Set(NanNew<String>("struct_t"),
        NanNew<Number>(static_cast<uint32_t>(clang::completion_type::struct_t)));

    target->Set(NanNew<String>("function_t"),
        NanNew<Number>(static_cast<uint32_t>(clang::completion_type::function_t)));

    target->Set(NanNew<String>("enum_t"),
        NanNew<Number>(static_cast<uint32_t>(clang::completion_type::enum_t)));

    target->Set(NanNew<String>("enum_static_t"),
        NanNew<Number>(static_cast<uint32_t>(clang::completion_type::enum_static_t)));

    target->Set(NanNew<String>("union_t"),
        NanNew<Number>(static_cast<uint32_t>(clang::completion_type::union_t)));

    target->Set(NanNew<String>("typedef_t"),
        NanNew<Number>(static_cast<uint32_t>(clang::completion_type::typedef_t)));

    target->Set(NanNew<String>("variable_t"),
        NanNew<Number>(static_cast<uint32_t>(clang::completion_type::variable_t)));

    target->Set(NanNew<String>("macro_t"),
        NanNew<Number>(static_cast<uint32_t>(clang::completion_type::macro_t)));

    target->Set(NanNew<String>("include_t"),
        NanNew<Number>(static_cast<uint32_t>(clang::completion_type::include_t)));

    target->Set(NanNew<String>("unkown_t"),
        NanNew<Number>(static_cast<uint32_t>(clang::completion_type::unkown_t)));
}

/// set arguments
NAN_METHOD(node_tool::setArgs) {
    NanScope();
    node_tool* instance = node::ObjectWrap::Unwrap<node_tool>(args.This());

    // make sure the syntax is correct
    if (args.Length() != 1 || !args[0]->IsArray())
      NanThrowError("Usage: setArgs(Array arguments)");

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
    NanReturnUndefined();
}

/// add / update file
NAN_METHOD(node_tool::indexTouch) {
    NanScope();
    node_tool* instance = node::ObjectWrap::Unwrap<node_tool>(args.This());

    // make sure the syntax is correct
    if (args.Length() != 1 || !args[0]->IsString())
      NanThrowError("Usage: indexTouch(String path)");

    String::Utf8Value str(args[0]);
    instance->tool.index_touch(*str);
    NanReturnUndefined();
}

/// add temp contents
NAN_METHOD(node_tool::indexTouchUnsaved) {
    NanScope();
    node_tool* instance = node::ObjectWrap::Unwrap<node_tool>(args.This());

    // make sure the syntax is correct
    if (args.Length() != 2 || !args[0]->IsString() || !args[1]->IsString())
        NanThrowError("Usage: indexTouchTemp(String path, String content)");

    String::Utf8Value pStr(args[0]);
    String::Utf8Value vStr(args[1]);

    instance->tool.index_touch_unsaved(*pStr, *vStr, vStr.length());
    NanReturnUndefined();
}

/// memory usage
NAN_METHOD(node_tool::indexStatus) {
    NanScope();
    node_tool* instance = node::ObjectWrap::Unwrap<node_tool>(args.This());

    Local<Array> ret = NanNew<Array>();
    auto stat = instance->tool.index_status();

    uint32_t i = 0;
    for (auto &entry : stat) {
        Local<Array> e = NanNew<Array>();
        e->Set(0, NanNew<String>(entry.first.c_str()));
        e->Set(1, NanNew<Number>(entry.second[CXTUResourceUsage_Combined]));
        ret->Set(i++, e);
    }

    NanReturnValue(ret);
}

// clear cache
NAN_METHOD(node_tool::indexClear) {
    NanScope();
    node_tool* instance = node::ObjectWrap::Unwrap<node_tool>(args.This());

    // make sure the syntax is correct
    if (args.Length() == 1 && !args[0]->IsString())
        NanThrowError("Usage: indexClear([String path])");

    if (args.Length()) {
        String::Utf8Value str(args[0]);
        instance->tool.index_remove(*str);
    } else {
        instance->tool.index_clear();
    }

    NanReturnUndefined();
}

/// returns file ast
NAN_METHOD(node_tool::fileAst) {
    NanScope();
    node_tool* instance = node::ObjectWrap::Unwrap<node_tool>(args.This());

     // make sure the syntax is correct
    if (args.Length() != 1 || !args[0]->IsString())
        NanThrowError("Usage: fileAst(String path)");

    String::Utf8Value str(args[0]);
    auto ast = instance->tool.tu_ast(*str);

    std::function<void(clang::ast_element*, Local<Object>)> astVisitor;
    astVisitor = [&](clang::ast_element *e, Local<Object> o) {
        if (e->cursor != clang::completion_type::unkown_t) {
            o->Set(NanNew<String>("name"), NanNew<String>(e->name.c_str()));
            o->Set(NanNew<String>("type"), NanNew<String>(e->type.c_str()));
            o->Set(NanNew<String>("doc"), NanNew<String>(e->doc.c_str()));
            o->Set(NanNew<String>("cursor"), NanNew<Number>(static_cast<uint32_t>(e->cursor)));
            o->Set(NanNew<String>("access"), NanNew<Number>(static_cast<uint32_t>(e->access)));
            o->Set(NanNew<String>("loc_file"), NanNew<String>(e->loc.file.c_str()));
            o->Set(NanNew<String>("loc_col"), NanNew<Number>(static_cast<uint32_t>(e->loc.col)));
            o->Set(NanNew<String>("loc_row"), NanNew<Number>(static_cast<uint32_t>(e->loc.row)));
        }

        Local<Array> children = NanNew<Array>();
        uint32_t children_idx = 0;

        for (auto &eC : e->children) {
            Local<Object> c = NanNew<Object>();
            astVisitor(&eC, c);
            children->Set(children_idx++, c);
        }

        o->Set(NanNew<String>("children"), children);
    };

    Local<Object> ret = NanNew<Object>();
    astVisitor(&ast, ret);

    NanReturnValue(ret);
}

/// get file diagnostics
NAN_METHOD(node_tool::fileDiagnose) {
    NanScope();
    node_tool* instance = node::ObjectWrap::Unwrap<node_tool>(args.This());

    // make sure the syntax is correct
    if (args.Length() != 1 || !args[0]->IsString())
        NanThrowError("Usage: fileDiagnose(String path)");

    String::Utf8Value str(args[0]);
    auto diag = instance->tool.tu_diagnose(*str);

    // Convert obj to ret
    Local<Array> ret = NanNew<Array>();

    uint32_t i = 0;
    for (auto &diagnose : diag) {
        Local<Object> e = NanNew<Object>();
        e->Set(NanNew<String>("row"), NanNew<Number>(diagnose.loc.row));
        e->Set(NanNew<String>("col"), NanNew<Number>(diagnose.loc.col));
        e->Set(NanNew<String>("file"), NanNew<String>(diagnose.loc.file.c_str()));
        e->Set(NanNew<String>("severity"), NanNew<Number>(diagnose.severity));
        e->Set(NanNew<String>("text"), NanNew<String>(diagnose.text.c_str()));
        e->Set(NanNew<String>("summary"), NanNew<String>(diagnose.summary.c_str()));
        ret->Set(i++, e);
    }

    NanReturnValue(ret);
}

/// code completion
NAN_METHOD(node_tool::cursorCandidatesAt) {
    NanScope();
    node_tool* instance = node::ObjectWrap::Unwrap<node_tool>(args.This());

    // make sure the syntax is correct
    if (args.Length() != 3 || !args[0]->IsString() || !args[1]->IsNumber() || !args[2]->IsNumber())
        NanThrowError("Usage: cursorCandidatesAt(String path, Number row, Number column)");

    String::Utf8Value str(args[0]);
    auto row = args[1]->ToNumber();
    auto col = args[2]->ToNumber();

    // get completion results
    Local<Array> ret = NanNew<Array>();
    auto comp = instance->tool.cursor_complete(*str, row->Value(), col->Value());

    uint32_t j = 0;
    for (auto &candidate : comp) {
        Local<Object> entry = NanNew<Object>();
        Local<Array> args = NanNew<Array>();
        entry->Set(NanNew<String>("name"), NanNew<String>(candidate.name.c_str()));
        entry->Set(NanNew<String>("return_type"), NanNew<String>(candidate.return_type.c_str()));
        entry->Set(NanNew<String>("type"), NanNew<Number>(static_cast<uint32_t>(candidate.type)));
        entry->Set(NanNew<String>("brief"), NanNew<String>(candidate.brief.c_str()));
        entry->Set(NanNew<String>("priority"), NanNew<Number>(candidate.priority));

        for (uint32_t i = 0; i < candidate.args.size(); ++i) {
            args->Set(i, NanNew<String>(candidate.args[i].c_str()));
        }

        entry->Set(NanNew<String>("args"), args);
        ret->Set(j++, entry);
    }

    NanReturnValue(ret);
}

/// get type at
NAN_METHOD(node_tool::cursorTypeAt) {
    NanScope();
    node_tool* instance = node::ObjectWrap::Unwrap<node_tool>(args.This());

    // make sure the syntax is correct
    if (args.Length() != 3 || !args[0]->IsString() || !args[1]->IsNumber() || !args[2]->IsNumber())
        NanThrowError("Usage: cursorTypeAt(String path, Number row, Number column)");

    String::Utf8Value str(args[0]);
    auto row = args[1]->ToNumber();
    auto col = args[2]->ToNumber();

    NanReturnValue(
        NanNew<String>(instance->tool.cursor_type(*str, row->Value(), col->Value()).c_str())
    );
}

/// get decleration for pos
NAN_METHOD(node_tool::cursorDeclarationAt) {
    NanScope();
    node_tool* instance = node::ObjectWrap::Unwrap<node_tool>(args.This());

    // make sure the syntax is correct
    if (args.Length() != 3 || !args[0]->IsString() || !args[1]->IsNumber() || !args[2]->IsNumber())
        NanThrowError("Usage: cursorTypeAt(String path, Number row, Number column)");

    String::Utf8Value str(args[0]);
    auto row = args[1]->ToNumber();
    auto col = args[2]->ToNumber();

    auto loc = instance->tool.cursor_declaration(*str, row->Value(), col->Value());

    Local<Object> ret = NanNew<Object>();
    ret->Set(NanNew<String>("file"), NanNew<String>(loc.file.c_str()));
    ret->Set(NanNew<String>("row"), NanNew<Number>(loc.row));
    ret->Set(NanNew<String>("col"), NanNew<Number>(loc.col));

    NanReturnValue(ret);
}

/// get definition for pos
NAN_METHOD(node_tool::cursorDefinitionAt) {
    NanScope();
    node_tool* instance = node::ObjectWrap::Unwrap<node_tool>(args.This());

    // make sure the syntax is correct
    if (args.Length() != 3 || !args[0]->IsString() || !args[1]->IsNumber() || !args[2]->IsNumber())
        NanThrowError("Usage: cursorTypeAt(String path, Number row, Number column)");

    String::Utf8Value str(args[0]);
    auto row = args[1]->ToNumber();
    auto col = args[2]->ToNumber();

    auto loc = instance->tool.cursor_definition(*str, row->Value(), col->Value());

    Local<Object> ret = NanNew<Object>();
    ret->Set(NanNew<String>("file"), NanNew<String>(loc.file.c_str()));
    ret->Set(NanNew<String>("row"), NanNew<Number>(loc.row));
    ret->Set(NanNew<String>("col"), NanNew<Number>(loc.col));

    NanReturnValue(ret);
}

/// module initialization
void initAll(Handle<Object> exports) {
    node_tool::Init(exports);
}

/// export node module
NODE_MODULE(clang_tool, initAll);
