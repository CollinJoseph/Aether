find_program(GLSLC_EXECUTABLE glslc)

set(SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
set(SHADER_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders")

set(SPIRV_OUTPUTS "")

file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR})

file(GLOB_RECURSE SHADERS "${SHADER_SOURCE_DIR}/*.vert" "${SHADER_SOURCE_DIR}/*.frag")

foreach (SHADER ${SHADERS})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    set(OUTPUT_SPIRV "${SHADER_OUTPUT_DIR}/${SHADER_NAME}.spv")

    list(APPEND SPIRV_OUTPUTS ${OUTPUT_SPIRV})
    add_custom_command(
            OUTPUT ${OUTPUT_SPIRV}
            COMMAND ${GLSLC_EXECUTABLE} ${SHADER} -o ${OUTPUT_SPIRV}
            DEPENDS ${SHADER}
            COMMENT "Compiling ${SHADER_NAME}"
    )
endforeach ()

add_custom_target(compile_shaders DEPENDS ${SPIRV_OUTPUTS})

