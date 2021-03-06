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

#include <nan.h>

#include "clang/clang_tool.hpp"

using namespace v8;

class node_tool : public Nan::ObjectWrap {
public:
    /** Persistend class obj for v8 */
    static Nan::Persistent<FunctionTemplate> constructor;

    /** Node's initialize function */
    static void Init(Handle<Object> target);

    /** Returns the current arguments supplied to clang */
    static NAN_METHOD(setArgs);

    /** Adds or updates the specified file on the index */
    static NAN_METHOD(indexTouch);

    /** Adds temporary content for specified file on the index, will be purged when using indexTouch */
    static NAN_METHOD(indexTouchUnsaved);

    /** Returns current memory usage */
    static NAN_METHOD(indexStatus);

    /** Clears all / a single index entry */
    static NAN_METHOD(indexClear);

    /** Returns the ast of the given translation unit */
    static NAN_METHOD(fileAst);

    /** Returns the candidates for the given location */
    static NAN_METHOD(fileDiagnose);

    /** Returns code completion candidates for given location */
    static NAN_METHOD(cursorCandidatesAt);

    /** Returns type at given location */
    static NAN_METHOD(cursorTypeAt);

    /** Returns where the type under the cursor is declared */
    static NAN_METHOD(cursorDeclarationAt);

    /** Returns where the type under the cursor is defined */
    static NAN_METHOD(cursorDefinitionAt);
private:
    /** Constructor */
    node_tool();
    /** Destructor */
    ~node_tool();

    /** Invoked when a new instance is created in NodeJs */
    static NAN_METHOD(New);

    /** Underlying clang-tool instance we are binding */
    clang::tool tool;
};

#endif /* _CLANG_TOOL_BINDINGS_HPP_ */
