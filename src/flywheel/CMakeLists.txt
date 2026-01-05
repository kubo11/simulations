set(SIMULATION_NAME "flywheel")

file(GLOB_RECURSE SIMULATION_SOURCES ${PROJECT_SOURCE_DIR}/src/${SIMULATION_NAME}/*.cc)
file(GLOB_RECURSE COMMON_SOURCES ${PROJECT_SOURCE_DIR}/src/common/*.cc)

add_executable(${SIMULATION_NAME} ${SIMULATION_SOURCES} ${COMMON_SOURCES})

target_include_directories(${SIMULATION_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src/${SIMULATION_NAME} ${PROJECT_SOURCE_DIR}/src/common)

target_precompile_headers(${SIMULATION_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src/common/pch.hh)

target_link_libraries(${SIMULATION_NAME} PRIVATE glad glfw glm imgui stb tinyobjloader ImGuiFileDialog)

set(BIN_NAME ${SIMULATION_NAME})
set(BIN_PATH ${CMAKE_CURRENT_BINARY_DIR})

if (WIN32)
        set(BIN_NAME ${BIN_NAME}.exe)
endif ()

add_custom_target(${SIMULATION_NAME}_copy_binary ALL
        COMMAND ${CMAKE_COMMAND} -E copy ${BIN_PATH}/${BIN_NAME} ${BUILD_ROOT}/${BIN_NAME})

add_dependencies(${SIMULATION_NAME}_copy_binary ${SIMULATION_NAME})