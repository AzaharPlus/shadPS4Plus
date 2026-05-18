cmake_minimum_required(VERSION 3.16)
project(PkgExtractor)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ZLIB REQUIRED)

set(CRYPTOPP_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../externals/cryptopp" CACHE PATH
    "Path to the Crypto++ source tree")
set(CRYPTOPP_LIBRARY "" CACHE FILEPATH "Path to the Crypto++ static library")

if(NOT EXISTS "${CRYPTOPP_ROOT}/cryptlib.h")
    message(FATAL_ERROR
        "Crypto++ headers were not found at ${CRYPTOPP_ROOT}. "
        "Run git submodule update --init --recursive or set CRYPTOPP_ROOT.")
endif()

if(CRYPTOPP_LIBRARY)
    if(NOT EXISTS "${CRYPTOPP_LIBRARY}")
        message(FATAL_ERROR "CRYPTOPP_LIBRARY does not exist: ${CRYPTOPP_LIBRARY}")
    endif()
    set(CRYPTOPP_LIBRARY_PATH "${CRYPTOPP_LIBRARY}")
else()
    find_library(CRYPTOPP_LIBRARY_PATH
        NAMES cryptopp libcryptopp cryptlib
        PATHS
            "${CRYPTOPP_ROOT}"
            "${CMAKE_CURRENT_BINARY_DIR}/cryptopp/cryptopp"
        NO_DEFAULT_PATH
    )
endif()

if(NOT CRYPTOPP_LIBRARY_PATH)
    find_program(CRYPTOPP_MAKE_EXECUTABLE NAMES gmake make)
    if(NOT CRYPTOPP_MAKE_EXECUTABLE)
        message(FATAL_ERROR
            "Crypto++ static library was not found. Build libcryptopp.a manually "
            "or set CRYPTOPP_LIBRARY.")
    endif()

    set(CRYPTOPP_LIBRARY_PATH "${CRYPTOPP_ROOT}/libcryptopp.a")
    add_custom_command(
        OUTPUT "${CRYPTOPP_LIBRARY_PATH}"
        COMMAND "${CRYPTOPP_MAKE_EXECUTABLE}" libcryptopp.a
        WORKING_DIRECTORY "${CRYPTOPP_ROOT}"
        COMMENT "Building Crypto++ static library"
        VERBATIM
    )

    add_custom_target(cryptopp_static DEPENDS "${CRYPTOPP_LIBRARY_PATH}")
endif()

add_library(CryptoPP::cryptopp STATIC IMPORTED GLOBAL)
set_target_properties(CryptoPP::cryptopp PROPERTIES
    IMPORTED_LOCATION "${CRYPTOPP_LIBRARY_PATH}"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/../externals"
)

add_executable(pkg_extractor
    main.cpp

    ../src/core/loader.cpp
    ../src/core/file_format/pkg.cpp
    ../src/core/file_format/psf.cpp
    ../src/core/file_format/pkg_type.cpp
    ../src/core/crypto/crypto.cpp

    ../src/common/io_file.cpp
    ../src/common/assert.cpp
    ../src/common/error.cpp
    ../src/common/path_util.cpp
    ../src/common/string_util.cpp
)

target_include_directories(pkg_extractor PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../src"
)

target_compile_definitions(pkg_extractor PRIVATE
    $<$<PLATFORM_ID:Windows>:NTDDI_VERSION=0x0A000006>
    $<$<PLATFORM_ID:Windows>:_WIN32_WINNT=0x0A00>
    $<$<PLATFORM_ID:Windows>:WINVER=0x0A00>
)

if(TARGET cryptopp_static)
    add_dependencies(pkg_extractor cryptopp_static)
endif()

target_link_libraries(pkg_extractor PRIVATE
    CryptoPP::cryptopp
    ZLIB::ZLIB
)
