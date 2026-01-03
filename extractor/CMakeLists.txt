project(PkgExtractor)
cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ZLIB 1.3 MODULE)

include_directories(../src)
include_directories(../externals)

add_definitions(-DNTDDI_VERSION=0x0A000006 -D_WIN32_WINNT=0x0A00 -DWINVER=0x0A00)

add_executable(pkg_extractor main.cpp
	../src/core/loader.cpp
	../src/core/file_format/pkg.cpp
	../src/core/file_format/psf.cpp
	../src/core/file_format/pkg_type.cpp
	../src/core/crypto/crypto.cpp
	../src/common/io_file.cpp
	../src/common/assert.cpp
	../src/common/error.cpp
	../src/common/path_util.cpp
	../src/common/string_util.cpp)

add_library(libcryptopp STATIC IMPORTED)
set_property(TARGET libcryptopp PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/cryptopp/cryptopp/libcryptopp.a")

target_link_libraries(pkg_extractor PRIVATE libcryptopp)
target_link_libraries(pkg_extractor PRIVATE ZLIB::ZLIB)