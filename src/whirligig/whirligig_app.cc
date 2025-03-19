#include "whirligig_app.hh"

WhirligigApp::WhirligigApp() : App("Whirligig"), m_visualization_mtx() {
  glDisable(GL_DEPTH_TEST);

  m_camera->rotate(glm::vec2(0.0f, 4.0f), 1e-3f);

  auto [message_queue_reader, message_queue_writer] = MessageQueue<WhirligigMessage>::create();
  m_message_queue = std::move(message_queue_reader);

  m_ui = std::make_unique<WhirligigUI>(*m_window, message_queue_writer);

  m_simulation = std::make_unique<Simulation<Whirligig>>(
      m_ui->get_dt(),
      [this](const Whirligig& whirligig) {
        m_ui->update_whirligig_data(whirligig);
        this->update_visualization_data(whirligig);
      });

  m_trajectory_length = m_ui->get_trajectory_length();

  auto cube_vertices = std::vector<NormalVertex>{
      {{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},  {{1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
      {{1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},  {{1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
      {{0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},  {{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
      {{1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},  {{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
      {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},  {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
      {{1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},  {{1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
      {{1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}}, {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
      {{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}}, {{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
      {{1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}}, {{1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
      {{0.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}}, {{0.0f, 0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
      {{0.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}}, {{0.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
      {{0.0f, 1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}}, {{0.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
      {{0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},  {{1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
      {{1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},  {{1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
      {{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},  {{0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
      {{0.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}}, {{1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
      {{1.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}}, {{1.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
      {{0.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}}, {{0.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
  };
  auto cube_vertex_buffer = std::make_unique<Buffer<NormalVertex>>();
  cube_vertex_buffer->bind();
  cube_vertex_buffer->copy(cube_vertices);
  cube_vertex_buffer->unbind();
  m_cube_vertex_array =
      std::make_unique<VertexArray<NormalVertex>>(std::move(cube_vertex_buffer), NormalVertex::get_vertex_attributes());

  auto plane_vertices =
      std::vector<NormalVertex>{{{-1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},  {{1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
                                {{1.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}},  {{1.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}},
                                {{-1.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}}, {{-1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}}};
  auto plane_vertex_buffer = std::make_unique<Buffer<NormalVertex>>();
  plane_vertex_buffer->bind();
  plane_vertex_buffer->copy(plane_vertices);
  plane_vertex_buffer->unbind();
  m_plane_vertex_array = std::make_unique<VertexArray<NormalVertex>>(std::move(plane_vertex_buffer),
                                                                     NormalVertex::get_vertex_attributes());

  auto diagonal_vertices = std::vector<Vertex>{{{0.0f, 0.0f, 0.0f}}, {{1.0f, 1.0f, 1.0f}}};
  auto diagonal_vertex_buffer = std::make_unique<Buffer<Vertex>>();
  diagonal_vertex_buffer->bind();
  diagonal_vertex_buffer->copy(diagonal_vertices);
  diagonal_vertex_buffer->unbind();
  m_diagonal_vertex_array =
      std::make_unique<VertexArray<Vertex>>(std::move(diagonal_vertex_buffer), Vertex::get_vertex_attributes());

  auto gravity_vector_vertices =
      std::vector<Vertex>{{{0.0f, 0.0f, 0.0f}},   {{0.0f, -1.0f, 0.0f}}, {{0.0f, -1.0f, 0.0f}},
                          {{-0.1f, -0.9f, 0.0f}}, {{0.0f, -1.0f, 0.0f}}, {{0.1f, -0.9f, 0.0f}}};
  auto gravity_vector_vertex_buffer = std::make_unique<Buffer<Vertex>>();
  gravity_vector_vertex_buffer->bind();
  gravity_vector_vertex_buffer->copy(gravity_vector_vertices);
  gravity_vector_vertex_buffer->unbind();
  m_gravity_vector_vertex_array =
      std::make_unique<VertexArray<Vertex>>(std::move(gravity_vector_vertex_buffer), Vertex::get_vertex_attributes());

  m_trajectory_vertices = std::vector<Vertex>(m_trajectory_length, {{0.0f, 0.0f, 0.0f}});
  auto trajectory_vertex_buffer = std::make_unique<Buffer<Vertex>>();
  trajectory_vertex_buffer->bind();
  trajectory_vertex_buffer->copy(m_trajectory_vertices);
  trajectory_vertex_buffer->unbind();
  m_trajectory_vertex_array =
      std::make_unique<VertexArray<Vertex>>(std::move(trajectory_vertex_buffer), Vertex::get_vertex_attributes());

  m_basic_shader = std::move(ShaderProgram::load("src/whirligig/shaders/basic"));
  m_phong_shader = std::move(ShaderProgram::load("src/whirligig/shaders/phong"));

  m_simulation->apply([&ui = *m_ui, this](Whirligig& whirligig){
    ui.update_whirligig_parameters(whirligig);
    ui.reset_whirligig(whirligig);
    this->update_visualization_data(whirligig);
  });
}

WhirligigApp::~WhirligigApp() { m_simulation->stop(); }

void WhirligigApp::update(float dt) {
  m_window->clear();
  render_visualization();
  m_ui->update();
  m_window->update();
}

void WhirligigApp::render_visualization() {
  m_message_queue->foreach([this](auto msg) {
    this->handle_message(std::move(msg));
  });

  std::lock_guard<std::mutex> guard(m_visualization_mtx);
  if (m_ui->show_plane()) {
    m_phong_shader->bind();
    m_phong_shader->set_and_commit_uniform_value("model", m_plane_model_matrix);
    m_phong_shader->set_uniform_value("view", m_camera->get_view_matrix());
    m_phong_shader->set_uniform_value("projection", m_camera->get_projection_matrix());
    m_phong_shader->set_and_commit_uniform_value("color", glm::vec4{0.2f, 0.2f, 0.2f, 0.5f});
    m_plane_vertex_array->bind();
    glDisable(GL_CULL_FACE);
    glDrawArrays(GL_TRIANGLES, 0, m_plane_vertex_array->get_draw_size());
    glEnable(GL_CULL_FACE);
    m_plane_vertex_array->unbind();
    m_phong_shader->unbind();
  }

  if (m_ui->show_diagonal()) {
    m_basic_shader->bind();
    m_basic_shader->set_and_commit_uniform_value("model", m_cube_model_matrix);
    m_basic_shader->set_uniform_value("view", m_camera->get_view_matrix());
    m_basic_shader->set_uniform_value("projection", m_camera->get_projection_matrix());
    m_basic_shader->set_and_commit_uniform_value("color", glm::vec4{1.0f, 1.0f, 1.0f, 1.0f});
    m_diagonal_vertex_array->bind();
    glDisable(GL_CULL_FACE);
    glDrawArrays(GL_LINES, 0, m_diagonal_vertex_array->get_draw_size());
    glEnable(GL_CULL_FACE);
    m_diagonal_vertex_array->unbind();
    m_basic_shader->unbind();
  }

  if (m_ui->show_gravity_vector()) {
    m_basic_shader->bind();
    m_basic_shader->set_and_commit_uniform_value("model", m_gravity_vector_model_matrix);
    m_basic_shader->set_uniform_value("view", m_camera->get_view_matrix());
    m_basic_shader->set_uniform_value("projection", m_camera->get_projection_matrix());
    m_basic_shader->set_and_commit_uniform_value("color", glm::vec4{1.0f, 1.0f, 1.0f, 1.0f});
    m_gravity_vector_vertex_array->bind();
    glDisable(GL_CULL_FACE);
    glDrawArrays(GL_LINES, 0, m_gravity_vector_vertex_array->get_draw_size());
    glEnable(GL_CULL_FACE);
    m_gravity_vector_vertex_array->unbind();
    m_basic_shader->unbind();
  }

  if (m_ui->show_trajectory()) {
    m_trajectory_vertex_array->get_vertex_buffer().bind();
    m_trajectory_vertex_array->get_vertex_buffer().copy(m_trajectory_vertices);
    m_trajectory_vertex_array->get_vertex_buffer().unbind();
    m_basic_shader->bind();
    m_basic_shader->set_and_commit_uniform_value("model", glm::mat4(1.0f));
    m_basic_shader->set_uniform_value("view", m_camera->get_view_matrix());
    m_basic_shader->set_uniform_value("projection", m_camera->get_projection_matrix());
    m_basic_shader->set_and_commit_uniform_value("color", glm::vec4{1.0f, 1.0f, 1.0f, 1.0f});
    m_trajectory_vertex_array->bind();
    glDisable(GL_CULL_FACE);
    glDrawArrays(GL_LINE_STRIP, 0, m_trajectory_offset);
    glDrawArrays(GL_LINE_STRIP, m_trajectory_offset, std::max(0u, m_trajectory_vertex_array->get_draw_size() - m_trajectory_offset));
    glEnable(GL_CULL_FACE);
    m_trajectory_vertex_array->unbind();
    m_basic_shader->unbind();
  }

  if (m_ui->show_cube()) {
    m_phong_shader->bind();
    m_phong_shader->set_and_commit_uniform_value("model", m_cube_model_matrix);
    m_phong_shader->set_uniform_value("view", m_camera->get_view_matrix());
    m_phong_shader->set_uniform_value("projection", m_camera->get_projection_matrix());
    m_phong_shader->set_and_commit_uniform_value("color", glm::vec4{0.0f, 0.2f, 0.3f, 0.4f});
    m_cube_vertex_array->bind();
    glDrawArrays(GL_TRIANGLES, 0, m_cube_vertex_array->get_draw_size());
    m_cube_vertex_array->unbind();
    m_phong_shader->unbind();
  }
}

void WhirligigApp::update_visualization_data(const Whirligig& whirligig) {
  std::lock_guard<std::mutex> guard(m_visualization_mtx);
  auto cube_scale = whirligig.get_cube_size();
  auto straight_up_matrix = glm::mat4(glm::angleAxis(glm::radians(-90.0f), glm::vec3(0.5f, 0.0f, -0.5f)));
  m_cube_model_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(cube_scale, cube_scale, cube_scale)) * straight_up_matrix * glm::mat4(whirligig.get_orientation());
  auto diagonal = glm::vec3(m_cube_model_matrix * glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
  auto mass_center = 0.5f * diagonal;
  m_trajectory_vertices[m_trajectory_offset] = Vertex{diagonal};
  m_trajectory_offset = (m_trajectory_offset + 1) % m_trajectory_length;
  if (m_trajectory_offset == 0) {
    m_trajectory_vertices[m_trajectory_offset] = Vertex{diagonal};
    m_trajectory_offset = (m_trajectory_offset + 1) % m_trajectory_length;
  }
  auto scale = whirligig.get_gravity() / 9.81f;
  auto scale_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, scale, 1.0f));
  m_gravity_vector_model_matrix = glm::translate(glm::mat4(1.0f), mass_center) * scale_matrix;
}

void WhirligigApp::handle_message(WhirligigMessage msg) {
  switch (msg) {
  case WhirligigMessage::Start:
    m_simulation->start();
    break;
  case WhirligigMessage::Stop:
    m_simulation->stop();
    break;

  case WhirligigMessage::Restart:
    m_simulation->stop();
    m_simulation->apply([&ui = *m_ui](Whirligig& whirligig){
      ui.update_whirligig_parameters(whirligig);
      ui.reset_whirligig(whirligig);
    });
    m_simulation->set_dt(m_ui->get_dt());
    m_trajectory_length = m_ui->get_trajectory_length();
    m_trajectory_offset = 0;
    m_trajectory_vertices.clear();
    m_trajectory_vertices.resize(m_trajectory_length, Vertex{{0.0f, 0.0f, 0.0f}});
    m_simulation->start();
    break;

  case WhirligigMessage::Apply:
    m_simulation->apply([&ui = *m_ui](Whirligig& whirligig){
      ui.update_whirligig_parameters(whirligig);
    });
    m_simulation->set_dt(m_ui->get_dt());
    m_trajectory_length = m_ui->get_trajectory_length();
    m_trajectory_offset = 0;
    m_trajectory_vertices.clear();
    m_trajectory_vertices.resize(m_trajectory_length, Vertex{{0.0f, 0.0f, 0.0f}});
    break;

  case WhirligigMessage::Skip:
    m_simulation->set_skip_frames(m_ui->get_skip_frames_count());
    break;

  default:
    break;
  }
}