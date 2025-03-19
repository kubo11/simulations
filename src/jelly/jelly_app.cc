#include "jelly_app.hh"

JellyApp::JellyApp() : App("Jelly"), m_visualization_mtx() {
  m_camera->rotate(glm::vec2(0.0f, 4.0f), 1e-3f);
  m_window->set_clear_color(glm::vec4{0.0f, 0.0f, 0.0f, 1.0f});

  auto [message_queue_reader, message_queue_writer] = MessageQueue<JellyMessage>::create();
  m_message_queue = std::move(message_queue_reader);

  m_ui = std::make_unique<JellyUI>(*m_window, message_queue_writer);

  m_simulation = std::make_unique<Simulation<Jelly>>(
      m_ui->get_dt(),
      [this](const Jelly& jelly) {
        this->update_visualization_data(jelly);
      });

  m_basic_shader = std::move(ShaderProgram::load("src/jelly/shaders/basic"));
  m_phong_shader = std::move(ShaderProgram::load("src/jelly/shaders/phong"));
  m_bezier_cube_shader = std::move(ShaderProgram::load("src/jelly/shaders/bezier_cube"));
  m_model_shader = std::move(ShaderProgram::load("src/jelly/shaders/model"));
  m_wireframe_shader = std::move(ShaderProgram::load("src/jelly/shaders/wireframe"));

  TextureConfig bezier_cube_texture_config = {};
  m_bezier_cube_texture = Texture::load(m_ui->get_texture_path(), bezier_cube_texture_config);

  m_bounding_box = std::make_unique<ControllableBox>(glm::vec3(3.0f));
  m_bounding_box->set_wireframe_width(1u);
  m_bounding_box->set_solid_color({0.2f, 0.2f, 0.4f, 1.0f});
  m_control_frame = std::make_unique<ControllableBox>();
  m_control_frame->set_wireframe_width(2u);

  auto control_points_indices = std::vector<unsigned int>();
  for (unsigned int i = 0; i < 4; ++i) {
    for (unsigned int j = 0; j < 4; ++j) {
      for (unsigned int k = 0; k < 4; ++k) {
        if (i < 3) {
          control_points_indices.push_back(16 * i + 4 * j + k);
          control_points_indices.push_back(16 * (i+1) + 4 * j + k);
        }
        if (j < 3) {
          control_points_indices.push_back(16 * i + 4 * j + k);
          control_points_indices.push_back(16 * i + 4 * (j+1) + k);
        }
        if (k < 3) {
          control_points_indices.push_back(16 * i + 4 * j + k);
          control_points_indices.push_back(16 * i + 4 * j + (k+1));
        }
      }
    }
  }
  m_control_points_vertex_array = VertexArray<Vertex>::create(std::vector<Vertex>(64), control_points_indices, Vertex::get_vertex_attributes());

  auto control_frame_springs_indices = std::vector<unsigned int>{0, 8, 1, 9, 2, 10, 3, 11, 4, 12, 5, 13, 6, 14, 7, 15};
  m_control_frame_springs_vertex_array = VertexArray<Vertex>::create(std::vector<Vertex>(16), control_frame_springs_indices, Vertex::get_vertex_attributes());

  auto bezier_cube_indices = std::vector<unsigned int>{
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
    51, 50, 49, 48, 55, 54, 53, 52, 59, 58, 57, 56, 63, 62, 61, 60,
    0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60,
    15, 11, 7, 3, 31, 27, 23, 19, 47, 43, 39, 35, 63, 59, 55, 51,
    3, 2, 1, 0, 19, 18, 17, 16, 35, 34, 33, 32, 51, 50, 49, 48,
    12, 13, 14, 15, 28, 29, 30, 31, 44, 45, 46, 47, 60, 61, 62, 63
  };
  m_bezier_cube_vertex_array = VertexArray<Vertex>::create(std::vector<Vertex>(64), bezier_cube_indices, Vertex::get_vertex_attributes());

  m_simulation->apply([&ui = *m_ui, this](Jelly& jelly){
    ui.update_jelly_parameters(jelly);
    jelly.reset();
    this->update_visualization_data(jelly);
  });
}

JellyApp::~JellyApp() { m_simulation->stop(); }

void JellyApp::update(float dt) {
  m_message_queue->foreach([this](auto msg) {
    this->handle_message(std::move(msg));
  });
  m_window->clear();
  render_visualization();
  m_ui->update();
  m_window->update();
}

void JellyApp::render_visualization() {
  std::lock_guard<std::mutex> guard(m_visualization_mtx);

  if (!m_control_points_vertices_staging.empty()) {
    m_control_points_vertex_array->get_vertex_buffer().bind();
    m_control_points_vertex_array->get_vertex_buffer().copy(m_control_points_vertices_staging);
    m_control_points_vertex_array->get_vertex_buffer().unbind();
    m_bezier_cube_vertex_array->get_vertex_buffer().bind();
    m_bezier_cube_vertex_array->get_vertex_buffer().copy(m_control_points_vertices_staging);
    m_bezier_cube_vertex_array->get_vertex_buffer().unbind();
    m_control_points_vertices_staging.clear();
  }

  if (!m_control_frame_springs_vertices_staging.empty()) {
    m_control_frame_springs_vertex_array->get_vertex_buffer().bind();
    m_control_frame_springs_vertex_array->get_vertex_buffer().copy_subregion(0, m_control_frame_springs_vertices_staging);
    m_control_frame_springs_vertex_array->get_vertex_buffer().unbind();
    m_control_frame_springs_vertices_staging.clear();
  }

  m_basic_shader->set_uniform_value("view", m_camera->get_view_matrix());
  m_basic_shader->set_uniform_value("projection", m_camera->get_projection_matrix());
  m_phong_shader->set_uniform_value("view", m_camera->get_view_matrix());
  m_phong_shader->set_uniform_value("projection", m_camera->get_projection_matrix());
  m_bezier_cube_shader->set_uniform_value("view", m_camera->get_view_matrix());
  m_bezier_cube_shader->set_uniform_value("projection", m_camera->get_projection_matrix());
  m_model_shader->set_uniform_value("view", m_camera->get_view_matrix());
  m_model_shader->set_uniform_value("projection", m_camera->get_projection_matrix());
  m_wireframe_shader->set_uniform_value("view", m_camera->get_view_matrix());
  m_wireframe_shader->set_uniform_value("projection", m_camera->get_projection_matrix());

  if (m_ui->show_bounding_box()) {
    glDisable(GL_DEPTH_TEST);
    m_bounding_box->draw_solid(*m_phong_shader);
    glEnable(GL_DEPTH_TEST);
    m_bounding_box->draw_wireframe(*m_wireframe_shader);
  }

  if (m_ui->show_model() && m_model) {
    m_model_shader->bind();
    glUniform3fv(glGetUniformLocation(m_model_shader->get_id(), "points"), 64, (GLfloat*)m_points_vertices.data());
    m_model->draw(*m_model_shader);
    m_model_shader->unbind();
  }

  if (m_ui->show_bezier_cube()) {
    m_bezier_cube_shader->bind();
    m_bezier_cube_texture->bind();
    m_bezier_cube_vertex_array->bind();
    glPatchParameteri(GL_PATCH_VERTICES, 16);
    glDrawElements(GL_PATCHES, m_bezier_cube_vertex_array->get_draw_size(), GL_UNSIGNED_INT, 0);
    m_bezier_cube_vertex_array->unbind();
    m_bezier_cube_shader->unbind();
  }

  glDisable(GL_DEPTH_TEST);
  if (m_ui->show_control_point_springs()) {
    glLineWidth(1);
    m_basic_shader->set_uniform_value("model", glm::mat4(1.0f));
    m_basic_shader->set_uniform_value("color", glm::vec4{0.0f, 0.0f, 0.0f, 1.0f});
    m_basic_shader->bind();
    m_control_points_vertex_array->bind();
    glDrawElements(GL_LINES, m_control_points_vertex_array->get_element_buffer().get_count(), GL_UNSIGNED_INT, 0);
    m_control_points_vertex_array->unbind();
    m_basic_shader->unbind();
  }

  if (m_ui->show_control_frame_springs()) {
    glLineWidth(1);
    m_basic_shader->set_uniform_value("model", glm::mat4(1.0f));
    m_basic_shader->set_uniform_value("color", glm::vec4{0.0f, 0.0f, 0.0f, 1.0f});
    m_basic_shader->bind();
    m_control_frame_springs_vertex_array->bind();
    glDrawElements(GL_LINES, m_control_frame_springs_vertex_array->get_draw_size(), GL_UNSIGNED_INT, 0);
    m_control_frame_springs_vertex_array->unbind();
    m_basic_shader->unbind();
  }

  if (m_ui->show_control_points()) {
    glPointSize(4);
    m_basic_shader->set_uniform_value("model", glm::mat4(1.0f));
    m_basic_shader->set_uniform_value("color", glm::vec4{0.0f, 0.0f, 0.0f, 1.0f});
    m_basic_shader->bind();
    m_control_points_vertex_array->bind();
    glDrawArrays(GL_POINTS, 0, m_control_points_vertex_array->get_vertex_buffer().get_count());
    m_control_points_vertex_array->unbind();
    m_basic_shader->unbind();
  }

  glEnable(GL_DEPTH_TEST);

  if (m_ui->show_control_frame()) {
    m_control_frame->draw_wireframe(*m_wireframe_shader);
  }
}

void JellyApp::update_visualization_data(const Jelly& jelly) {
  std::lock_guard<std::mutex> guard(m_visualization_mtx);

  m_control_points_vertices_staging.reserve(jelly.get_control_point_positions().size());
  for (auto& vert : jelly.get_control_point_positions()) {
    m_control_points_vertices_staging.push_back({vert});
  }

  m_control_frame_springs_vertices_staging = {
    {jelly.get_control_point_position(0, 0, 0)},
    {jelly.get_control_point_position(3, 0, 0)},
    {jelly.get_control_point_position(0, 3, 0)},
    {jelly.get_control_point_position(0, 0, 3)},
    {jelly.get_control_point_position(3, 3, 0)},
    {jelly.get_control_point_position(0, 3, 3)},
    {jelly.get_control_point_position(3, 0, 3)},
    {jelly.get_control_point_position(3, 3, 3)},
  };

  m_points_vertices = jelly.get_control_point_positions();
}

void JellyApp::handle_message(JellyMessage msg) {
  switch (msg) {
  case JellyMessage::Start:
    m_simulation->start();
    break;
  case JellyMessage::Stop:
    m_simulation->stop();
    break;

  case JellyMessage::Restart:
    m_simulation->stop();
    m_simulation->apply([&ui = *m_ui](Jelly& jelly){
      ui.update_jelly_parameters(jelly);
      jelly.reset();
    });
    m_simulation->set_dt(m_ui->get_dt());
    m_simulation->start();
    break;

  case JellyMessage::Apply:
    m_simulation->apply([&ui = *m_ui](Jelly& jelly){
      ui.update_jelly_parameters(jelly);
    });
    m_simulation->set_dt(m_ui->get_dt());
    break;

  case JellyMessage::Skip:
    m_simulation->set_skip_frames(m_ui->get_skip_frames_count());
    break;

  case JellyMessage::Distort:
    m_simulation->apply([](Jelly& jelly){
      jelly.distort();
    });
    break;

  case JellyMessage::UpdateFrame:
    {
      auto rpy = m_ui->get_frame_orientation();
      auto q = glm::quat(glm::vec3{rpy.y, rpy.x, rpy.z});
      auto pos = m_ui->get_frame_position();
      m_control_frame->set_position(pos);
      m_control_frame->set_orientation(q);
      auto size = 1.0f / 2.0f;
      std::vector<Vertex> control_frame_springs_vertices = {
        {pos + glm::vec3{-size, -size, -size}},
        {pos + glm::vec3{+size, -size, -size}},
        {pos + glm::vec3{-size, +size, -size}},
        {pos + glm::vec3{-size, -size, +size}},
        {pos + glm::vec3{+size, +size, -size}},
        {pos + glm::vec3{-size, +size, +size}},
        {pos + glm::vec3{+size, -size, +size}},
        {pos + glm::vec3{+size, +size, +size}}
      };
      m_control_frame_springs_vertex_array->get_vertex_buffer().bind();
      m_control_frame_springs_vertex_array->get_vertex_buffer().copy_subregion(8, control_frame_springs_vertices);
      m_control_frame_springs_vertex_array->get_vertex_buffer().bind();
      m_simulation->apply([&ui = *m_ui, q](Jelly& jelly){
        jelly.set_frame_position(ui.get_frame_position());
        jelly.set_frame_orientation(q);
        jelly.toggle_frame_springs(ui.get_control_springs_state());
      });
    }
    if (m_model) {
      m_model->set_model_mat(m_control_frame->get_model_mat());
    }
    break;

  case JellyMessage::UpdateTexture:
    {
      TextureConfig bezier_cube_texture_config = {};
      m_bezier_cube_texture = Texture::load(m_ui->get_texture_path(), bezier_cube_texture_config);
    }
    break;
  case JellyMessage::UpdateModel:
    m_model = Model::load(m_ui->get_model_path());
    break;
  }
}