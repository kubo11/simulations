#include "flywheel_app.hh"

FlywheelApp::FlywheelApp() : App("Flywheel") {
  m_framebuffer = std::make_unique<Framebuffer>(m_frame_size_multiplier.x * m_window->get_width(), m_frame_size_multiplier.y * m_window->get_height());
  glfwSetFramebufferSizeCallback(m_window->get_instance(), FlywheelApp::framebuffer_resize_callback);

  m_camera->set_aspect_ratio(m_framebuffer->get_aspect_ratio());
  m_camera->set_fov(glm::degrees(2.0f * atanf( tanf(glm::radians(90.0f) * 0.5f) / m_framebuffer->get_aspect_ratio() )));
  m_camera->set_rotation({glm::half_pi<float>(), 0.0f});
  m_camera->set_distance(6.0f);
  m_camera->set_max_dist(20.0f);

  auto [message_queue_reader, message_queue_writer] = MessageQueue<FlywheelMessage>::create();
  m_message_queue = std::move(message_queue_reader);

  m_ui = std::make_unique<FlywheelUI>(*m_window, *m_framebuffer, message_queue_writer);

  m_flywheel_simulation = std::make_unique<Simulation<Flywheel>>(
      m_ui->get_dt(), [this](const Flywheel& flywheel) { m_ui->update_flywheel_data(flywheel); this->update_visualization_data(flywheel); });

  m_shader = std::move(ShaderProgram::load("src/flywheel/shaders/phong"));
  m_flywheel_model = Model::load(fs::current_path() / "resources" / "meshes" / "flywheel.obj");
  m_flywheel_model->get_material().set_color(Colors::Orange);
  m_handle_model = Model::load(fs::current_path() / "resources" / "meshes" / "handle.obj");
  m_handle_model->get_material().set_color(Colors::Orange);
  m_piston_model = Model::load(fs::current_path() / "resources" / "meshes" / "piston.obj");
  m_piston_model->get_material().set_color(Colors::Orange);

  m_flywheel_simulation->apply([&](auto& flywheel){
    m_ui->update_flywheel_parameters(flywheel);
    this->update_visualization_data(flywheel);
    flywheel.reset();
  });
}

FlywheelApp::~FlywheelApp() { m_flywheel_simulation->stop(); }

void FlywheelApp::update(float dt) {
  m_message_queue->foreach([this](auto msg) {
    this->handle_message(std::move(msg));
  });

  m_window->clear();
  m_ui->update();
  m_framebuffer->bind();
  m_window->clear();
  render_visualization();
  m_framebuffer->unbind();
  m_window->update();
}

void FlywheelApp::render_visualization() {
  m_shader->set_uniform_value("view", m_camera->get_view_matrix());
  m_shader->set_uniform_value("projection", m_camera->get_projection_matrix());
  m_shader->bind();
  m_flywheel_model->draw(*m_shader);
  m_handle_model->draw(*m_shader);
  m_piston_model->draw(*m_shader);
  m_shader->unbind();
}

void FlywheelApp::framebuffer_resize_callback(GLFWwindow* window, int width, int height) {
  App::framebuffer_resize_callback(window, width, height);
  FlywheelApp* app = static_cast<FlywheelApp*>(glfwGetWindowUserPointer(window));
  app->m_framebuffer->resize(m_frame_size_multiplier.x * width, m_frame_size_multiplier.y * height);
}

void FlywheelApp::update_visualization_data(const Flywheel& flywheel) {
  {
    auto offset = (flywheel.get_length() + 1.0f) / 2.0f;
    auto model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, offset));
    model = glm::scale(model, glm::vec3(1.0f, flywheel.get_radius(), flywheel.get_radius()));
    m_flywheel_model->set_model_mat(model);
  }
  {
    float py = std::sin(flywheel.get_angle()) * flywheel.get_radius();
    float s =  py / flywheel.get_current_length();
    auto offset = -flywheel.get_piston_position() + (flywheel.get_length() + 1.0f) / 2.0f;
    auto model = glm::translate(glm::mat4(1.0f), glm::vec3(0.35f, 0.0f, offset));
    model = glm::rotate(model, static_cast<float>(-std::asin(s)), {1.0f, 0.0f, 0.0f});
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, flywheel.get_current_length()));
    m_handle_model->set_model_mat(model);
  }
  {
    auto offset = -0.5f-flywheel.get_piston_position() + (flywheel.get_length() + 1.0f) / 2.0f;
    auto model = glm::translate(glm::mat4(1.0f), glm::vec3(0.35f, 0.0f, offset));
    m_piston_model->set_model_mat(model);
  }
}

void FlywheelApp::handle_message(FlywheelMessage msg) {
  switch (msg) {
  case FlywheelMessage::Start:
    m_flywheel_simulation->start();
    break;
  case FlywheelMessage::Stop:
    m_flywheel_simulation->stop();
    break;

  case FlywheelMessage::Restart:
    m_flywheel_simulation->stop();
    m_flywheel_simulation->apply([&ui = *m_ui](Flywheel& flywheel){
      ui.update_flywheel_parameters(flywheel);
      ui.reset_flywheel(flywheel);
    });
    m_flywheel_simulation->set_dt(m_ui->get_dt());
    m_ui->clear();
    m_flywheel_simulation->start();
    break;

  case FlywheelMessage::Apply:
    m_flywheel_simulation->apply([&ui = *m_ui, &sim = *m_flywheel_simulation](Flywheel& flywheel){
      ui.update_flywheel_parameters(flywheel);
      if (!sim.is_running()) flywheel.update(ui.get_dt());
    });
    m_flywheel_simulation->set_dt(m_ui->get_dt());
    break;

  case FlywheelMessage::Skip:
    m_flywheel_simulation->set_skip_frames(m_ui->get_skip_frames_count());
    break;

  default:
    break;
  }
}