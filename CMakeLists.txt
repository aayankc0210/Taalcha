cmake_minimum_required(VERSION 3.21)
project(Taalcha)

if(WIN32)
    list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/vendor/slint-windows/lib/cmake")
elseif(APPLE)
    list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/vendor/slint-macos/lib/cmake")
else()
    list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/vendor/slint-linux/lib/cmake")
endif()

find_package(Slint REQUIRED)
find_package(OpenSSL REQUIRED)

add_executable(taalcha
    main.cpp
    src/db.cpp
    src/VaultManager.cpp
    src/VaultManagerInit.cpp
    src/VaultManagerAuth.cpp
    src/VaultManagerCrypto.cpp
)

target_include_directories(taalcha PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include
    ${OPENSSL_INCLUDE_DIR}
)

slint_target_sources(taalcha src/ui/app.slint)

target_link_libraries(taalcha PRIVATE
    Slint::Slint
    sqlite3
    OpenSSL::SSL
    OpenSSL::Crypto
)
