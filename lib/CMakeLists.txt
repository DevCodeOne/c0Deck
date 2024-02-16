set(SOURCES_LIB
        config.h
        config.cpp
        json_utils.h
    )

# Qt6
# qt_add_library(c0deckLib STATIC ${SOURCES_LIB})
add_library(c0deckLib STATIC ${SOURCES_LIB})
target_link_libraries(c0deckLib PUBLIC nlohmann_json spdlog)
target_include_directories(c0deckLib PUBLIC .)

add_library(proto-objects OBJECT "${CMAKE_CURRENT_LIST_DIR}/grpc/service.proto")
target_link_libraries(proto-objects PUBLIC protobuf::libprotobuf gRPC::grpc++_unsecure gRPC::grpc++_reflection)

# Without this the folder doesn't exist and the build fails
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(PROTO_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(PROTO_IMPORT_DIRS "${CMAKE_CURRENT_LIST_DIR}/grpc")

protobuf_generate(
    TARGET proto-objects
    OUT_VAR PROTO_GENERATED_FILES
    IMPORT_DIRS ${PROTO_IMPORT_DIRS}
    PROTOC_OUT_DIR "${PROTO_BINARY_DIR}"
)
set_source_files_properties(${PROTO_GENERATED_FILES} PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON)

protobuf_generate(
    TARGET proto-objects
    OUT_VAR PROTO_GENERATED_FILES
    LANGUAGE grpc
    GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
    PLUGIN "protoc-gen-grpc=\$<TARGET_FILE:gRPC::grpc_cpp_plugin>"
    # PLUGIN_OPTIONS "generate_mock_code=true"
    IMPORT_DIRS ${PROTO_IMPORT_DIRS}
    PROTOC_OUT_DIR "${PROTO_BINARY_DIR}"
)
set_source_files_properties(${PROTO_GENERATED_FILES} PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON)
target_include_directories(proto-objects PUBLIC "$<BUILD_INTERFACE:${PROTO_BINARY_DIR}>")