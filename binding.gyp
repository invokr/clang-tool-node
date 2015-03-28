{
  "targets": [
    {
      "target_name": "clang_tool",
      "sources": [
        "src/clang/clang_diagnostic.cpp",
        "src/clang/clang_ressource_usage.cpp",
        "src/clang/clang_tool.cpp",
        "src/clang/clang_translation_unit.cpp",
        "src/clang/clang_translation_unit_cache.cpp",
        "src/clang/sha1.cpp",
        "src/bindings.cpp"
      ],
      "cflags_cc": [
        "-O3",
        "-fomit-frame-pointer",
        "-std=c++11",
        "-Wall",
        "-Wno-unused-variable",
        "-Wno-unused-function",
        "-Wno-unused-private-field",

        "-I/usr/local/llvm34/include",
        "-I/usr/local/llvm35/include",
        "-I/usr/lib",
        "-I/usr/lib64",
        "-I/usr/lib/llvm",
        "-I/usr/lib64/llvm"
        "-I/usr/include",
        "-I/usr/include/llvm",
        "-I/usr/local/include",
        "-I/usr/local/include/llvm",
        "-I/usr/lib/llvm-3.5/include"
      ],

      "libraries": [
        "-lclang",

        "-L/usr/local/llvm34/lib/",
        "-L/usr/local/llvm35/lib/",
        "-L/usr/lib/x86_64-linux-gnu/",
        "-L/usr/lib/i386-linux-gnu/",
        "-L/usr/lib/llvm-3.5/lib",
        "-L/usr/lib/llvm-3.4/lib"
      ]
    }
  ]
}
