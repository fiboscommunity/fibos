cmake_minimum_required( VERSION 3.8 )

set(name fibos)
project(${name})
set( CMAKE_CXX_STANDARD 20 )

include(${CMAKE_CURRENT_LIST_DIR}/vender/fibjs/vender/build_tools/cmake/option.cmake)
include(${PROJECT_SOURCE_DIR}/vender/fibjs/vender/v8/cmake/options.cmake)

file(GLOB_RECURSE src_list "src/*.c*")

set( fc_list
     vender/eos/libraries/libfc/src/uint128.cpp
     vender/eos/libraries/libfc/src/variant.cpp
     vender/eos/libraries/libfc/src/exception.cpp
     vender/eos/libraries/libfc/src/variant_object.cpp
     vender/eos/libraries/libfc/src/string.cpp
     vender/eos/libraries/libfc/src/time.cpp
     vender/eos/libraries/libfc/src/mock_time.cpp
     vender/eos/libraries/libfc/src/utf8.cpp
     vender/eos/libraries/libfc/src/io/datastream.cpp
    #  vender/eos/libraries/libfc/src/io/json.cpp
     vender/eos/libraries/libfc/src/io/varint.cpp
     vender/eos/libraries/libfc/src/io/fstream.cpp
     vender/eos/libraries/libfc/src/io/console.cpp
     vender/eos/libraries/libfc/src/filesystem.cpp
     vender/eos/libraries/libfc/src/interprocess/file_mapping.cpp
     vender/eos/libraries/libfc/src/log/log_message.cpp
     vender/eos/libraries/libfc/src/log/logger.cpp
     vender/eos/libraries/libfc/src/log/appender.cpp
     vender/eos/libraries/libfc/src/log/console_appender.cpp
     vender/eos/libraries/libfc/src/log/dmlog_appender.cpp
     vender/eos/libraries/libfc/src/log/logger_config.cpp
     vender/eos/libraries/libfc/src/crypto/_digest_common.cpp
     vender/eos/libraries/libfc/src/crypto/aes.cpp
     vender/eos/libraries/libfc/src/crypto/crc.cpp
     vender/eos/libraries/libfc/src/crypto/city.cpp
     vender/eos/libraries/libfc/src/crypto/base58.cpp
     vender/eos/libraries/libfc/src/crypto/base64.cpp
     vender/eos/libraries/libfc/src/crypto/bigint.cpp
     vender/eos/libraries/libfc/src/crypto/hex.cpp
     vender/eos/libraries/libfc/src/crypto/sha1.cpp
     vender/eos/libraries/libfc/src/crypto/sha3.cpp
     vender/eos/libraries/libfc/src/crypto/ripemd160.cpp
     vender/eos/libraries/libfc/src/crypto/sha256.cpp
     vender/eos/libraries/libfc/src/crypto/sha224.cpp
     vender/eos/libraries/libfc/src/crypto/sha512.cpp
     vender/eos/libraries/libfc/src/crypto/elliptic_common.cpp
     vender/eos/libraries/libfc/src/crypto/elliptic_impl_priv.cpp
     vender/eos/libraries/libfc/src/crypto/elliptic_secp256k1.cpp
     vender/eos/libraries/libfc/src/crypto/elliptic_r1.cpp
     vender/eos/libraries/libfc/src/crypto/elliptic_webauthn.cpp
     vender/eos/libraries/libfc/src/crypto/rand.cpp
    #  vender/eos/libraries/libfc/src/crypto/public_key.cpp
     vender/eos/libraries/libfc/src/crypto/private_key.cpp
     vender/eos/libraries/libfc/src/crypto/signature.cpp
     vender/eos/libraries/libfc/src/crypto/modular_arithmetic.cpp
     vender/eos/libraries/libfc/src/crypto/blake2.cpp
     vender/eos/libraries/libfc/src/crypto/k1_recover.cpp
     vender/eos/libraries/libfc/src/network/url.cpp
     vender/eos/libraries/libfc/src/network/http/http_client.cpp
     vender/eos/libraries/libfc/src/compress/zlib.cpp
     vender/eos/libraries/libfc/src/log/gelf_appender.cpp
)

add_executable(${name} ${src_list} ${fc_list})

set(FIBJS_DIR ${PROJECT_SOURCE_DIR}/vender/fibjs/bin/${BUILD_OS}_${BUILD_ARCH}_${BUILD_TYPE})
set(LIBC_DIR ${PROJECT_SOURCE_DIR}/vender/libc/${BUILD_ARCH})

set(EOS_DIR ${PROJECT_SOURCE_DIR}/vender/eos)
set(EOS_BUILD_DIR ${EOS_DIR}/build/${BUILD_OS}_${BUILD_TYPE})

add_definitions(-DEOSIO_EOS_VM_RUNTIME_ENABLED)

include_directories("${PROJECT_SOURCE_DIR}/include"
	"${PROJECT_SOURCE_DIR}/vender/fibjs/fibjs/include"
	"${PROJECT_SOURCE_DIR}/vender/fibjs/vender"
	"${PROJECT_SOURCE_DIR}/vender/fibjs/vender/uv/include"
	"${PROJECT_SOURCE_DIR}/vender/fibjs/vender/openssl/include"
	"${CMAKE_CURRENT_BINARY_DIR}")

MACRO(include_dirs dir)
	file(GLOB _libs "${dir}/*")
	foreach(eos_lib ${_libs})
		if(IS_DIRECTORY "${eos_lib}/include")
			include_directories("${eos_lib}/include")
		endif()
	endforeach()
ENDMACRO()

include_dirs("${EOS_DIR}/plugins")
include_dirs("${EOS_DIR}/libraries")

include_directories("${EOS_DIR}/libraries")
include_directories("${EOS_DIR}/plugins")
include_directories("${EOS_DIR}/libraries/builtins")
include_directories("${EOS_DIR}/libraries/softfloat/source/include")
include_directories("${EOS_DIR}/libraries/libfc/secp256k1/secp256k1/include")
include_directories("${PROJECT_SOURCE_DIR}/vender/gmp/build/${BUILD_OS}_${BUILD_TYPE}")
include_directories("${EOS_DIR}/libraries/libfc/libraries/bls12-381/include")
include_directories("${EOS_DIR}/libraries/wasm-jit/Include")
include_directories("${EOS_BUILD_DIR}/programs")
include_directories("${EOS_BUILD_DIR}/libraries/chain/include")

MACRO(link_libs dir)
	file(GLOB_RECURSE _libs "${dir}/*.a")
	foreach(eos_lib ${_libs})
		if(NOT eos_lib MATCHES "boringssl" AND NOT eos_lib MATCHES "libfc.a")
			target_link_libraries(${name} ${eos_lib})
		endif()
	endforeach()
ENDMACRO()

link_libs("${EOS_BUILD_DIR}/plugins")
link_libs("${EOS_BUILD_DIR}/libraries")
target_link_libraries(${name} "${EOS_BUILD_DIR}/libraries/builtins/libbuiltins.a")

include_dirs("${EOS_DIR}/libraries/boost/libs")
include_directories("${EOS_DIR}/libraries/boost/libs/numeric/conversion/include")

link_libs("${EOS_BUILD_DIR}/libraries/boost/libs")

target_link_libraries(${name} ${PROJECT_SOURCE_DIR}/vender/gmp/build/${BUILD_OS}_${BUILD_TYPE}/.libs/libgmp.a)

include(${CMAKE_CURRENT_LIST_DIR}/vender/fibjs/vender/libs.cmake)
set(libs fibjs ${libs})

foreach(lib ${libs})
	target_link_libraries(${name} "${FIBJS_DIR}/lib${lib}.a")
endforeach()

if(${BUILD_OS} STREQUAL "Darwin")
     find_library(localauthentication_framework LocalAuthentication)
     find_library(corefoundation_framework CoreFoundation)
     find_library(cocoa_framework Cocoa)
     find_library(security_framework Security)
	set(link_flags "${link_flags} -nostdlib")
	target_link_libraries(${name}
		${LIBC_DIR}/libc++.a
		${LIBC_DIR}/libc++abi.a
		${LIBC_DIR}/libunwind.a ${localauthentication_framework} ${corefoundation_framework} ${cocoa_framework} ${security_framework})
else()
	set(link_flags "${link_flags} -rdynamic")
endif()

setup_result_library(${name})
