add_executable(IprTest hello/IprTest.cpp)
target_link_libraries(IprTest ipr)
set_target_properties(IprTest
   PROPERTIES
      CXX_STANDARD 17
      CXX_STANDARD_REQUIRED ON
	  CXX_EXTENSIONS OFF
)

add_compile_options(-std=c++2a -Wall -g)

