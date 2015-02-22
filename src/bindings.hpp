/**
* @file bindings.hpp
* @author Robin Dietrich <me (at) invokr (dot) org>
* @version 1.0
*
* @par License
*   clang-tool
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

#ifndef _CLANG_TOOL_BINDINGS_HPP_
#define _CLANG_TOOL_BINDINGS_HPP_

#include <v8.h>
#include <node.h>

using namespace v8;

class node_tool : public node::ObjectWrap {
public:
    /** Persistend class obj for v8 */
    static Persistent<FunctionTemplate> constructor;

    /** Node's initialize function */
    static void Init(Handle<Object> target);

    /** Returns the current arguments supplied to clang */
    static Handle<Value> getArgs(Local<String> property, const AccessorInfo& info);

    /** Sets the arguments supplied to clang */
    static void setArgs(Local<String> property, Local<Value> value, const AccessorInfo& info);

    /** Adds or updates the specified file on the index */
    static Handle<Value> indexTouch(const Arguments& args);

    /** Returns current memory usage */
    static Handle<Value> indexStatus(const Arguments& args);

    /** Clears all / a single index entry */
    static Handle<Value> indexClear(const Arguments& args);

    /** Returns the outline of the given translation unit */
    static Handle<Value> fileOutline(const Arguments& args);

    /** Returns the candidates for the given location */
    static Handle<Value> fileDiagnose(const Arguments& args);

    /** Returns code completion candidates for given location */
    static Handle<Value> cursorCandidatesAt(const Arguments& args);

    /** Returns type at given location */
    static Handle<Value> cursorTypeAt(const Arguments& args);

    /** Returns where the type under the cursor is declared */
    static Handle<Value> cursorDeclarationAt(const Arguments& args);

    /** Returns where the type under the cursor is defined */
    static Handle<Value> cursorDefinitionAt(const Arguments& args);
private:
    /** Constructor */
    node_tool();

    /** Destructor */
    ~node_tool();

    /** Invoked when a new instance is created in NodeJs */
    static Handle<Value> New(const Arguments& args);
};

#endif /* _CLANG_TOOL_BINDINGS_HPP_ */
