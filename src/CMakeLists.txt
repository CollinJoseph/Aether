file(GLOB_RECURSE AETHER_SRC CONFIGURE_DEPENDS *.cpp)
file(GLOB_RECURSE AETHER_HEADERS CONFIGURE_DEPENDS *.h)

add_library(Aether STATIC ${AETHER_SRC} ${AETHER_HEADERS})

target_include_directories(Aether PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/include
)

# TODO : Add external dependencies
# Example external dependency
# target_link_libraries(Engine PUBLIC SDL2)