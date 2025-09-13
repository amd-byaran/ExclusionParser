cmake_minimum_required(VERSION 3.20)
project(ExclusionCoverageParser VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler-specific options
if(MSVC)
    add_compile_options(/W4 /WX)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    # Enable /bigobj for large object files
    add_compile_options(/bigobj)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Include directories
include_directories(include)

# Source files
set(PARSER_SOURCES
    src/ExclusionParser.cpp
    src/ExclusionWriter.cpp
    src/ExclusionData.cpp
)

# Header files
set(PARSER_HEADERS
    include/ExclusionTypes.h
    include/ExclusionParser.h
    include/ExclusionWriter.h
    include/ExclusionData.h
)

# Static Library Target
add_library(ExclusionCoverageParser_static STATIC ${PARSER_SOURCES} ${PARSER_HEADERS})
target_include_directories(ExclusionCoverageParser_static PUBLIC include)
set_target_properties(ExclusionCoverageParser_static PROPERTIES
    OUTPUT_NAME "ExclusionCoverageParser"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

# Shared Library (DLL) Target
add_library(ExclusionCoverageParser_shared SHARED ${PARSER_SOURCES} ${PARSER_HEADERS})
target_include_directories(ExclusionCoverageParser_shared PUBLIC include)
target_compile_definitions(ExclusionCoverageParser_shared PRIVATE EXCLUSION_PARSER_EXPORTS)
set_target_properties(ExclusionCoverageParser_shared PROPERTIES
    OUTPUT_NAME "ExclusionCoverageParser"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

# Test executable
find_package(GTest QUIET)
if(GTest_FOUND)
    enable_testing()
    
    add_executable(ExclusionParserTests
        test/test_main.cpp
        test/test_parser.cpp
        test/test_writer.cpp
        test/test_data_structures.cpp
    )
    
    target_link_libraries(ExclusionParserTests 
        ExclusionCoverageParser_static
        GTest::gtest 
        GTest::gtest_main
    )
    
    target_include_directories(ExclusionParserTests PRIVATE include)
    
    add_test(NAME ExclusionParserTests COMMAND ExclusionParserTests)
else()
    message(WARNING "GoogleTest not found. Tests will not be built.")
endif()

# Install targets
install(TARGETS ExclusionCoverageParser_static ExclusionCoverageParser_shared
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES ${PARSER_HEADERS} DESTINATION include)

# Package configuration
set(CPACK_PACKAGE_NAME "ExclusionCoverageParser")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION "C++ Library for parsing and writing exclusion coverage files")
set(CPACK_PACKAGE_CONTACT "AMD")
include(CPack)