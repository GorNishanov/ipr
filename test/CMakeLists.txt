add_executable(IprTest hello/IprTest.cpp)
add_executable(SemaTest sema/SemaTest.cpp)
add_executable(TemplTest templ/TemplTest.cpp)

target_link_libraries(SemaTest ipr)
set_target_properties(SemaTest
   PROPERTIES
      CXX_STANDARD 17
      CXX_STANDARD_REQUIRED ON
	  CXX_EXTENSIONS OFF
)

target_link_libraries(IprTest ipr)
set_target_properties(IprTest
   PROPERTIES
      CXX_STANDARD 17
      CXX_STANDARD_REQUIRED ON
	  CXX_EXTENSIONS OFF
)

target_link_libraries(TemplTest ipr)
set_target_properties(TemplTest
   PROPERTIES
      CXX_STANDARD 17
      CXX_STANDARD_REQUIRED ON
	  CXX_EXTENSIONS OFF
)
