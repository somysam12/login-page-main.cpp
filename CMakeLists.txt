cmake_minimum_required(VERSION 3.10)
project(BR_MODS_EXTERNAL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_INTEGRITY_CHECK "Enable binary integrity verification" OFF)

if(ENABLE_INTEGRITY_CHECK)
    add_definitions(-DENABLE_INTEGRITY_CHECK)
endif()

find_package(OpenGL REQUIRED)
find_package(GLFW3 REQUIRED)
find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)

include_directories(
    ${OPENGL_INCLUDE_DIRS}
    ${GLFW3_INCLUDE_DIRS}
    ${CURL_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/imgui
    ${CMAKE_SOURCE_DIR}/imgui/backends
)

set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/imgui)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

add_executable(${PROJECT_NAME}
    main.cpp
    auth_handler.cpp
    http_client.cpp
    ${IMGUI_SOURCES}
)

target_link_libraries(${PROJECT_NAME}
    OpenGL::GL
    glfw
    ${CURL_LIBRARIES}
    OpenSSL::SSL
    OpenSSL::Crypto
    ${CMAKE_DL_LIBS}
)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} ws2_32)
endif()

if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} pthread)
endif()
