#include "spring_app.hh"

SpringApp::SpringApp() : m_simulation_mtx() {
  m_window =
      std::make_unique<Window>("Spring", 1280u, 720u, glm::vec3(100.0f / 256.f, 100.0f / 256.0f, 100.0f / 256.0f));

  glfwSetWindowUserPointer(m_window->get_instance(), this);

  glfwSetWindowCloseCallback(m_window->get_instance(), SpringApp::close_callback);
  glfwSetFramebufferSizeCallback(m_window->get_instance(), SpringApp::framebuffer_resize_callback);
  glfwSetCursorPosCallback(m_window->get_instance(), SpringApp::mouse_position_callback);
  glfwSetScrollCallback(m_window->get_instance(), SpringApp::scroll_callback);

  auto res = gladLoadGL(glfwGetProcAddress);
  if (res == 0) {
    throw std::runtime_error("Failed to initialize GLAD");
  }

  m_framebuffer = std::make_unique<Framebuffer>(m_window->get_width(), m_window->get_height());

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glEnable(GL_MULTISAMPLE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(m_window->get_instance(), true);
  ImGui_ImplOpenGL3_Init("#version 460 core");

  m_ui = std::make_unique<SpringUI>(*m_window, *m_framebuffer, std::bind(&SpringApp::start_simulatiton, this),
                                    std::bind(&SpringApp::stop_simulation, this),
                                    std::bind(&SpringApp::restart_simulation, this), std::bind(&SpringApp::skip_simulation, this));
  m_dt = m_ui->get_dt();
  m_spring =
      std::make_unique<Spring>(m_ui->get_weight_mass(), m_ui->get_elasticity_coef(), m_ui->get_damping_coef(),
                               m_ui->get_weight_starting_position(), m_ui->get_weight_starting_velocity(),
                               m_ui->get_rest_position_function(), m_ui->get_field_force_function());
  m_spring_model_mat = glm::scale(glm::translate(m_weight_model_mat, glm::vec3(0.0f, 2.5f, 0.0f)), glm::vec3(1.0f, -1.0f, 1.0f));

  m_camera = std::make_unique<Camera>(glm::vec3{0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 8.0f, 1.6f, 64.0f, 45.0f,
                                      m_window->get_aspect_ratio(), 0.5f, 500.0f);
  m_spring_vertex_array = std::make_unique<VertexArray<int>>();
  m_spring_shader = std::move(ShaderProgram::load("src/spring/shaders/spring"));
  m_weight_shader = std::move(ShaderProgram::load("src/spring/shaders/weight"));
  auto vertices = std::vector<WeightVertex>{
      {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},   {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
      {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},     {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
      {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},    {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
      {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},    {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
      {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},    {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
      {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},     {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
      {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},  {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
      {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},  {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
      {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},   {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
      {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}}, {{-0.5f, -0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}},
      {{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}},   {{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}},
      {{-0.5f, 0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}},  {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}},
      {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},    {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
      {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},    {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
      {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},   {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
      {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}}, {{0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}},
      {{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}},   {{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}},
      {{-0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}},  {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}},
  };
  auto vertex_buffer = std::make_unique<Buffer<WeightVertex>>();
  vertex_buffer->bind();
  vertex_buffer->copy(vertices);
  vertex_buffer->unbind();
  m_weight_vertex_array =
      std::make_unique<VertexArray<WeightVertex>>(std::move(vertex_buffer), WeightVertex::get_vertex_attributes());
}

SpringApp::~SpringApp() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImPlot::DestroyContext();
  ImGui::DestroyContext();
}

void SpringApp::update(float dt) {
  m_window->clear();
  m_ui->update();
  m_framebuffer->bind();
  m_window->clear();
  render_visualization();
  m_framebuffer->unbind();
  m_window->update();
}

void SpringApp::close_callback(GLFWwindow* window) {
  SpringApp* app = static_cast<SpringApp*>(glfwGetWindowUserPointer(window));
  app->m_run = false;
  app->stop_simulation();
}

void SpringApp::framebuffer_resize_callback(GLFWwindow* window, int width, int height) {
  SpringApp* app = static_cast<SpringApp*>(glfwGetWindowUserPointer(window));
  app->m_window->set_size(width, height);
  app->m_framebuffer->resize(width, height);
}

void SpringApp::mouse_position_callback(GLFWwindow* window, double xpos, double ypos) {
  SpringApp* app = static_cast<SpringApp*>(glfwGetWindowUserPointer(window));
  static float prev_xpos = 0.0;
  static float prev_ypos = 0.0;
  float curr_xpos = -1.0 + xpos / static_cast<float>(app->m_window->get_width()) * 2.0;
  float curr_ypos = 1.0 - ypos / static_cast<float>(app->m_window->get_height()) * 2.0;

  if (glfwGetKey(app->m_window->get_instance(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS &&
   glfwGetMouseButton(app->m_window->get_instance(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
    app->m_camera->rotate(-glm::vec2(curr_xpos - prev_xpos, curr_ypos - prev_ypos), 0.01f);
  }

  prev_xpos = curr_xpos;
  prev_ypos = curr_ypos;
}

void SpringApp::scroll_callback(GLFWwindow* window, double, double yoffset) {
  SpringApp* app = static_cast<SpringApp*>(glfwGetWindowUserPointer(window));
  if (glfwGetKey(app->m_window->get_instance(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
    app->m_camera->zoom(-yoffset, 0.01f);
  }
}

void SpringApp::simulation_loop() {
  while (true) {
    auto beg = std::chrono::high_resolution_clock::now();
    m_spring->update(m_dt);
    m_ui->update_spring_data(m_spring->get_weight_position(), m_spring->get_weight_velocity(),
                             m_spring->get_weight_acceleration(), m_spring->get_elasticity_force(),
                             m_spring->get_damping_force(), m_spring->get_field_force(),
                             m_spring->get_rest_position(), m_spring->get_t());
    m_weight_model_mat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, m_spring->get_weight_position(), 0.0f));
    m_spring_height = 2.0f - m_spring->get_weight_position();
    {
      std::lock_guard<std::mutex> guard(m_simulation_mtx);
      if (!m_run_simulation) return;
      if (m_simulation_frames_to_skip > 0) {
        --m_simulation_frames_to_skip;
        continue;
      }
    }
    auto end = std::chrono::high_resolution_clock::now();
    float loop_runtime = std::chrono::duration<float, std::chrono::seconds::period>(beg - end).count();
    if (loop_runtime < m_dt) {
      std::this_thread::sleep_for(std::chrono::duration<float, std::chrono::seconds::period>(m_dt - loop_runtime));
    }
  }
}

void SpringApp::start_simulatiton() {
  {
    std::lock_guard<std::mutex> guard(m_simulation_mtx);
    if (m_run_simulation) return;
    m_run_simulation = true;
  }
  m_simulation_thread = std::thread(&SpringApp::simulation_loop, this);
}

void SpringApp::stop_simulation() {
  {
    std::lock_guard<std::mutex> guard(m_simulation_mtx);
    if (!m_run_simulation) return;
    m_run_simulation = false;
  }
  m_simulation_thread.join();
}

void SpringApp::restart_simulation() {
  m_spring_height = 2.0f;
  stop_simulation();
  copy_ui_data();
  start_simulatiton();
}

void SpringApp::skip_simulation() {
  {
    std::lock_guard<std::mutex> guard(m_simulation_mtx);
    m_simulation_frames_to_skip = m_ui->get_frames_to_skip();
  } 
}

void SpringApp::copy_ui_data() {
  m_dt = m_ui->get_dt();
  m_spring->weight_mass = m_ui->get_weight_mass();
  m_spring->damping_coef = m_ui->get_damping_coef();
  m_spring->elasticity_coef = m_ui->get_elasticity_coef();
  m_spring->rest_position_function = std::move(m_ui->get_rest_position_function());
  m_spring->field_force_function = std::move(m_ui->get_field_force_function());
  m_spring->reset(m_ui->get_weight_starting_position(), m_ui->get_weight_starting_velocity());
}

void SpringApp::render_visualization() {
  m_spring_shader->bind();
  m_spring_shader->set_uniform_value("model", m_spring_model_mat);
  m_spring_shader->set_uniform_value("view", m_camera->get_view_matrix());
  m_spring_shader->set_uniform_value("projection", m_camera->get_projection_matrix());
  m_spring_shader->set_uniform_value("h0", m_spring_height);
  m_spring_shader->set_uniform_value("l", 10.0f);
  m_spring_shader->set_uniform_value("r", 0.2f);
  m_spring_shader->set_uniform_value("rsmall", 0.05f);
  m_spring_shader->set_uniform_value("count", 400.0f);
  m_spring_vertex_array->bind();
  glDrawArrays(GL_LINE_STRIP, 0, 400);
  glCheckError();
  m_weight_shader->bind();
  m_weight_shader->set_uniform_value("model", m_weight_model_mat);
  m_weight_shader->set_uniform_value("view", m_camera->get_view_matrix());
  m_weight_shader->set_uniform_value("projection", m_camera->get_projection_matrix());
  m_weight_vertex_array->bind();
  glDrawArrays(GL_TRIANGLES, 0, m_weight_vertex_array->get_draw_size());
  m_weight_vertex_array->unbind();
  m_weight_shader->unbind();
}