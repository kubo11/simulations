# glad
add_subdirectory(glad/cmake)
set(OPENGL_EXTENSIONS GL_ARB_shader_storage_buffer_object)
glad_add_library(glad REPRODUCIBLE EXCLUDE_FROM_ALL LOADER API gl:core=4.6 EXTENSIONS ${OPENGL_EXTENSIONS})

# glfw
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
add_subdirectory(glfw)

# glm
add_subdirectory(glm)

# imgui & implot
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
SET(IMPLOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/implot)
file(GLOB IMGUI_SOURCES ${IMGUI_DIR}/*.cpp ${IMGUI_DIR}/*.h)
file(GLOB IMPLOT_SOURCES ${IMPLOT_DIR}/*.cpp ${IMPLOT_DIR}/*.h)
add_library(
    imgui
    ${IMGUI_SOURCES}
    ${IMPLOT_SOURCES}
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.h
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.h
    ${IMGUI_DIR}/misc/cpp/imgui_stdlib.cpp
    ${IMGUI_DIR}/misc/cpp/imgui_stdlib.h
)

target_link_libraries(imgui PUBLIC glad)
target_link_libraries(imgui PUBLIC glfw)
target_include_directories(
    imgui
    PUBLIC
    ${IMGUI_DIR}
    ${IMPLOT_DIR}
    ${IMGUI_DIR}/backends
)

# ImGuiFileDialog
add_subdirectory(imguifiledialog)
target_link_libraries(ImGuiFileDialog imgui)

# stb image
set(STB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/stb)
set(STB_SOURCES ${STB_DIR}/stb_image.h)
add_library(stb ${STB_SOURCES})
set_target_properties(stb PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(stb PUBLIC ${STB_DIR})

# tinyobjloader
add_subdirectory(tinyobjloader)