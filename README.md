# brace

Incredibly simple JSON parser in C++

## Example usage

```cpp
auto json_str = R"({"hello": "world"}")";
brace::Parser parser;
auto json = parser.parse(json_str).expect("parse");
std::string hello = json["hello"];
printf("Hello, %s!\n", hello.c_str());
```

## Getting Started

A C++ compiler with at least support for C++17 is required. The easiest way to get started is by adding [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake/blob/v0.40.2/cmake/CPM.cmake) to your project:

```cmake
include(cmake/CPM.cmake)
CPMAddPackage("gh:twct/brace#git_tag_or_rev_sha")
target_link_libraries(MyTarget PRIVATE brace)
```

**Build & run unit tests**

```sh
$ mkdir build && cd build
$ cmake -G Ninja -DBRACE_BUILD_TESTS=on ..
$ ninja && ninja test
```

**Build docs**

```sh
$ cd docs && make
# bundle as zip
$ make zip
```
