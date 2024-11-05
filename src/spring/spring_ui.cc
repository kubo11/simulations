#include "spring_ui.hh"

SpringUI::SpringUI(Window& window, const Framebuffer& framebuffer, Spring& spring,
                   std::function<void(void)> start_handler, std::function<void(void)> stop_handler,
                   std::function<void(void)> apply_handler, std::function<void(void)> skip_handler)
    : UI(window),
      m_window(window),
      m_framebuffer(framebuffer),
      m_spring(spring),
      m_rest_position_functions(),
      m_field_force_function(),
      m_start_handler(start_handler),
      m_stop_handler(stop_handler),
      m_apply_handler(apply_handler),
      m_skip_handler(skip_handler),
      m_ui_mtx() {
  m_rest_position_functions.emplace_back(std::unique_ptr<Function>(new ConstFunction(0.0f)));
  m_rest_position_functions.emplace_back(std::unique_ptr<Function>(new StepFunction(1.0f, 0.0f, 0.0f)));
  m_rest_position_functions.emplace_back(std::unique_ptr<Function>(new SinStepFunction(1.0f, 1.0f, 0.0f)));
  m_rest_position_functions.emplace_back(std::unique_ptr<Function>(new SinFunction(1.0f, 1.0f, 0.0f)));
  m_field_force_function.emplace_back(std::unique_ptr<Function>(new ConstFunction(0.0f)));
  m_field_force_function.emplace_back(std::unique_ptr<Function>(new StepFunction(1.0f, 0.0f, 0.0f)));
  m_field_force_function.emplace_back(std::unique_ptr<Function>(new SinStepFunction(1.0f, 1.0f, 0.0f)));
  m_field_force_function.emplace_back(std::unique_ptr<Function>(new SinFunction(1.0f, 1.0f, 0.0f)));
}

void SpringUI::update_spring_data() {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  m_weight_position.push_back(m_spring.get_weight_position());
  m_weight_velocity.push_back(m_spring.get_weight_velocity());
  m_weight_acceleration.push_back(m_spring.get_weight_acceleration());
  m_elasticity_force.push_back(m_spring.get_elasticity_force());
  m_damping_force.push_back(m_spring.get_damping_force());
  m_field_force.push_back(m_spring.get_field_force());
  m_rest_position.push_back(m_spring.get_rest_position());
  m_time.push_back(m_spring.get_t());
}

void SpringUI::update_spring_parameters() {
  m_spring.set_elasticity_coef(m_elasticity_coef);
  m_spring.set_damping_coef(m_damping_coef);
  m_spring.set_field_force_function(m_field_force_function[m_selected_field_force_func_idx]->copy());
  m_spring.set_rest_position_function(m_rest_position_functions[m_selected_rest_pos_func_idx]->copy());
  m_spring.set_weight_mass(m_weight_mass);
}

void SpringUI::clear() {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  m_weight_position.clear();
  m_weight_velocity.clear();
  m_weight_acceleration.clear();
  m_elasticity_force.clear();
  m_damping_force.clear();
  m_field_force.clear();
  m_rest_position.clear();
  m_time.clear();
}

float SpringUI::get_dt() {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  return m_dt;
}

unsigned int SpringUI::get_skip_frames_count() {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  return static_cast<unsigned int>(m_skip_frames);
}

void SpringUI::draw() {
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowSize(ImVec2(m_window.get_width(), m_window.get_height()));
  if (ImGui::Begin("Spring", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove)) {
    show_property_panel();
    show_pos_vel_acc_graph();
    show_forces_graph();
    show_trajectory_graph();
    show_visulaization();
  }
  ImGui::End();
}

void SpringUI::show_property_panel() {
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::BeginChild("properties", ImVec2(0.2 * m_window.get_width(), m_window.get_height()),
                    ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
  ImVec2 size = ImGui::GetItemRectSize();
  size.x = (size.x - 2.0f * ImGui::GetStyle().ItemSpacing.x - 2.0f * ImGui::GetStyle().WindowPadding.x) / 3.0f;
  ImGui::BeginGroup();
  ImGui::BeginDisabled(!m_start_button_enabled);
  if (ImGui::Button("Start", size)) {
    if (!m_time.empty()) {
      m_start_handler();
    } else {
      m_apply_handler();
      m_spring.reset(m_weight_starting_position, m_weight_starting_velocity);
      m_start_handler();
    }
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
    m_spring.reset(m_weight_starting_position, m_weight_starting_velocity);
    clear();
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
    ImGui::Text("Kinematics");
    ImGui::Text("x(t): %.3f", (!m_weight_position.empty()) ? m_weight_position.back() : 0.0f);
    ImGui::Text("v(t): %.3f", (!m_weight_velocity.empty()) ? m_weight_velocity.back() : 0.0f);
    ImGui::Text("a(t): %.3f", (!m_weight_acceleration.empty()) ? m_weight_acceleration.back() : 0.0f);
    ImGui::Text("Forces");
    ImGui::Text("f(t): %.3f", (!m_elasticity_force.empty()) ? m_elasticity_force.back() : 0.0f);
    ImGui::Text("g(t): %.3f", (!m_damping_force.empty()) ? m_damping_force.back() : 0.0f);
    ImGui::Text("h(t): %.3f", (!m_field_force.empty()) ? m_field_force.back() : 0.0f);
    ImGui::Text("Other");
    ImGui::Text("w(t): %.3f", (!m_rest_position.empty()) ? m_rest_position.back() : 0.0f);

    imgui_center_text("Simulation properties");
    ImGui::Text("x0:  ");
    ImGui::SameLine();
    if (ImGui::DragFloat("##x0", &m_weight_starting_position, 0.05f, 0.0f, 10.0f)) m_apply_button_enabled = true;
    ImGui::Text("v0:  ");
    ImGui::SameLine();
    if (ImGui::DragFloat("##v0", &m_weight_starting_velocity, 0.05f, 0.0f, 10.0f)) m_apply_button_enabled = true;
    ImGui::Text("dt:  ");
    ImGui::SameLine();
    if (ImGui::DragFloat("##dt", &m_dt, 0.05f, 0.0f, 10.0f)) m_apply_button_enabled = true;
    ImGui::Text("m:   ");
    ImGui::SameLine();
    if (ImGui::DragFloat("##m", &m_weight_mass, 0.05f, 0.0f, 10.0f)) m_apply_button_enabled = true;
    ImGui::Text("k:   ");
    ImGui::SameLine();
    if (ImGui::DragFloat("##k", &m_damping_coef, 0.05f, 0.0f, 10.0f)) m_apply_button_enabled = true;
    ImGui::Text("c:   ");
    ImGui::SameLine();
    if (ImGui::DragFloat("##c", &m_elasticity_coef, 0.05f, 0.0f, 10.0f)) m_apply_button_enabled = true;

    ImGui::Text("w(t):");
    ImGui::SameLine();
    if (ImGui::BeginCombo("##rest_func", m_rest_position_functions[m_selected_rest_pos_func_idx]->to_string().c_str(),
                          ImGuiComboFlags_None)) {
      for (int i = 0; i < m_rest_position_functions.size(); ++i) {
        const bool is_selected = (m_selected_rest_pos_func_idx == i);
        if (ImGui::Selectable(m_rest_position_functions[i]->to_string().c_str(), is_selected)) {
          m_selected_rest_pos_func_idx = i;
          m_apply_button_enabled = true;
        }

        if (is_selected) ImGui::SetItemDefaultFocus();
      }
      ImGui::EndCombo();
    }
    if (m_rest_position_functions[m_selected_rest_pos_func_idx]->show_ui()) m_apply_button_enabled = true;

    ImGui::Text("h(t):");
    ImGui::SameLine();
    if (ImGui::BeginCombo("##field_force_func",
                          m_field_force_function[m_selected_field_force_func_idx]->to_string().c_str(),
                          ImGuiComboFlags_None)) {
      for (int i = 0; i < m_field_force_function.size(); ++i) {
        const bool is_selected = (m_selected_field_force_func_idx == i);
        if (ImGui::Selectable(m_field_force_function[i]->to_string().c_str(), is_selected)) {
          m_selected_field_force_func_idx = i;
          m_apply_button_enabled = true;
        }

        if (is_selected) ImGui::SetItemDefaultFocus();
      }
      ImGui::EndCombo();
    }
    if (m_field_force_function[m_selected_field_force_func_idx]->show_ui()) m_apply_button_enabled = true;
  }
  ImGui::BeginDisabled(!m_apply_button_enabled);
  if (ImGui::Button("Apply", ImVec2(-1.0, 0.0))) {
    if (m_stop_button_enabled) {
      m_stop_handler();
      m_apply_handler();
      m_start_handler();
    } else {
      m_apply_handler();
    }
    m_apply_button_enabled = false;
  }
  ImGui::EndDisabled();

  ImGui::EndChild();
}

void SpringUI::show_pos_vel_acc_graph() {
  ImGui::SetNextWindowPos(ImVec2(0.2 * m_window.get_width(), 0.7 * m_window.get_height()));
  ImGui::BeginChild("pos_vel_acc_panel", ImVec2(0.5 * m_window.get_width(), 0.3 * m_window.get_height()),
                    ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
  static ImPlotAxisFlags axis_flags = ImPlotAxisFlags_None;
  {
    std::lock_guard<std::mutex> guard(m_ui_mtx);
    if (ImPlot::BeginPlot("Position, velocity and acceleration", ImVec2(-1, -1), ImPlotFlags_Equal)) {
      ImPlot::SetupAxis(ImAxis_Y1, nullptr, axis_flags);
      axis_flags = ImPlot::IsPlotHovered() ? ImPlotAxisFlags_None : ImPlotAxisFlags_AutoFit;
      ImPlot::PlotLine("Position", m_time.data(), m_weight_position.data(), m_time.size());
      ImPlot::PlotLine("Velocity", m_time.data(), m_weight_velocity.data(), m_time.size());
      ImPlot::PlotLine("Acceleration", m_time.data(), m_weight_acceleration.data(), m_time.size());
    }
    ImPlot::EndPlot();
  }
  ImGui::EndChild();
}

void SpringUI::show_forces_graph() {
  ImGui::SetNextWindowPos(ImVec2(0.7 * m_window.get_width(), 0));
  ImGui::BeginChild("forces_panel", ImVec2(0.3 * m_window.get_width(), 0.5 * m_window.get_height()),
                    ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
  static ImPlotAxisFlags axis_flags = ImPlotAxisFlags_None;
  {
    std::lock_guard<std::mutex> guard(m_ui_mtx);
    if (ImPlot::BeginPlot("Forces", ImVec2(-1, -1), ImPlotFlags_Equal)) {
      ImPlot::SetupAxis(ImAxis_Y1, nullptr, axis_flags);
      axis_flags = ImPlot::IsPlotHovered() ? ImPlotAxisFlags_None : ImPlotAxisFlags_AutoFit;
      ImPlot::PlotLine("f(t)", m_time.data(), m_elasticity_force.data(), m_time.size());
      ImPlot::PlotLine("g(t)", m_time.data(), m_damping_force.data(), m_time.size());
      ImPlot::PlotLine("h(t)", m_time.data(), m_field_force.data(), m_time.size());
      ImPlot::PlotLine("w(t)", m_time.data(), m_rest_position.data(), m_time.size());
    }
    ImPlot::EndPlot();
  }
  ImGui::EndChild();
}

void SpringUI::show_trajectory_graph() {
  ImGui::SetNextWindowPos(ImVec2(0.7 * m_window.get_width(), 0.5 * m_window.get_height()));
  ImGui::BeginChild("trajectory_panel", ImVec2(0.3 * m_window.get_width(), 0.5 * m_window.get_height()),
                    ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
  static ImPlotAxisFlags axis_flags = ImPlotAxisFlags_None;
  {
    std::lock_guard<std::mutex> guard(m_ui_mtx);
    if (ImPlot::BeginPlot("Trajectory", ImVec2(-1, -1), ImPlotFlags_Equal)) {
      ImPlot::SetupAxis(ImAxis_X1, nullptr, axis_flags);
      ImPlot::SetupAxis(ImAxis_Y1, nullptr, axis_flags);
      axis_flags = ImPlot::IsPlotHovered() ? ImPlotAxisFlags_None : ImPlotAxisFlags_AutoFit;
      ImPlot::PlotLine("", m_weight_position.data(), m_weight_velocity.data(), m_time.size());
    }
    ImPlot::EndPlot();
  }
  ImGui::EndChild();
}

void SpringUI::show_visulaization() {
  auto padding = ImGui::GetStyle().WindowPadding;
  auto window_pos = ImVec2(0.2 * m_window.get_width(), 0);
  auto window_size = ImVec2(0.5 * m_window.get_width(), 0.7 * m_window.get_height());
  ImGui::SetNextWindowPos(window_pos);
  ImGui::BeginChild("visualization", window_size, ImGuiChildFlags_AlwaysUseWindowPadding,
                    ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
  ImGui::GetWindowDrawList()->AddImage(m_framebuffer.get_texture_id(), window_pos + padding,
                                       window_pos + window_size - padding, ImVec2(0, 1), ImVec2(1, 0));
  ImGui::EndChild();
}