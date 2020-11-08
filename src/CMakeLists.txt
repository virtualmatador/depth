cmake_minimum_required(VERSION 3.13)
enable_testing()

file(GLOB sources "./*.cpp")
add_executable("${CMAKE_PROJECT_NAME}" ${sources})
set_property(TARGET "${CMAKE_PROJECT_NAME}" PROPERTY CXX_STANDARD 17)
target_link_libraries("${CMAKE_PROJECT_NAME}" stereogram png pthread)
install(TARGETS "${CMAKE_PROJECT_NAME}" DESTINATION "bin")
