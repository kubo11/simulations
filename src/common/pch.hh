#ifndef SIMULATIONS_COMMON_PCH
#define SIMULATIONS_COMMON_PCH

#include <array>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <concepts>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <random>
#include <ranges>
#include <set>
#include <stdexcept>
#include <string>
#include <thread>
#include <tuple>
#include <queue>
#include <unordered_map>
#include <variant>
#include <vector>

namespace fs = std::filesystem;

// glad
#include <glad/gl.h>

// glfw
#include <GLFW/glfw3.h>

// glm
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/scalar_constants.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/euler_angles.hpp>

// imgui
#define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>
#include <misc/cpp/imgui_stdlib.h>
#include <ImGuiFileDialog.h>

// tinyobjloader
#include <tiny_obj_loader.h>

// simulations
#include "utils.hh"

#endif  // SIMULATIONS_COMMON_PCH