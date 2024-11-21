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
}

void JellyUI::reset_jelly(Jelly& jelly) {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
}

float JellyUI::get_dt() {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  return m_dt;
}

unsigned int JellyUI::get_skip_frames_count() {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  return static_cast<unsigned int>(m_skip_frames);
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

    imgui_center_text("Visulaization properties");

  }
  ImGui::BeginDisabled(!m_apply_button_enabled);
  if (ImGui::Button("Apply", ImVec2(-1.0, 0.0))) {
    m_message_queue->push(JellyMessage::Apply);
    m_apply_button_enabled = false;
  }
  ImGui::EndDisabled();
}