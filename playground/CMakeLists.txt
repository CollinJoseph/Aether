file(GLOB_RECURSE PLAYGROUND_SRC CONFIGURE_DEPENDS *.cpp)

add_executable(playground ${PLAYGROUND_SRC})

target_link_libraries(playground PRIVATE Aether)