cmake_minimum_required(
   VERSION 3.5
)

enable_testing()

project(ipr
   VERSION 0.47
   LANGUAGES CXX
)

add_library(${PROJECT_NAME}
   src/interface.cxx
   src/impl.cxx
   src/io.cxx
   src/traversal.cxx
   src/utility.cxx
)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "No build type selected, default to Debug")
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type (default Debug)" FORCE)
endif()

add_subdirectory(test)

target_include_directories(${PROJECT_NAME}
   PUBLIC
      ${PROJECT_SOURCE_DIR}/include
)

set_target_properties(${PROJECT_NAME}
   PROPERTIES
      CXX_STANDARD 17
      CXX_STANDARD_REQUIRED ON
	  CXX_EXTENSIONS OFF
)

add_compile_options(
   $<$<CXX_COMPILER_ID:MSVC>:
      /wd4267       # 'conversion' conversion from 'size_t' to 'int', possible loss of data
      /wd4244       # 'conversion' conversion from '__int64' to 'int', possible loss of data
      /permissive-  # Turn on strict language conformance
      /EHsc         # Turn on exception handling semantics
   >
   $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:
      -Wall         # Turn on all useful warnings
      -pedantic     # Turn on strict language conformance
   >
   $<$<CXX_COMPILER_ID:Clang>:
      -Wno-overloaded-virtual                   # Too many false positives
      -Wno-delete-non-abstract-non-virtual-dtor # System headers plagued
   >
)

target_compile_options(${PROJECT_NAME}
   PRIVATE
      $<$<CXX_COMPILER_ID:MSVC>:
         /wd4267      # 'conversion' conversion from 'size_t' to 'int', possible loss of data
         /wd4244      # 'conversion' conversion from '__int64' to 'int', possible loss of data
         /permissive-  # Turn on strict language conformance
         /EHsc         # Turn on exception handling semantics
      >
      $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:
         -Wall         # Turn on all useful warnings
         -pedantic     # Turn on strict language conformance
      >
      $<$<CXX_COMPILER_ID:Clang>:
		 -Wno-overloaded-virtual                   # Too many false positives
		 -Wno-delete-non-abstract-non-virtual-dtor # System headers plagued
      >
)

target_compile_definitions(${PROJECT_NAME}
   PRIVATE
	  $<$<PLATFORM_ID:UNIX>:
		 _FILE_OFFSET_BITS=64 # We want the ability to process large files.
	  >
)

install(
   TARGETS ipr
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)
install(
   DIRECTORY include/ipr
   DESTINATION include
)
