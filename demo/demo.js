// Get the native module and initialize the library
var clang_autocomplete = require("../build/Release/clang_tool.node");
var obj = new clang_autocomplete.object;

// Set your projects compiler flags
obj.setArgs(["-I/usr/include", "-I/usr/local/include"]);

// Add files to the index, an absolute path is required
// If the path is relative, reparses will break certain functions
var path = require('path');
var file = path.resolve('./demo.cpp');
obj.indexTouch(file);

/// You can now call any of the internal functions
console.log(
    obj.fileAst(file)
);
