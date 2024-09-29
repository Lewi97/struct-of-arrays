
project("SoAUnitTests")

add_executable(${PROJECT_NAME} "UnitTests.cpp" "UnitTests.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
	set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)
endif()

enable_testing()

find_package(GTest CONFIG REQUIRED)

target_include_directories(${PROJECT_NAME}
	PRIVATE ../
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE GTest::gtest_main 
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})