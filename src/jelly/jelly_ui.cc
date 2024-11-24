#include "jelly_ui.hh"

JellyUI::JellyUI(Window& window, std::shared_ptr<MessageQueueWriter<JellyMessage>> message_queue)
    : UI(window),
      m_window(window),
      m_message_queue(message_queue),
      m_ui_mtx() {
}

void JellyUI::update_jelly_data(const Jelly& jelly) {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
}

void JellyUI::update_jelly_parameters(Jelly& jelly) {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  jelly.set_mass(m_mass);
  jelly.set_frame_position(glm::vec3(m_control_frame_position[0], m_control_frame_position[1], m_control_frame_position[2]));
  jelly.set_damping(m_k);
  jelly.set_inner_spring_elasticity(m_c1);
  jelly.set_frame_spring_elasticity(m_c2);
  jelly.set_size(1.0);
  jelly.set_gravitational_acceleration(glm::vec3(0.0f));
  jelly.set_distortion_amount(m_distortion_amount);
}

void JellyUI::reset_jelly(Jelly& jelly) {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  jelly.reset();
}

float JellyUI::get_dt() {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  return m_dt;
}

unsigned int JellyUI::get_skip_frames_count() {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  return static_cast<unsigned int>(m_skip_frames);
}

glm::vec3 JellyUI::get_frame_position() const {
  return glm::vec3{m_control_frame_position[0], m_control_frame_position[1], m_control_frame_position[2]};
}

glm::vec3 JellyUI::get_frame_orientation() const {
  return glm::vec3{m_control_frame_orientation[0], m_control_frame_orientation[1], m_control_frame_orientation[2]};
}

bool JellyUI::show_control_points() const {
   return m_show_control_points;
}

bool JellyUI::show_control_point_springs() const {
  return m_show_control_point_springs;
}

bool JellyUI::show_control_frame() const {
  return m_show_control_frame;
}

bool JellyUI::show_control_frame_springs() const {
  return m_show_control_frame_springs;
}

bool JellyUI::show_bounding_box() const {
  return m_show_bounding_box;
}

bool JellyUI::show_bezier_cube() const {
  return m_show_bezier_cube;
}

bool JellyUI::show_model() const {
  return m_show_model;
}


void JellyUI::draw() {
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowSize(ImVec2(0.2 * m_window.get_width(), m_window.get_height()));
  if (ImGui::Begin("JellyControls", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove)) {
    show_property_panel();
  }
  ImGui::End();
}

void JellyUI::show_property_panel() {
  ImVec2 size = ImGui::GetItemRectSize();
  size.x = (size.x - 2.0f * ImGui::GetStyle().ItemSpacing.x - 2.0f * ImGui::GetStyle().WindowPadding.x) / 3.0f;
  ImGui::BeginGroup();
  ImGui::BeginDisabled(!m_start_button_enabled);
  if (ImGui::Button("Start", size)) {
    m_message_queue->push(JellyMessage::Start);
    m_start_button_enabled = false;
    m_stop_button_enabled = true;
    m_skip_button_enabled = true;
  }
  ImGui::EndDisabled();
  ImGui::SameLine();
  ImGui::BeginDisabled(!m_stop_button_enabled);
  if (ImGui::Button("Pause", size)) {
    m_message_queue->push(JellyMessage::Stop);
    m_stop_button_enabled = false;
    m_start_button_enabled = true;
    m_skip_button_enabled = false;
  }
  ImGui::EndDisabled();
  ImGui::SameLine();
  if (ImGui::Button("Restart", size)) {
    m_message_queue->push(JellyMessage::Restart);
    m_start_button_enabled = false;
    m_stop_button_enabled = true;
    m_skip_button_enabled = true;
    m_apply_button_enabled = false;
  }
  ImGui::EndGroup();

  ImGui::BeginDisabled(!m_skip_button_enabled);
  if (ImGui::Button("Skip", size)) {
    m_message_queue->push(JellyMessage::Skip);
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

    imgui_center_text("Simulation properties");
    ImGui::Text("mass       ");
    ImGui::SameLine();
    if (ImGui::DragFloat("##mass", &m_mass)) m_apply_button_enabled = true;
    ImGui::Text("c1         ");
    ImGui::SameLine();
    if (ImGui::DragFloat("##c1", &m_c1)) m_apply_button_enabled = true;
    ImGui::Text("c2         ");
    ImGui::SameLine();
    if (ImGui::DragFloat("##c2", &m_c2)) m_apply_button_enabled = true;
    ImGui::Text("k          ");
    ImGui::SameLine();
    if (ImGui::DragFloat("##k", &m_k)) m_apply_button_enabled = true;
    ImGui::Text("distortion ");
    ImGui::SameLine();
    if (ImGui::DragFloat("##distortion", &m_distortion_amount)) m_apply_button_enabled = true;
    ImGui::Text("position   ");
    ImGui::SameLine();
    if (ImGui::DragFloat3("##position", m_control_frame_position, .1f)) m_message_queue->push(JellyMessage::UpdateFrame);
    ImGui::Text("orientation");
    ImGui::SameLine();
    if (ImGui::DragFloat3("##orientation", m_control_frame_orientation, .1f)) m_message_queue->push(JellyMessage::UpdateFrame);
    if (ImGui::Checkbox("enable control frame springs", &m_enable_control_frame_springs)) m_apply_button_enabled = true;

    imgui_center_text("Visulaization properties");
    ImGui::Checkbox("show control points", &m_show_control_points);
    ImGui::Checkbox("show control points springs", &m_show_control_point_springs);
    ImGui::Checkbox("show control frame", &m_show_control_frame);
    ImGui::Checkbox("show control frame springs", &m_show_control_frame_springs);
    ImGui::Checkbox("show bezier cube", &m_show_bezier_cube);
    ImGui::Checkbox("show model", &m_show_model);
  }
  ImGui::BeginDisabled(!m_apply_button_enabled);
  if (ImGui::Button("Apply", ImVec2(-1.0, 0.0))) {
    m_message_queue->push(JellyMessage::Apply);
    m_apply_button_enabled = false;
  }
  ImGui::EndDisabled();
  if (ImGui::Button("Distort", ImVec2(-1.0, 0.0))) {
    m_message_queue->push(JellyMessage::Distort);
  }
}