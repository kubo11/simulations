#include "spring_ui.hh"

SpringUI::SpringUI(const Window& window, const Framebuffer& framebuffer, std::function<void(void)> start_handler,
                   std::function<void(void)> pause_handler, std::function<void(void)> restart_handler)
    : m_window(window),
      m_framebuffer(framebuffer),
      m_anchor_position_functions(),
      m_field_force_function(),
      m_start_handler(start_handler),
      m_pause_handler(pause_handler),
      m_restart_handler(restart_handler),
      m_ui_mtx() {
  m_anchor_position_functions.emplace_back(std::unique_ptr<Function>(new ConstFunction(1.0f)));
  m_anchor_position_functions.emplace_back(std::unique_ptr<Function>(new StepFunction(1.0f, 0.0f, 0.0f)));
  m_anchor_position_functions.emplace_back(std::unique_ptr<Function>(new SinStepFunction(1.0f, 1.0f, 0.0f)));
  m_anchor_position_functions.emplace_back(std::unique_ptr<Function>(new SinFunction(1.0f, 1.0f, 0.0f)));
  m_field_force_function.emplace_back(std::unique_ptr<Function>(new ConstFunction(1.0f)));
  m_field_force_function.emplace_back(std::unique_ptr<Function>(new StepFunction(1.0f, 0.0f, 0.0f)));
  m_field_force_function.emplace_back(std::unique_ptr<Function>(new SinStepFunction(1.0f, 1.0f, 0.0f)));
  m_field_force_function.emplace_back(std::unique_ptr<Function>(new SinFunction(1.0f, 1.0f, 0.0f)));
}

void SpringUI::update_spring_data(float position, float velocity, float acceleration, float elasticity, float damping,
                                  float field, float anchor_position, float time) {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  m_weight_position.push_back(position);
  m_weight_velocity.push_back(velocity);
  m_weight_acceleration.push_back(acceleration);
  m_elasticity_force.push_back(elasticity);
  m_damping_force.push_back(damping);
  m_field_force.push_back(field);
  m_anchor_position.push_back(anchor_position);
  m_time.push_back(time);
}

void SpringUI::clear() {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  m_weight_position.clear();
  m_weight_velocity.clear();
  m_weight_acceleration.clear();
  m_elasticity_force.clear();
  m_damping_force.clear();
  m_field_force.clear();
  m_anchor_position.clear();
  m_time.clear();
}

float SpringUI::get_weight_starting_position() {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  return m_weight_starting_position;
}

float SpringUI::get_weight_starting_velocity() {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  return m_weight_starting_velocity;
}

float SpringUI::get_dt() {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  return m_dt;
}

float SpringUI::get_weight_mass() {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  return m_weight_mass;
}

float SpringUI::get_elasticity_coef() {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  return m_elasticity_coef;
}

float SpringUI::get_damping_coef() {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  return m_damping_coef;
}

std::unique_ptr<Function> SpringUI::get_anchor_position_function() {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  return m_anchor_position_functions[m_selected_anchor_pos_func_idx]->copy();
}

std::unique_ptr<Function> SpringUI::get_field_force_function() {
  std::lock_guard<std::mutex> guard(m_ui_mtx);
  return m_field_force_function[m_selected_field_force_func_idx]->copy();
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
  bool show = true;
  ImGui::ShowDemoWindow(&show);
  ImPlot::ShowDemoWindow(&show);
  ImGui::End();
}

void SpringUI::show_property_panel() {
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::BeginChild("properties", ImVec2(0.2 * m_window.get_width(), m_window.get_height()),
                    ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
  ImVec2 size = ImGui::GetContentRegionAvail();
  if (ImGui::Button("Start", ImVec2(size.x / 3, 25))) {
    m_start_handler();
  }
  ImGui::SameLine();
  if (ImGui::Button("Pause", ImVec2(size.x / 3, 25))) {
    m_pause_handler();
  }
  ImGui::SameLine();
  if (ImGui::Button("Restart", ImVec2(size.x / 3, 25))) {
    m_restart_handler();
    clear();
  }
  ImGui::Text("App information");
  ImGuiIO& io = ImGui::GetIO();
  ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

  {
    std::lock_guard<std::mutex> guard(m_ui_mtx);
    ImGui::Text("Simulation information");
    ImGui::Text("Kinematics");
    ImGui::Text("x(t): %.3f", (!m_weight_position.empty()) ? m_weight_position.back() : 0.0f);
    ImGui::Text("v(t): %.3f", (!m_weight_velocity.empty()) ? m_weight_velocity.back() : 0.0f);
    ImGui::Text("a(t): %.3f", (!m_weight_acceleration.empty()) ? m_weight_acceleration.back() : 0.0f);
    ImGui::Text("Forces");
    ImGui::Text("f(t): %.3f", (!m_elasticity_force.empty()) ? m_elasticity_force.back() : 0.0f);
    ImGui::Text("g(t): %.3f", (!m_damping_force.empty()) ? m_damping_force.back() : 0.0f);
    ImGui::Text("h(t): %.3f", (!m_field_force.empty()) ? m_field_force.back() : 0.0f);
    ImGui::Text("Other");
    ImGui::Text("w(t): %.3f", (!m_anchor_position.empty()) ? m_anchor_position.back() : 0.0f);

    ImGui::Text("Simulation properties");
    ImGui::Text("x0:  ");
    ImGui::SameLine();
    ImGui::DragFloat("##x0", &m_weight_starting_position, 0.05f, 0.0f, 10.0f);
    ImGui::Text("v0:  ");
    ImGui::SameLine();
    ImGui::DragFloat("##v0", &m_weight_starting_velocity, 0.05f, 0.0f, 10.0f);
    ImGui::Text("dt:  ");
    ImGui::SameLine();
    ImGui::DragFloat("##dt", &m_dt, 0.05f, 0.0f, 10.0f);
    ImGui::Text("m:   ");
    ImGui::SameLine();
    ImGui::DragFloat("##m", &m_weight_mass, 0.05f, 0.0f, 10.0f);
    ImGui::Text("k:   ");
    ImGui::SameLine();
    ImGui::DragFloat("##k", &m_damping_coef, 0.05f, 0.0f, 10.0f);
    ImGui::Text("c:   ");
    ImGui::SameLine();
    ImGui::DragFloat("##c", &m_elasticity_coef, 0.05f, 0.0f, 10.0f);

    ImGui::Text("w(t):");
    ImGui::SameLine();
    if (ImGui::BeginCombo("##anchor_func",
                          m_anchor_position_functions[m_selected_anchor_pos_func_idx]->to_string().c_str(),
                          ImGuiComboFlags_None)) {
      for (int i = 0; i < m_anchor_position_functions.size(); ++i) {
        const bool is_selected = (m_selected_anchor_pos_func_idx == i);
        if (ImGui::Selectable(m_anchor_position_functions[i]->to_string().c_str(), is_selected)) {
          m_selected_anchor_pos_func_idx = i;
        }

        if (is_selected) ImGui::SetItemDefaultFocus();
      }
      ImGui::EndCombo();
    }
    m_anchor_position_functions[m_selected_anchor_pos_func_idx]->show_ui();

    ImGui::Text("h(t):");
    ImGui::SameLine();
    if (ImGui::BeginCombo("##field_force_func",
                          m_field_force_function[m_selected_field_force_func_idx]->to_string().c_str(),
                          ImGuiComboFlags_None)) {
      for (int i = 0; i < m_field_force_function.size(); ++i) {
        const bool is_selected = (m_selected_field_force_func_idx == i);
        if (ImGui::Selectable(m_field_force_function[i]->to_string().c_str(), is_selected)) {
          m_selected_field_force_func_idx = i;
        }

        if (is_selected) ImGui::SetItemDefaultFocus();
      }
      ImGui::EndCombo();
    }
    m_field_force_function[m_selected_field_force_func_idx]->show_ui();
  }

  ImGui::EndChild();
}

void SpringUI::show_pos_vel_acc_graph() {
  ImGui::SetNextWindowPos(ImVec2(0.2 * m_window.get_width(), 0.7 * m_window.get_height()));
  ImGui::BeginChild("pos_vel_acc_panel", ImVec2(0.5 * m_window.get_width(), 0.3 * m_window.get_height()),
                    ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
  {
    std::lock_guard<std::mutex> guard(m_ui_mtx);
    if (ImPlot::BeginPlot("Position, velocity and acceleration", ImVec2(-1, -1), ImPlotFlags_Equal)) {
      ImPlot::SetupAxis(ImAxis_Y1, nullptr, ImPlotAxisFlags_AutoFit);
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
  {
    std::lock_guard<std::mutex> guard(m_ui_mtx);
    if (ImPlot::BeginPlot("Forces", ImVec2(-1, -1), ImPlotFlags_Equal)) {
      ImPlot::SetupAxis(ImAxis_Y1, nullptr, ImPlotAxisFlags_AutoFit);
      ImPlot::PlotLine("f(t)", m_time.data(), m_elasticity_force.data(), m_time.size());
      ImPlot::PlotLine("g(t)", m_time.data(), m_damping_force.data(), m_time.size());
      ImPlot::PlotLine("h(t)", m_time.data(), m_field_force.data(), m_time.size());
      ImPlot::PlotLine("w(t)", m_time.data(), m_anchor_position.data(), m_time.size());
    }
    ImPlot::EndPlot();
  }
  ImGui::EndChild();
}

void SpringUI::show_trajectory_graph() {
  ImGui::SetNextWindowPos(ImVec2(0.7 * m_window.get_width(), 0.5 * m_window.get_height()));
  ImGui::BeginChild("trajectory_panel", ImVec2(0.3 * m_window.get_width(), 0.5 * m_window.get_height()),
                    ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
  {
    std::lock_guard<std::mutex> guard(m_ui_mtx);
    if (ImPlot::BeginPlot("Trajectory", ImVec2(-1, -1), ImPlotFlags_Equal)) {
      ImPlot::SetupAxis(ImAxis_X1, nullptr, ImPlotAxisFlags_AutoFit);
      ImPlot::SetupAxis(ImAxis_Y1, nullptr, ImPlotAxisFlags_AutoFit);
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