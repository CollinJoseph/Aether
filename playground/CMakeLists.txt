file(GLOB_RECURSE PLAYGROUND_SRC CONFIGURE_DEPENDS *.cpp)

add_executable(playground ${PLAYGROUND_SRC})

target_include_directories(playground PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(playground PRIVATE Aether)