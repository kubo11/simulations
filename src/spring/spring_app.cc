#include "spring_app.hh"

SpringApp::SpringApp() : App("Spring") {
  m_framebuffer = std::make_unique<Framebuffer>(m_window->get_width(), m_window->get_height());
  glfwSetFramebufferSizeCallback(m_window->get_instance(), SpringApp::framebuffer_resize_callback);

  auto [message_queue_reader, message_queue_writer] = MessageQueue<SpringMessage>::create();
  m_message_queue = std::move(message_queue_reader);

  m_ui = std::make_unique<SpringUI>(*m_window, *m_framebuffer, message_queue_writer);

  m_spring_simulation = std::make_unique<Simulation<Spring>>(
      m_ui->get_dt(), [this](const Spring& spring) { m_ui->update_spring_data(spring); this->update_visualization_data(spring); });


  m_spring_vertex_array = std::make_unique<VertexArray<int>>();
  m_spring_shader = std::move(ShaderProgram::load("src/spring/shaders/spring"));
  m_weight_shader = std::move(ShaderProgram::load("src/spring/shaders/weight"));
  auto vertices = std::vector<WeightVertex>{
      {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},   {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
      {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},     {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
      {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},    {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
      {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},    {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
      {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},    {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
      {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},     {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
      {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},  {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
      {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},  {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
      {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},   {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
      {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}}, {{-0.5f, -0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}},
      {{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}},   {{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}},
      {{-0.5f, 0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}},  {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}},
      {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},    {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
      {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},    {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
      {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},   {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
      {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}}, {{0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}},
      {{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}},   {{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}},
      {{-0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}},  {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}},
  };
  auto vertex_buffer = std::make_unique<Buffer<WeightVertex>>();
  vertex_buffer->bind();
  vertex_buffer->copy(vertices);
  vertex_buffer->unbind();
  m_weight_vertex_array =
      std::make_unique<VertexArray<WeightVertex>>(std::move(vertex_buffer), WeightVertex::get_vertex_attributes());
}

SpringApp::~SpringApp() { m_spring_simulation->stop(); }

void SpringApp::update(float dt) {
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

void SpringApp::render_visualization() {
  m_spring_shader->bind();
  m_spring_shader->set_uniform_value("model", m_spring_model_mat);
  m_spring_shader->set_uniform_value("view", m_camera->get_view_matrix());
  m_spring_shader->set_uniform_value("projection", m_camera->get_projection_matrix());
  m_spring_shader->set_uniform_value("h0", m_spring_height);
  m_spring_shader->set_uniform_value("l", 10.0f);
  m_spring_shader->set_uniform_value("r", 0.2f);
  m_spring_shader->set_uniform_value("rsmall", 0.05f);
  m_spring_shader->set_uniform_value("count", 400.0f);
  m_spring_vertex_array->bind();
  glDrawArrays(GL_LINE_STRIP, 0, 400);
  glCheckError();
  m_weight_shader->bind();
  m_weight_shader->set_uniform_value("model", m_weight_model_mat);
  m_weight_shader->set_uniform_value("view", m_camera->get_view_matrix());
  m_weight_shader->set_uniform_value("projection", m_camera->get_projection_matrix());
  m_weight_vertex_array->bind();
  glDrawArrays(GL_TRIANGLES, 0, m_weight_vertex_array->get_draw_size());
  m_weight_vertex_array->unbind();
  m_weight_shader->unbind();
}

void SpringApp::framebuffer_resize_callback(GLFWwindow* window, int width, int height) {
  App::framebuffer_resize_callback(window, width, height);
  SpringApp* app = static_cast<SpringApp*>(glfwGetWindowUserPointer(window));
  app->m_framebuffer->resize(width, height);
}

void SpringApp::update_visualization_data(const Spring& spring) {
  m_weight_model_mat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, spring.get_weight_position(), 0.0f));
  float anchor_pos_y = 2.5f + spring.get_rest_position();
  m_spring_height = anchor_pos_y - spring.get_weight_position();
  m_spring_model_mat =
      glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, -1.0f, 1.0f)), glm::vec3(0.0f, -anchor_pos_y, 0.0f));
}

void SpringApp::handle_message(SpringMessage msg) {
  switch (msg) {
  case SpringMessage::Start:
    m_spring_simulation->start();
    break;
  case SpringMessage::Stop:
    m_spring_simulation->stop();
    break;

  case SpringMessage::Restart:
    m_spring_simulation->stop();
    m_spring_simulation->apply([&ui = *m_ui](Spring& spring){
      ui.update_spring_parameters(spring);
      ui.reset_spring(spring);
    });
    m_spring_simulation->set_dt(m_ui->get_dt());
    m_ui->clear();
    m_spring_simulation->start();
    break;

  case SpringMessage::Apply:
    m_spring_simulation->apply([&ui = *m_ui](Spring& spring){
      ui.update_spring_parameters(spring);
    });
    m_spring_simulation->set_dt(m_ui->get_dt());
    break;

  case SpringMessage::Skip:
    m_spring_simulation->set_skip_frames(m_ui->get_skip_frames_count());
    break;

  default:
    break;
  }
}