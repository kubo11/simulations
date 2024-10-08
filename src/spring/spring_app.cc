#include "spring_app.hh"

SpringApp::SpringApp() {
  m_window =
      std::make_unique<Window>("Spring", 1280u, 720u, glm::vec3(100.0f / 256.f, 100.0f / 256.0f, 100.0f / 256.0f));

  glfwSetWindowUserPointer(m_window->get_instance(), this);

  glfwSetWindowCloseCallback(m_window->get_instance(), SpringApp::close_callback);
  glfwSetFramebufferSizeCallback(m_window->get_instance(), SpringApp::framebuffer_resize_callback);

  auto res = gladLoadGL(glfwGetProcAddress);
  if (res == 0) {
    throw std::runtime_error("Failed to initialize GLAD");
  }

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
}

SpringApp::~SpringApp() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImPlot::DestroyContext();
  ImGui::DestroyContext();
}

void SpringApp::update(float dt) {
  m_window->clear();

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  bool dwnd = true;
  ImGui::ShowDemoWindow(&dwnd);
  ImPlot::ShowDemoWindow(&dwnd);

  ImGui::Render();
  ImGui::EndFrame();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  m_window->update();
}

void SpringApp::close_callback(GLFWwindow* window) {
  SpringApp* app = static_cast<SpringApp*>(glfwGetWindowUserPointer(window));
  app->m_run = false;
}

void SpringApp::framebuffer_resize_callback(GLFWwindow* window, int width, int height) {
  SpringApp* app = static_cast<SpringApp*>(glfwGetWindowUserPointer(window));
  app->m_window->set_size(width, height);
}