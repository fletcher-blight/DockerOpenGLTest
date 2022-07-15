cmake_minimum_required(VERSION 3.22)
project(DockerOpenGLTest)

option(ENABLE_CONAN "Automatically install conan dependencies" ON)
if (${ENABLE_CONAN})
    include(config/conan.cmake)
endif()

add_subdirectory(src)