#include "jelly_ui.hh"

JellyUI::JellyUI(Window& window, std::shared_ptr<MessageQueueWriter<JellyMessage>> message_queue)
    : UI(window),
      m_window(window),
      m_message_queue(message_queue),
      m_ui_mtx() {
}

void JellyUI::update_jelly_parameters(Jelly& jelly) {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  jelly.set_mass(m_mass);
  jelly.set_damping(m_k);
  jelly.set_inner_spring_elasticity(m_c1);
  jelly.set_frame_spring_elasticity(m_c2);
  jelly.set_size(1.0);
  jelly.set_gravitational_acceleration(glm::vec3(m_gravity[0], m_gravity[1], m_gravity[2]));
  jelly.set_distortion_amount(m_distortion_amount);
  jelly.toggle_frame_springs(m_enable_control_frame_springs);
  jelly.set_collision_elasticity(m_collision_elasticity_coef);
  jelly.set_collision_model(m_collision_model);
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

bool JellyUI::get_control_springs_state() const {
  return m_enable_control_frame_springs;
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
    ImGui::Text("gravity    ");
    ImGui::SameLine();
    if (ImGui::DragFloat3("##gravity", m_gravity)) m_apply_button_enabled = true;
    ImGui::Text("mu         ");
    ImGui::SameLine();
    if (ImGui::SliderFloat("##collision_elasticity", &m_collision_elasticity_coef, 0.0f, 1.0f)) m_apply_button_enabled = true;
    imgui_center_text("Collision model");
    if (ImGui::RadioButton("Full velocity modification", m_collision_model == CollisionModel::FullVelocityDamping)) {
      m_collision_model = CollisionModel::FullVelocityDamping;
      m_apply_button_enabled = true;
    }
    if (ImGui::RadioButton("Velocity component modification", m_collision_model == CollisionModel::VelocityComponentDamping)) {
      m_collision_model = CollisionModel::VelocityComponentDamping;
      m_apply_button_enabled = true;
    }

    ImGui::BeginDisabled(!m_apply_button_enabled);
    if (ImGui::Button("Apply", ImVec2(-1.0, 0.0))) {
      m_message_queue->push(JellyMessage::Apply);
      m_apply_button_enabled = false;
    }
    ImGui::EndDisabled();

    ImGui::Separator();

    imgui_center_text("Controls");
    if (ImGui::Checkbox("enable control frame springs", &m_enable_control_frame_springs)) m_message_queue->push(JellyMessage::UpdateFrame);
    ImGui::Text("position   ");
    ImGui::SameLine();
    if (ImGui::DragFloat3("##position", m_control_frame_position, .1f)) m_message_queue->push(JellyMessage::UpdateFrame);
    ImGui::Text("orientation");
    ImGui::SameLine();
    if (ImGui::DragFloat3("##orientation", m_control_frame_orientation, .1f)) m_message_queue->push(JellyMessage::UpdateFrame);
    if (ImGui::Button("Distort", ImVec2(-1.0, 0.0))) {
      m_message_queue->push(JellyMessage::Distort);
    }

    ImGui::Separator();

    imgui_center_text("Visulaization properties");
    ImGui::Checkbox("show control points", &m_show_control_points);
    ImGui::Checkbox("show control points springs", &m_show_control_point_springs);
    ImGui::Checkbox("show control frame", &m_show_control_frame);
    ImGui::Checkbox("show control frame springs", &m_show_control_frame_springs);
    ImGui::Checkbox("show bezier cube", &m_show_bezier_cube);
    ImGui::BeginDisabled(true);
    ImGui::Checkbox("show model", &m_show_model);
    ImGui::EndDisabled();

    if (ImGui::Button("Open")) {
      IGFD::FileDialogConfig config;
      config.path = m_texture_path;
      ImGuiFileDialog::Instance()->OpenDialog("ChooseTexture", "Choose File", ".png,.jpg", config);
    }
    ImGui::SameLine();
    ImGui::InputText("##texture_path", &m_texture_path, ImGuiInputTextFlags_ReadOnly);
    if (ImGui::Button("Open##2")) {
      IGFD::FileDialogConfig config;
      config.path = m_model_path;
      ImGuiFileDialog::Instance()->OpenDialog("ChooseModel", "Choose File", ".obj", config);
    }
    ImGui::SameLine();
    ImGui::InputText("##model_path", &m_model_path, ImGuiInputTextFlags_ReadOnly);

    if (ImGuiFileDialog::Instance()->Display("ChooseTexture")) {
      if (ImGuiFileDialog::Instance()->IsOk()) {
        std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
        std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
        // action
      }

      ImGuiFileDialog::Instance()->Close();
    }
    if (ImGuiFileDialog::Instance()->Display("ChooseModel")) {
      if (ImGuiFileDialog::Instance()->IsOk()) {
        std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
        std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
        // action
      }

      ImGuiFileDialog::Instance()->Close();
    }
  }
}