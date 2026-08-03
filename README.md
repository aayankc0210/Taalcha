# Taalcha

Taalcha is a password manager developed as a semester project for Kathmandu University. It is built using C++, Slint, SQLite, and OpenSSL with the goal of exploring secure credential storage and native cross-platform desktop application development.

The project focuses on implementing the core components of a password manager, including encrypted storage, authentication, and a modern desktop interface.

---

## Features

* Local encrypted password storage
* Master password authentication
* Native desktop interface built with Slint
* SQLite database backend
* AES encryption using OpenSSL
* Cross-platform design

---

## Technologies Used

* C++17
* Slint
* SQLite3
* OpenSSL
* CMake
* Ninja

---

## Building

### Prerequisites

Before building, make sure you have the following installed:

* CMake 3.21 or later
* A C++20 compatible compiler
* Ninja
* OpenSSL
* SQLite3
* libcurl

### Slint SDK

Taalcha expects the Slint C++ SDK to be available in one of the following locations:

| Platform | Location                    |
| -------- | --------------------------- |
| Linux    | `vendor/slint-linux/`       |
| macOS    | `vendor/slint-macos/`       |
| Windows  | `C:/Libs/Slint-cpp 1.17.1/` |

### Clone the repository

```bash
git clone https://github.com/aayankc0210/taalcha.git
cd taalcha
```

### Configure

**Linux**

```bash
cmake -S . -B build -G Ninja
```

**Windows (vcpkg)**

```bash
cmake -S . -B build -G Ninja ^
    -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
```

Replace `C:/vcpkg` with the location where vcpkg is installed on your system.
### Configure

**Linux**

```bash
cmake -S . -B build -G Ninja
```

**Windows (vcpkg)**

```bash
cmake -S . -B build -G Ninja -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
```

Replace `C:/vcpkg` with the location where vcpkg is installed on your system.

### Build

```bash
cmake --build build
```

### Run

```bash
./build/taalcha
```

---

## Project Structure

```text
Taalcha/
├── assets/
├── include/
├── src/
├── ui/
├── vendor/
├── CMakeLists.txt
└── README.md
```

---

## Project Objectives

The primary objectives of this project are to:

* Learn modern C++ application development
* Explore cryptographic techniques for securing user credentials
* Design a responsive native desktop interface
* Build a maintainable cross-platform application using CMake

---

## Acknowledgements

This project was developed as part of the B.Sc. Computer Science curriculum at Kathmandu University.

---

## License

This project is licensed under the MIT License.
