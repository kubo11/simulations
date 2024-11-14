#include "whirligig_ui.hh"

WhirligigUI::WhirligigUI(Window& window, std::shared_ptr<MessageQueueWriter<WhirligigMessage>> message_queue)
    : UI(window),
      m_window(window),
      m_message_queue(message_queue),
      m_ui_mtx() {
  m_gravity_function.emplace_back(std::unique_ptr<Function>(new ConstFunction(9.81f)));
  m_gravity_function.emplace_back(std::unique_ptr<Function>(new StepFunction(9.81f, 0.0f, 0.0f)));
  m_gravity_function.emplace_back(std::unique_ptr<Function>(new SinStepFunction(9.81f, 1.0f, 0.0f)));
  m_gravity_function.emplace_back(std::unique_ptr<Function>(new SinFunction(9.81f, 1.0f, 0.0f)));
}

void WhirligigUI::update_whirligig_data(const Whirligig& whirligig) {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  m_orientation = glm::normalize(whirligig.get_diagonal());
  m_gravity = whirligig.get_gravity();
  m_mass_center = 0.5f * whirligig.get_diagonal();
  m_angular_velocity = whirligig.get_angular_velocity();
}

void WhirligigUI::update_whirligig_parameters(Whirligig& whirligig) {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  whirligig.set_cube_size(m_cube_size);
  whirligig.set_cube_density(m_cube_density);
  whirligig.set_gravity(m_enable_gravity);
  whirligig.set_gravity_function(m_gravity_function[m_selected_gravity_func_idx]->copy());
}

void WhirligigUI::reset_whirligig(Whirligig& whirligig) {
  whirligig.reset(m_cube_tilt, m_starting_angular_velocity);
}

float WhirligigUI::get_dt() {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  return m_dt;
}

unsigned int WhirligigUI::get_skip_frames_count() {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  return static_cast<unsigned int>(m_skip_frames);
}

float WhirligigUI::get_cube_tilt() {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  return m_cube_tilt;
}

unsigned int WhirligigUI::get_trajectory_length() {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  return static_cast<unsigned int>(m_path_length);
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
    m_message_queue->push(WhirligigMessage::Start);
    m_start_button_enabled = false;
    m_stop_button_enabled = true;
    m_skip_button_enabled = true;
  }
  ImGui::EndDisabled();
  ImGui::SameLine();
  ImGui::BeginDisabled(!m_stop_button_enabled);
  if (ImGui::Button("Pause", size)) {
    m_message_queue->push(WhirligigMessage::Stop);
    m_stop_button_enabled = false;
    m_start_button_enabled = true;
    m_skip_button_enabled = false;
  }
  ImGui::EndDisabled();
  ImGui::SameLine();
  if (ImGui::Button("Restart", size)) {
    m_message_queue->push(WhirligigMessage::Restart);
    m_start_button_enabled = false;
    m_stop_button_enabled = true;
    m_skip_button_enabled = true;
    m_apply_button_enabled = false;
  }
  ImGui::EndGroup();

  ImGui::BeginDisabled(!m_skip_button_enabled);
  if (ImGui::Button("Skip", size)) {
    m_message_queue->push(WhirligigMessage::Skip);
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
    ImGui::Text("o(t): %.3f, %.3f, %.3f", m_orientation.x, m_orientation.y, m_orientation.z);
    ImGui::Text("w(t): %.3f, %.3f, %.3f", m_angular_velocity.x, m_angular_velocity.y, m_angular_velocity.z);
    ImGui::Text("c(t): %.3f, %.3f, %.3f", m_mass_center.x, m_mass_center.y, m_mass_center.z);
    ImGui::Text("g(t): %.3f", m_gravity);

    imgui_center_text("Simulation properties");
    ImGui::Text("angular velocity:");
    ImGui::SameLine();
    ImGui::DragFloat("##starting_angular_velocity", &m_starting_angular_velocity);
    ImGui::Text("cube tilt:       ");
    ImGui::SameLine();
    ImGui::DragFloat("##cube_incline", &m_cube_tilt);
    ImGui::Text("cube size:       ");
    ImGui::SameLine();
    if (ImGui::DragFloat("##cube_size", &m_cube_size)) m_apply_button_enabled = true;
    ImGui::Text("cube density:    ");
    ImGui::SameLine();
    if (ImGui::DragFloat("##cube_density", &m_cube_density)) m_apply_button_enabled = true;
    ImGui::Text("dt:              ");
    ImGui::SameLine();
    if (ImGui::DragFloat("##dt", &m_dt, 1.0f, 0.0f, 0.0f, "%.8f")) m_apply_button_enabled = true;
    ImGui::Text("g(t):            ");
    ImGui::SameLine();
    ImGui::BeginDisabled(!m_enable_gravity);
    if (ImGui::BeginCombo("##gravity_func", m_gravity_function[m_selected_gravity_func_idx]->to_string().c_str(),
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
    ImGui::EndDisabled();
    if (ImGui::Checkbox("enable gravity", &m_enable_gravity)) m_apply_button_enabled = true;

    imgui_center_text("Visulaization properties");
    ImGui::Checkbox("show cube", &m_show_cube);
    ImGui::Checkbox("show diagonal", &m_show_diagonal);
    ImGui::Checkbox("show trajectory", &m_show_trajectory);
    ImGui::Checkbox("show gravity vector", &m_show_gravity_vector);
    ImGui::Checkbox("show plane", &m_show_plane);
    ImGui::Text("path length");
    ImGui::SameLine();
    if (ImGui::DragInt("##path_length", &m_path_length)) m_apply_button_enabled = true;
  }
  ImGui::BeginDisabled(!m_apply_button_enabled);
  if (ImGui::Button("Apply", ImVec2(-1.0, 0.0))) {
    m_message_queue->push(WhirligigMessage::Apply);
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
