#include "spring_app.hh"

SpringApp::SpringApp() : m_simulation_mtx() {
  m_window =
      std::make_unique<Window>("Spring", 1280u, 720u, glm::vec3(100.0f / 256.f, 100.0f / 256.0f, 100.0f / 256.0f));

  glfwSetWindowUserPointer(m_window->get_instance(), this);

  glfwSetWindowCloseCallback(m_window->get_instance(), SpringApp::close_callback);
  glfwSetFramebufferSizeCallback(m_window->get_instance(), SpringApp::framebuffer_resize_callback);

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
                                    std::bind(&SpringApp::restart_simulation, this));
  m_dt = m_ui->get_dt();
  m_spring =
      std::make_unique<Spring>(6.0f, m_ui->get_weight_mass(), m_ui->get_elasticity_coef(), m_ui->get_damping_coef(),
                               m_ui->get_weight_starting_position(), m_ui->get_weight_starting_velocity(),
                               m_ui->get_anchor_position_function(), m_ui->get_field_force_function());

  m_shader = std::move(ShaderProgram::load("src/spring/shaders/basic"));
  auto vertices = std::vector<SpringVertex>{{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
                                            {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
                                            {{0.0f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}};
  auto vertex_buffer = std::make_unique<Buffer<SpringVertex>>();
  vertex_buffer->bind();
  vertex_buffer->copy(vertices);
  vertex_buffer->unbind();
  m_vertex_array =
      std::make_unique<VertexArray<SpringVertex>>(std::move(vertex_buffer), SpringVertex::get_vertex_attributes());
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

void SpringApp::simulation_loop() {
  while (true) {
    auto beg = std::chrono::high_resolution_clock::now();
    m_spring->update(m_dt);
    m_ui->update_spring_data(m_spring->get_weight_position(), m_spring->get_weight_velocity(),
                             m_spring->get_weight_acceleration(), m_spring->get_elasticity_force(),
                             m_spring->get_damping_force(), m_spring->get_field_force(),
                             m_spring->get_anchor_position(), m_spring->get_t());
    {
      std::lock_guard<std::mutex> guard(m_simulation_mtx);
      if (!m_run_simulation) return;
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
  stop_simulation();
  copy_ui_data();
  start_simulatiton();
}

void SpringApp::copy_ui_data() {
  m_dt = m_ui->get_dt();
  m_spring->weight_mass = m_ui->get_weight_mass();
  m_spring->damping_coef = m_ui->get_damping_coef();
  m_spring->elasticity_coef = m_ui->get_elasticity_coef();
  m_spring->anchor_position_function = std::move(m_ui->get_anchor_position_function());
  m_spring->field_force_function = std::move(m_ui->get_field_force_function());
  m_spring->reset(m_ui->get_weight_starting_position(), m_ui->get_weight_starting_velocity());
}

void SpringApp::render_visualization() {
  m_shader->bind();
  m_vertex_array->bind();
  glDrawArrays(GL_TRIANGLES, 0, m_vertex_array->get_draw_size());
  m_vertex_array->unbind();
  m_shader->unbind();
}