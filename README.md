clang-tool
==========

Simple and small utility library to be used in IDE's to provide C / C++ support using libclang.

Usage disclaimer: This library is in alpha stage.

Installing
----------

The library is available via npm under the name `clang_tool`.

Installation is as easy as:

    npm install clang_tool

Because each and every distribution feels the need to put the headers for
libclang-c in a different place, you might need to add the correct include path
for your distribution in `bindings.gyp`.

Usage
-----

See demo/demo.js for a quick example.

Contributers
------------

[Code Hz](https://github.com/codehz) 
[Petr Kobalicek](https://github.com/kobalicek)

API
---

The following functions are exported:

    /// Sets the compiler arguments
    void setArgs(Array args);

    /// Adds or updates the specified file on the index
    void indexTouch(String file);

    /// Adds temporary file content to the index
    void indexTouchUnsaved(String file, String value);

    /// Returns memory usage statistics for each file on the index
    Object indexStatus();

    /// Clears all [a single] cache entries
    void indexClear([String file]);

    /// Returns the ast of the given file
    Object fileAst(String file);

    /// Returns diagnostic information for the given file
    Object fileDiagnose(String file);

    /// Returns code completion candidates
    Object cursorCandidatesAt(String file, Number row, Number col);

    /// Returns where the type under the cursor is declared
    Object cursorTypeAt(String file, Number row, Number col);

    /// Returns where the type under the cursor is defined
    Object cursorDefinitionAt(String file, Number row, Number col);

    /// Returns where the type under the cursor is decleared
    Object cursorDeclarationAt(String file, Number row, Number col);

All functions that have a `String file` argument require the file to be added to the index using
`indexTouch(file)` beforehand. Failing to do so will result in an exception.

All path's supplied must be absolute. Using relative path may lead to undefined behavior when using
the ast and diagnostic functions.
