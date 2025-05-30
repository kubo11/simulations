#include "blackhole_ui.hh"

BlackHoleUI::BlackHoleUI(Window& window, std::shared_ptr<MessageQueueWriter<BlackHoleMessage>> message_queue) :
      UI(window),
      m_window(window),
      m_message_queue(message_queue),
      m_ui_mtx() {}

float BlackHoleUI::get_distance() {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  return m_distance;
}

float BlackHoleUI::get_mass() {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  return m_mass;
}

void BlackHoleUI::draw() {
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowSize(ImVec2(0.2 * m_window.get_width(), m_window.get_height()));
  if (ImGui::Begin("BlackHoleControlls", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground)) {
    show_property_panel();
  }
  ImGui::End();
}

void BlackHoleUI::show_property_panel() {
  if (ImGui::SliderFloat("##BlackHoleMass", &m_mass, 1.0f, 100.0f, "Mass: %.3f")) {
    m_message_queue->push(BlackHoleMessage::UpdateMass);
  }
  if (ImGui::SliderFloat("##BlackHoleDistance", &m_distance, 1.0f, 1e4f, "Distance: %.3f", ImGuiSliderFlags_Logarithmic)) {
    m_message_queue->push(BlackHoleMessage::UpdateDistance);
  }
}