#include "flywheel_ui.hh"

FlywheelUI::FlywheelUI(Window& window, const Framebuffer& framebuffer, std::shared_ptr<MessageQueueWriter<FlywheelMessage>> message_queue)
    : UI(window),
      m_window(window),
      m_framebuffer(framebuffer),
      m_message_queue(message_queue),
      m_ui_mtx() {}

void FlywheelUI::update_flywheel_data(const Flywheel& flywheel) {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  m_piston_position.push_back(flywheel.get_piston_position());
  m_piston_velocity.push_back(flywheel.get_piston_velocity());
  m_piston_acceleration.push_back(flywheel.get_piston_acceleration());
  m_time.push_back(flywheel.get_t());
}

void FlywheelUI::update_flywheel_parameters(Flywheel& flywheel) {
  flywheel.set_angular_velocity(m_angular_velocity);
  flywheel.set_radius(m_radius);
  flywheel.set_length(m_length);
  flywheel.set_standard_deviation(m_standard_deviation);
}

void FlywheelUI::reset_flywheel(Flywheel& flywheel) {
  flywheel.reset();
}

void FlywheelUI::clear() {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  m_piston_position.clear();
  m_piston_velocity.clear();
  m_piston_acceleration.clear();
  m_time.clear();
}

float FlywheelUI::get_dt() {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  return m_dt;
}

unsigned int FlywheelUI::get_skip_frames_count() {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  return static_cast<unsigned int>(m_skip_frames);
}

void FlywheelUI::draw() {
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowSize(ImVec2(m_window.get_width(), m_window.get_height()));
  if (ImGui::Begin("Flywheel", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove)) {
    show_property_panel();
    show_pos_graph();
    show_vel_graph();
    show_acc_graph();
    show_trajectory_graph();
    show_visulaization();
  }
  ImGui::End();
}

void FlywheelUI::show_property_panel() {
  ImGui::SetNextWindowPos(ImVec2(0.7 * m_window.get_width(), 0));
  ImGui::BeginChild("properties", ImVec2(0.3 * m_window.get_width(), 0.5 * m_window.get_height()),
                    ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
  ImVec2 size = ImGui::GetItemRectSize();
  size.x = (size.x - 2.0f * ImGui::GetStyle().ItemSpacing.x - 2.0f * ImGui::GetStyle().WindowPadding.x) / 3.0f;
  ImGui::BeginGroup();
  ImGui::BeginDisabled(!m_start_button_enabled);
  if (ImGui::Button("Start", size)) {
    m_message_queue->push(FlywheelMessage::Start);
    m_start_button_enabled = false;
    m_stop_button_enabled = true;
    m_skip_button_enabled = true;
  }
  ImGui::EndDisabled();
  ImGui::SameLine();
  ImGui::BeginDisabled(!m_stop_button_enabled);
  if (ImGui::Button("Pause", size)) {
    m_message_queue->push(FlywheelMessage::Stop);
    m_stop_button_enabled = false;
    m_start_button_enabled = true;
    m_skip_button_enabled = false;
  }
  ImGui::EndDisabled();
  ImGui::SameLine();
  if (ImGui::Button("Restart", size)) {
    m_message_queue->push(FlywheelMessage::Restart);
    m_start_button_enabled = false;
    m_stop_button_enabled = true;
    m_skip_button_enabled = true;
    m_apply_button_enabled = false;
  }
  ImGui::EndGroup();

  ImGui::BeginDisabled(!m_skip_button_enabled);
  if (ImGui::Button("Skip", size)) {
    m_message_queue->push(FlywheelMessage::Skip);
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
    ImGui::Text("Piston");
    ImGui::Text("x(t): %.3f", (!m_piston_position.empty()) ? m_piston_position.back() : 0.0f);
    ImGui::Text("v(t): %.3f", (!m_piston_velocity.empty()) ? m_piston_velocity.back() : 0.0f);
    ImGui::Text("a(t): %.3f", (!m_piston_acceleration.empty()) ? m_piston_acceleration.back() : 0.0f);

    imgui_center_text("Simulation properties");
    ImGui::Text("dt:  ");
    ImGui::SameLine();
    if (ImGui::DragFloat("##dt", &m_dt, 0.05f, 0.0f, 10.0f)) m_apply_button_enabled = true;
    ImGui::Text("R:   ");
    ImGui::SameLine();
    if (ImGui::DragFloat("##R", &m_radius, 0.05f, 0.1f, m_length / 2.0f)) m_apply_button_enabled = true;
    ImGui::Text("L:   ");
    ImGui::SameLine();
    if (ImGui::DragFloat("##L", &m_length, 0.05f, 2.0f*m_radius, 10.0f)) m_apply_button_enabled = true;
    ImGui::Text("w:   ");
    ImGui::SameLine();
    if (ImGui::DragFloat("##w", &m_angular_velocity, 0.05f, 0.0f, 10.0f)) m_apply_button_enabled = true;
    ImGui::Text("e0:  ");
    ImGui::SameLine();
    if (ImGui::DragFloat("##e0", &m_standard_deviation, 0.001f, 0.0f, 0.1f)) m_apply_button_enabled = true;
  }
  ImGui::BeginDisabled(!m_apply_button_enabled);
  if (ImGui::Button("Apply", ImVec2(-1.0, 0.0))) {
    m_message_queue->push(FlywheelMessage::Apply);
    m_apply_button_enabled = false;
  }
  ImGui::EndDisabled();

  ImGui::Text("Autofit plot axes: ");
  ImGui::SameLine();
  ImGui::Checkbox("X", &m_autofit_x);
  ImGui::SameLine();
  ImGui::Checkbox("Y", &m_autofit_y);

  ImGui::EndChild();
}

void FlywheelUI::show_pos_graph() {
  ImGui::SetNextWindowPos(ImVec2(0.0, 0.7 * m_window.get_height()));
  ImGui::BeginChild("pos_panel", ImVec2(0.7 * m_window.get_width() / 3.0f, 0.3 * m_window.get_height()),
                    ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
  static ImPlotAxisFlags axis_flags = ImPlotAxisFlags_None;
  {
    std::lock_guard<std::mutex> guard(m_ui_mtx);
    if (ImPlot::BeginPlot("Position", ImVec2(-1, -1), ImPlotFlags_Equal | ImPlotFlags_NoLegend)) {
      axis_flags = m_autofit_x ? ImPlotAxisFlags_AutoFit : ImPlotAxisFlags_None;
      ImPlot::SetupAxis(ImAxis_X1, "t", axis_flags);
      axis_flags = m_autofit_y ? ImPlotAxisFlags_AutoFit : ImPlotAxisFlags_None;
      ImPlot::SetupAxis(ImAxis_Y1, "x", axis_flags);
      ImPlot::PlotLine("Position", m_time.data(), m_piston_position.data(), m_time.size());
    }
    ImPlot::EndPlot();
  }
  ImGui::EndChild();
}

void FlywheelUI::show_vel_graph() {
  ImGui::SetNextWindowPos(ImVec2(0.7 * m_window.get_width() / 3.0f, 0.7 * m_window.get_height()));
  ImGui::BeginChild("velpanel", ImVec2(0.7 * m_window.get_width() / 3.0f, 0.3 * m_window.get_height()),
                    ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
  static ImPlotAxisFlags axis_flags = ImPlotAxisFlags_None;
  {
    std::lock_guard<std::mutex> guard(m_ui_mtx);
    if (ImPlot::BeginPlot("Velocity", ImVec2(-1, -1), ImPlotFlags_Equal | ImPlotFlags_NoLegend)) {
      axis_flags = m_autofit_x ? ImPlotAxisFlags_AutoFit : ImPlotAxisFlags_None;
      ImPlot::SetupAxis(ImAxis_X1, "t", axis_flags);
      axis_flags = m_autofit_y ? ImPlotAxisFlags_AutoFit : ImPlotAxisFlags_None;
      ImPlot::SetupAxis(ImAxis_Y1, "x'", axis_flags);
      ImPlot::PlotLine("Velocity", m_time.data(), m_piston_velocity.data(), m_time.size());
    }
    ImPlot::EndPlot();
  }
  ImGui::EndChild();
}

void FlywheelUI::show_acc_graph() {
  ImGui::SetNextWindowPos(ImVec2(0.7 * m_window.get_width() / 3.0f * 2.0f, 0.7 * m_window.get_height()));
  ImGui::BeginChild("acc_panel", ImVec2(0.7 * m_window.get_width() / 3.0f, 0.3 * m_window.get_height()),
                    ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
  static ImPlotAxisFlags axis_flags = ImPlotAxisFlags_None;
  {
    std::lock_guard<std::mutex> guard(m_ui_mtx);
    if (ImPlot::BeginPlot("Acceleration", ImVec2(-1, -1), ImPlotFlags_Equal | ImPlotFlags_NoLegend)) {
      axis_flags = m_autofit_x ? ImPlotAxisFlags_AutoFit : ImPlotAxisFlags_None;
      ImPlot::SetupAxis(ImAxis_X1, "t", axis_flags);
      axis_flags = m_autofit_y ? ImPlotAxisFlags_AutoFit : ImPlotAxisFlags_None;
      ImPlot::SetupAxis(ImAxis_Y1, "x''", axis_flags);
      ImPlot::PlotLine("Acceleration", m_time.data(), m_piston_acceleration.data(), m_time.size());
    }
    ImPlot::EndPlot();
  }
  ImGui::EndChild();
}

void FlywheelUI::show_trajectory_graph() {
  ImGui::SetNextWindowPos(ImVec2(0.7 * m_window.get_width(), 0.5 * m_window.get_height()));
  ImGui::BeginChild("trajectory_panel", ImVec2(0.3 * m_window.get_width(), 0.5 * m_window.get_height()),
                    ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
  static ImPlotAxisFlags axis_flags = ImPlotAxisFlags_None;
  {
    std::lock_guard<std::mutex> guard(m_ui_mtx);
    if (ImPlot::BeginPlot("Trajectory", ImVec2(-1, -1), ImPlotFlags_Equal | ImPlotFlags_NoLegend)) {
      axis_flags = m_autofit_x ? ImPlotAxisFlags_AutoFit : ImPlotAxisFlags_None;
      ImPlot::SetupAxis(ImAxis_X1, "x", axis_flags);
      axis_flags = m_autofit_y ? ImPlotAxisFlags_AutoFit : ImPlotAxisFlags_None;
      ImPlot::SetupAxis(ImAxis_Y1, "x'", axis_flags);
      ImPlot::PlotLine("", m_piston_position.data(), m_piston_velocity.data(), m_time.size());
    }
    ImPlot::EndPlot();
  }
  ImGui::EndChild();
}

void FlywheelUI::show_visulaization() {
  auto padding = ImGui::GetStyle().WindowPadding;
  auto window_pos = ImVec2(0, 0);
  auto window_size = ImVec2(0.7 * m_window.get_width(), 0.7 * m_window.get_height());
  ImGui::SetNextWindowPos(window_pos);
  ImGui::BeginChild("visualization", window_size, ImGuiChildFlags_AlwaysUseWindowPadding,
                    ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
  ImGui::GetWindowDrawList()->AddImage(m_framebuffer.get_texture_id(), window_pos + padding,
                                       window_pos + window_size - padding, ImVec2(0, 1), ImVec2(1, 0));
  ImGui::EndChild();
}