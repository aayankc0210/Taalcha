cmake_minimum_required(VERSION 3.21)

project(Taalcha LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)

    # Official Slint MSVC SDK
    set(SLINT_ROOT "C:/Libs/Slint-cpp 1.17.1")
    set(Slint_DIR "${SLINT_ROOT}/lib/cmake/Slint")

elseif(APPLE)

    list(APPEND CMAKE_PREFIX_PATH
        "${CMAKE_SOURCE_DIR}/vendor/slint-macos/lib/cmake"
    )

else()

    list(APPEND CMAKE_PREFIX_PATH
        "${CMAKE_SOURCE_DIR}/vendor/slint-linux/lib/cmake"
    )

endif()

find_package(Slint REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(CURL REQUIRED)

add_executable(taalcha
    main.cpp

    src/db.cpp
    src/generator.cpp

    src/VaultManager.cpp
    src/VaultManagerInit.cpp
    src/VaultManagerAuth.cpp
    src/VaultManagerData.cpp
    src/VaultManagerCrypto.cpp

    src/favicon.cpp
)

target_include_directories(taalcha PRIVATE
    src
    include
)

slint_target_sources(taalcha
    src/ui/app.slint
)

target_link_libraries(taalcha PRIVATE
    Slint::Slint
    OpenSSL::SSL
    OpenSSL::Crypto
    SQLite::SQLite3
    CURL::libcurl
)

if(WIN32)

    # Copy all runtime DLLs from vcpkg
    add_custom_command(TARGET taalcha POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:taalcha>
            $<TARGET_FILE_DIR:taalcha>
        COMMAND_EXPAND_LISTS
    )

    # Copy Slint runtime DLL
    if(EXISTS "${SLINT_ROOT}/bin/slint_cpp.dll")
        add_custom_command(TARGET taalcha POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${SLINT_ROOT}/bin/slint_cpp.dll"
                "$<TARGET_FILE_DIR:taalcha>"
        )
    endif()

endif()
