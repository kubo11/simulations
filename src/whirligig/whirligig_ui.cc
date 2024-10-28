#include "whirligig_ui.hh"

WhirligigUI::WhirligigUI(Window& window, Whirligig& whirligig, std::function<void(void)> start_handler,
              std::function<void(void)> stop_handler, std::function<void(void)> apply_handler,
              std::function<void(void)> skip_handler)
    : UI(window),
      m_window(window),
      m_whirligig(whirligig),
      m_start_handler(start_handler),
      m_stop_handler(stop_handler),
      m_apply_handler(apply_handler),
      m_skip_handler(skip_handler),
      m_ui_mtx() {
  m_gravity_function.emplace_back(std::unique_ptr<Function>(new ConstFunction(9.81f)));
  m_gravity_function.emplace_back(std::unique_ptr<Function>(new StepFunction(9.81f, 0.0f, 0.0f)));
  m_gravity_function.emplace_back(std::unique_ptr<Function>(new SinStepFunction(9.81f, 1.0f, 0.0f)));
  m_gravity_function.emplace_back(std::unique_ptr<Function>(new SinFunction(9.81f, 1.0f, 0.0f)));
}

void WhirligigUI::update_whirligig_data() {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  m_orientation = glm::normalize(m_whirligig.get_diagonal());
  m_gravity = m_whirligig.get_gravity();
  m_mass_center = 0.5f * m_whirligig.get_diagonal();
}

void WhirligigUI::update_whirligig_parameters() {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  m_whirligig.set_cube_size(m_cube_size);
  m_whirligig.set_cube_density(m_cube_density);
  m_whirligig.set_gravity_function(m_gravity_function[m_selected_gravity_func_idx]->copy());
}

void WhirligigUI::reset_whirligig_parameters() {
  update_whirligig_parameters();
  m_whirligig.reset(m_incline, m_angular_velocity);
}

float WhirligigUI::get_dt() {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  return m_dt;
}

unsigned int WhirligigUI::get_skip_frames_count() {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  return static_cast<unsigned int>(m_skip_frames);
}

float WhirligigUI::get_incline() {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  return m_incline;
}

unsigned int WhirligigUI::get_trajectory_length() {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  return static_cast<unsigned int>(m_trajectory_length);
}

void WhirligigUI::draw() {
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowSize(ImVec2(0.2 * m_window.get_width(), m_window.get_height()));
  if (ImGui::Begin("WhirligigControls", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove)) {
    show_property_panel();
  }
  ImGui::End();
}

void WhirligigUI::show_property_panel() {
  ImVec2 size = ImGui::GetItemRectSize();
  size.x = (size.x - 2.0f * ImGui::GetStyle().ItemSpacing.x - 2.0f * ImGui::GetStyle().WindowPadding.x) / 3.0f;
  ImGui::BeginGroup();
  ImGui::BeginDisabled(!m_start_button_enabled);
  if (ImGui::Button("Start", size)) {
      m_start_handler();
      m_start_button_enabled = false;
      m_stop_button_enabled = true;
      m_skip_button_enabled = true;
  }
  ImGui::EndDisabled();
  ImGui::SameLine();
  ImGui::BeginDisabled(!m_stop_button_enabled);
  if (ImGui::Button("Pause", size)) {
    m_stop_handler();
    m_stop_button_enabled = false;
    m_start_button_enabled = true;
    m_skip_button_enabled = false;
  }
  ImGui::EndDisabled();
  ImGui::SameLine();
  if (ImGui::Button("Restart", size)) {
    m_stop_handler();
    m_apply_handler();
    m_whirligig.reset(m_incline, m_angular_velocity);
    m_start_handler();
    m_start_button_enabled = false;
    m_stop_button_enabled = true;
    m_skip_button_enabled = true;
    m_apply_button_enabled = false;
  }
  ImGui::EndGroup();

  ImGui::BeginDisabled(!m_skip_button_enabled);
  if (ImGui::Button("Skip", size)) {
    m_skip_handler();
  }
  ImGui::EndDisabled();
  ImGui::SameLine();
  ImGui::PushItemWidth(-1);
  ImGui::DragInt("##skip_frames", &m_skip_frames, 1.0f, 1, 1e4);
  ImGui::Separator();
  
  imgui_center_text("App information");
  ImGuiIO& io = ImGui::GetIO();
  ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

  {
    std::lock_guard<std::mutex> guard(m_ui_mtx);
  
    imgui_center_text("Simulation information");
    auto orientation = glm::normalize(m_whirligig.get_diagonal());
    ImGui::Text("o(t): %.3f, %.3f, %.3f", m_orientation.x, m_orientation.y, m_orientation.z);
    ImGui::Text("c(t): %.3f, %.3f, %.3f", m_mass_center.x, m_mass_center.y, m_mass_center.z);
    ImGui::Text("g(t): %.3f", m_gravity);

    imgui_center_text("Simulation properties");
    ImGui::Text("cube size:   ");
    ImGui::SameLine();
    if (ImGui::DragFloat("##cube_size", &m_cube_size)) m_apply_button_enabled = true;
    ImGui::Text("cube density:");
    ImGui::SameLine();
    if (ImGui::DragFloat("##cube_density", &m_cube_density)) m_apply_button_enabled = true;
    ImGui::Text("cube incline:");
    ImGui::SameLine();
    if (ImGui::DragFloat("##cube_incline", &m_incline)) m_apply_button_enabled = true;
    ImGui::Text("dt:          ");
    ImGui::SameLine();
    if (ImGui::DragFloat("##dt", &m_dt)) m_apply_button_enabled = true;
    ImGui::Text("g(t):        ");
    ImGui::SameLine();
    if (ImGui::BeginCombo("##gravity_func",
                          m_gravity_function[m_selected_gravity_func_idx]->to_string().c_str(),
                          ImGuiComboFlags_None)) {
      for (int i = 0; i < m_gravity_function.size(); ++i) {
        const bool is_selected = (m_selected_gravity_func_idx == i);
        if (ImGui::Selectable(m_gravity_function[i]->to_string().c_str(), is_selected)) {
          m_selected_gravity_func_idx = i;
          m_apply_button_enabled = true;
        }

        if (is_selected) ImGui::SetItemDefaultFocus();
      }
      ImGui::EndCombo();
    }
    if (m_gravity_function[m_selected_gravity_func_idx]->show_ui()) m_apply_button_enabled = true;

    imgui_center_text("Visulaization properties");
    ImGui::Checkbox("show cube", &m_show_cube);
    ImGui::Checkbox("show diagonal", &m_show_diagonal);
    ImGui::Checkbox("show trajectory", &m_show_trajectory);
    ImGui::Checkbox("show gravity vector", &m_show_gravity_vector);
    ImGui::Checkbox("show plane", &m_show_plane);
    ImGui::Text("path length");
    ImGui::SameLine();
    ImGui::DragInt("##path_length", &m_path_length);
  }
  ImGui::BeginDisabled(!m_apply_button_enabled);
  if (ImGui::Button("Apply", ImVec2(-1.0, 0.0))) {
    if (m_stop_button_enabled) {
      m_stop_handler();
      m_apply_handler();
      m_start_handler();
    }
    else {
      m_apply_handler();
    }
    m_apply_button_enabled = false;
  } 
  ImGui::EndDisabled();
}

bool WhirligigUI::show_cube() {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  return m_show_cube;
}

bool WhirligigUI::show_diagonal() {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  return m_show_diagonal;
}

bool WhirligigUI::show_trajectory() {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  return m_show_trajectory;
}

bool WhirligigUI::show_gravity_vector() {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  return m_show_gravity_vector;
}

bool WhirligigUI::show_plane() {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  return m_show_plane;
}
