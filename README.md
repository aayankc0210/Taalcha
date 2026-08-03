# Taalcha: Secure Desktop Password Manager

A modern, Material 3 Expressive password manager built with C++ and Slint UI.

## Build

### Linux

```bash
cmake -B build
cmake --build build
./build/taalcha
```

### Windows (MinGW)

Install MinGW-w64 and Ninja, then:

```
cmake -B build-win \
-G Ninja \
-DCMAKE_SYSTEM_NAME=Windows \
-DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc \
-DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ \
-DCMAKE_BUILD_TYPE=Release

cmake --build build-win
```