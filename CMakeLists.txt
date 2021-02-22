cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
include(FetchContent)
set(CMAKE_CXX_STANDARD 11)
project (draco_c C CXX)

FetchContent_Declare(
    draco
    GIT_REPOSITORY https://github.com/google/draco.git
    GIT_TAG        master
)

FetchContent_GetProperties(draco)
if(NOT draco_POPULATED)
    FetchContent_Populate(draco)
    set(BUILD_SHARED_LIBS_SAVED "${BUILD_SHARED_LIBS}")
    set(BUILD_SHARED_LIBS OFF)
    add_subdirectory(${draco_SOURCE_DIR} ${draco_BINARY_DIR} EXCLUDE_FROM_ALL)
    set(BUILD_SHARED_LIBS "${BUILD_SHARED_LIBS_SAVED}")
endif()

list(APPEND draco_c_sources
    "${CMAKE_CURRENT_SOURCE_DIR}/src/decoder.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/mesh.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/point_attribute.cc"
)

list(APPEND draco_c_public_api "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(draco_INCLUDE_DIRS "${draco_SOURCE_DIR}/src" "${CMAKE_BINARY_DIR}")


set(CMAKE_POSITION_INDEPENDENT_CODE ON)
if(BUILD_SHARED_LIBS)
    set(draco_lib_type SHARED)
else()
    set(draco_lib_type STATIC)
endif()

add_library(draco_c ${draco_lib_type} ${draco_c_sources})
target_include_directories(draco_c PRIVATE ${draco_INCLUDE_DIRS})
target_include_directories(draco_c PUBLIC ${draco_c_public_api})
if(BUILD_SHARED_LIBS)
    target_compile_definitions(draco_c PRIVATE "DRACO_C_BUILDING_DLL=1")
    set(link_type PRIVATE)
    set_target_properties(draco_c PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS FALSE)
    set_target_properties(draco_c PROPERTIES C_VISIBILITY_PRESET hidden)
    set_target_properties(draco_c PROPERTIES CXX_VISIBILITY_PRESET hidden)
    set_target_properties(draco_c PROPERTIES VISIBILITY_INLINES_HIDDEN ON)
else()
    target_compile_definitions(draco_c PRIVATE "DRACO_C_STATIC=1")
    set(link_type PUBLIC)
endif()

set(draco_object_library_deps
      draco_attributes
      draco_compression_attributes_dec
      draco_compression_attributes_enc
      draco_compression_attributes_pred_schemes_dec
      draco_compression_attributes_pred_schemes_enc
      draco_compression_bit_coders
      draco_compression_decode
      draco_compression_encode
      draco_compression_entropy
      draco_compression_mesh_dec
      draco_compression_mesh_enc
      draco_compression_point_cloud_dec
      draco_compression_point_cloud_enc
      draco_core
      draco_dec_config
      draco_enc_config
      draco_io
      draco_mesh
      draco_metadata
      draco_metadata_dec
      draco_metadata_enc
      draco_animation
      draco_animation_dec
      draco_animation_enc
      draco_point_cloud
      draco_points_dec
      draco_points_enc)

target_link_libraries(draco_c PRIVATE ${draco_object_library_deps})

if(DRACO_C_TESTS)
    enable_testing()
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.10.0
    )
    FetchContent_GetProperties(googletest)
    if(NOT googletest_POPULATED)
        FetchContent_Populate(googletest)
        add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif()
    
    set(SRCS_UNITTEST
        "${CMAKE_CURRENT_SOURCE_DIR}/src/c_api_test.cc"
    )

    add_executable(test_draco_c ${SRCS_UNITTEST})

    target_include_directories(test_draco_c PRIVATE
        "${draco_INCLUDE_DIRS}"
	    "${gtest_SOURCE_DIR}/include"
    )

    target_link_libraries(test_draco_c PRIVATE draco_c gtest gtest_main)

    target_compile_definitions(test_draco_c PRIVATE "DRACO_C_STATIC=1")
    add_definitions(-DDRACO_C_TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/testdata/")

    add_test(test_draco_c ${CMAKE_CURRENT_BINARY_DIR}/test_draco_c)
endif()