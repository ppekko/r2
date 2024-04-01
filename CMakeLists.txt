cmake_minimum_required(VERSION 3.5)
project(r2)


find_package(SDL2 REQUIRED)
find_package(Freetype REQUIRED)


set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -fomit-frame-pointer -ffile-prefix-map='${CMAKE_CURRENT_SOURCE_DIR}/src/sources/'=")

file(GLOB PROJECT_HEADERS src/headers/*.h)
file(GLOB PROJECT_SOURCES src/sources/*.c)

source_group("Headers" FILES ${PROJECT_HEADERS})
source_group("Sources" FILES ${PROJECT_SOURCES})

add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${VENDORS_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE 
        src/headers/
        ${FREETYPE_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
        m
        SDL2::SDL2
        ${FREETYPE_LIBRARIES}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE 
        -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\"
)

add_executable(demo demo/main.c)

target_link_libraries(demo PRIVATE ${PROJECT_NAME})

target_include_directories(demo PRIVATE 
        ${CMAKE_SOURCE_DIR}/src/headers
        ${CMAKE_BINARY_DIR}/src/vend/glad/include
)

file(COPY ${CMAKE_SOURCE_DIR}/demo/rsc DESTINATION ${CMAKE_BINARY_DIR})

install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static
)
