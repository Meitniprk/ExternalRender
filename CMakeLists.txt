cmake_minimum_required(VERSION 3.24)
project(ExternalRender)

set(CMAKE_CXX_STANDARD 20)
set(BUILD_USE_64BITS on)

add_definitions(-DUNICODE -D_UNICODE)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

if (MSVC)
    add_compile_options(/MP)
endif()

add_library(DependenciesLib OBJECT "")
set_target_properties(DependenciesLib PROPERTIES CXX_VISIBILITY_PRESET hidden C_VISIBILITY_PRESET hidden)

add_subdirectory(Dependencies)
set_target_properties(DependenciesLib PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(DependenciesLib PUBLIC "${CMAKE_SOURCE_DIR}/Dependencies")
target_link_libraries(DependenciesLib PRIVATE ${CMAKE_DL_LIBS})

add_executable(ExternalRender Source/main.cpp Source/Window.cpp Source/Window.h Source/Render.cpp Source/Render.h Source/Network.cpp Source/Network.h)

target_link_libraries(ExternalRender PRIVATE DependenciesLib d3d11.lib Ws2_32.lib Mswsock.lib AdvApi32.lib dwmapi.lib)
