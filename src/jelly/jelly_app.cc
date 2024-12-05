#include "jelly_app.hh"

JellyApp::JellyApp() : App("Jelly"), m_visualization_mtx() {
  m_camera->rotate(glm::vec2(0.0f, 4.0f), 1e-3f);

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

  auto control_points_vertices = std::vector<Vertex>(64);
  auto control_points_vertex_buffer = std::make_unique<Buffer<Vertex>>();
  control_points_vertex_buffer->bind();
  control_points_vertex_buffer->copy(control_points_vertices);
  control_points_vertex_buffer->unbind();
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
  auto control_points_index_buffer = std::make_unique<Buffer<unsigned int>>(BufferType::ElementArray);
  control_points_index_buffer->bind();
  control_points_index_buffer->copy(control_points_indices);
  control_points_index_buffer->unbind();
  m_control_points_vertex_array =
      std::make_unique<VertexArray<Vertex>>(std::move(control_points_vertex_buffer), Vertex::get_vertex_attributes(), std::move(control_points_index_buffer));

  auto control_frame_vertices = std::vector<Vertex>{
    {{-0.5f, -0.5f, +0.5f}},
    {{+0.5f, -0.5f, +0.5f}},
    {{+0.5f, +0.5f, +0.5f}},
    {{-0.5f, +0.5f, +0.5f}},
    {{-0.5f, -0.5f, -0.5f}},
    {{+0.5f, -0.5f, -0.5f}},
    {{+0.5f, +0.5f, -0.5f}},
    {{-0.5f, +0.5f, -0.5f}},
  };
  auto control_frame_vertex_buffer = std::make_unique<Buffer<Vertex>>();
  control_frame_vertex_buffer->bind();
  control_frame_vertex_buffer->copy(control_frame_vertices);
  control_frame_vertex_buffer->unbind();
  auto control_frame_indices = std::vector<unsigned int>{0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6, 6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7};
  auto control_frame_index_buffer = std::make_unique<Buffer<unsigned int>>(BufferType::ElementArray);
  control_frame_index_buffer->bind();
  control_frame_index_buffer->copy(control_frame_indices);
  control_frame_index_buffer->unbind();
  m_control_frame_vertex_array =
      std::make_unique<VertexArray<Vertex>>(std::move(control_frame_vertex_buffer), Vertex::get_vertex_attributes(), std::move(control_frame_index_buffer));

  auto control_frame_springs_vertices = std::vector<Vertex>(16);
  auto control_frame_springs_vertex_buffer = std::make_unique<Buffer<Vertex>>();
  control_frame_springs_vertex_buffer->bind();
  control_frame_springs_vertex_buffer->copy(control_frame_springs_vertices);
  control_frame_springs_vertex_buffer->unbind();
  auto control_frame_springs_indices = std::vector<unsigned int>{0, 8, 1, 9, 2, 10, 3, 11, 4, 12, 5, 13, 6, 14, 7, 15};
  auto control_frame_springs_index_buffer = std::make_unique<Buffer<unsigned int>>(BufferType::ElementArray);
  control_frame_springs_index_buffer->bind();
  control_frame_springs_index_buffer->copy(control_frame_springs_indices);
  control_frame_springs_index_buffer->unbind();
  m_control_frame_springs_vertex_array =
      std::make_unique<VertexArray<Vertex>>(std::move(control_frame_springs_vertex_buffer), Vertex::get_vertex_attributes(), std::move(control_frame_springs_index_buffer));

  auto bounding_box_vertices = std::vector<NormalVertex>{
      {{-0.5f, -0.5f, +0.5f}, {+0.0f, +0.0f, -1.0f}}, {{+0.5f, +0.5f, +0.5f}, {+0.0f, +0.0f, -1.0f}}, {{+0.5f, -0.5f, +0.5f}, {+0.0f, +0.0f, -1.0f}},
      {{+0.5f, +0.5f, +0.5f}, {+0.0f, +0.0f, -1.0f}}, {{-0.5f, -0.5f, +0.5f}, {+0.0f, +0.0f, -1.0f}}, {{-0.5f, +0.5f, +0.5f}, {+0.0f, +0.0f, -1.0f}},
      {{+0.5f, -0.5f, +0.5f}, {-1.0f, +0.0f, +0.0f}}, {{+0.5f, +0.5f, -0.5f}, {-1.0f, +0.0f, +0.0f}}, {{+0.5f, -0.5f, -0.5f}, {-1.0f, +0.0f, +0.0f}},
      {{+0.5f, +0.5f, -0.5f}, {-1.0f, +0.0f, +0.0f}}, {{+0.5f, -0.5f, +0.5f}, {-1.0f, +0.0f, +0.0f}}, {{+0.5f, +0.5f, +0.5f}, {-1.0f, +0.0f, +0.0f}},
      {{+0.5f, -0.5f, -0.5f}, {+0.0f, +0.0f, +1.0f}}, {{-0.5f, +0.5f, -0.5f}, {+0.0f, +0.0f, +1.0f}}, {{-0.5f, -0.5f, -0.5f}, {+0.0f, +0.0f, +1.0f}},
      {{-0.5f, +0.5f, -0.5f}, {+0.0f, +0.0f, +1.0f}}, {{+0.5f, -0.5f, -0.5f}, {+0.0f, +0.0f, +1.0f}}, {{+0.5f, +0.5f, -0.5f}, {+0.0f, +0.0f, +1.0f}},
      {{-0.5f, -0.5f, -0.5f}, {+1.0f, +0.0f, +0.0f}}, {{-0.5f, +0.5f, +0.5f}, {+1.0f, +0.0f, +0.0f}}, {{-0.5f, -0.5f, +0.5f}, {+1.0f, +0.0f, +0.0f}},
      {{-0.5f, +0.5f, +0.5f}, {+1.0f, +0.0f, +0.0f}}, {{-0.5f, -0.5f, -0.5f}, {+1.0f, +0.0f, +0.0f}}, {{-0.5f, +0.5f, -0.5f}, {+1.0f, +0.0f, +0.0f}},
      {{-0.5f, +0.5f, +0.5f}, {+0.0f, -1.0f, +0.0f}}, {{+0.5f, +0.5f, -0.5f}, {+0.0f, -1.0f, +0.0f}}, {{+0.5f, +0.5f, +0.5f}, {+0.0f, -1.0f, +0.0f}},
      {{+0.5f, +0.5f, -0.5f}, {+0.0f, -1.0f, +0.0f}}, {{-0.5f, +0.5f, +0.5f}, {+0.0f, -1.0f, +0.0f}}, {{-0.5f, +0.5f, -0.5f}, {+0.0f, -1.0f, +0.0f}},
      {{-0.5f, -0.5f, -0.5f}, {+0.0f, +1.0f, +0.0f}}, {{+0.5f, -0.5f, +0.5f}, {+0.0f, +1.0f, +0.0f}}, {{+0.5f, -0.5f, -0.5f}, {+0.0f, +1.0f, +0.0f}},
      {{+0.5f, -0.5f, +0.5f}, {+0.0f, +1.0f, +0.0f}}, {{-0.5f, -0.5f, -0.5f}, {+0.0f, +1.0f, +0.0f}}, {{-0.5f, -0.5f, +0.5f}, {+0.0f, +1.0f, +0.0f}},
  };
  auto bounding_box_vertex_buffer = std::make_unique<Buffer<NormalVertex>>();
  bounding_box_vertex_buffer->bind();
  bounding_box_vertex_buffer->copy(bounding_box_vertices);
  bounding_box_vertex_buffer->unbind();
  m_bounding_box_vertex_array =
      std::make_unique<VertexArray<NormalVertex>>(std::move(bounding_box_vertex_buffer), NormalVertex::get_vertex_attributes());

  m_simulation->apply([&ui = *m_ui, this](Jelly& jelly){
    ui.update_jelly_parameters(jelly);
    jelly.reset();
    this->update_visualization_data(jelly);
  });
}

JellyApp::~JellyApp() { m_simulation->stop(); }

void JellyApp::update(float dt) {
  m_window->clear();
  render_visualization();
  m_ui->update();
  m_window->update();
}

void JellyApp::render_visualization() {
  m_message_queue->foreach([this](auto msg) {
    this->handle_message(std::move(msg));
  });

  std::lock_guard<std::mutex> guard(m_visualization_mtx);

  if (!m_control_points_vertices_staging.empty()) {
    m_control_points_vertex_array->get_vertex_buffer().bind();
    m_control_points_vertex_array->get_vertex_buffer().copy(m_control_points_vertices_staging);
    m_control_points_vertex_array->get_vertex_buffer().unbind();
    m_control_points_vertices_staging.clear();
  }

  if (!m_control_frame_springs_vertices_staging.empty()) {
    m_control_frame_springs_vertex_array->get_vertex_buffer().bind();
    m_control_frame_springs_vertex_array->get_vertex_buffer().copy_subregion(0, m_control_frame_springs_vertices_staging);
    m_control_frame_springs_vertex_array->get_vertex_buffer().unbind();
    m_control_frame_springs_vertices_staging.clear();
  }

  if (m_ui->show_control_points()) {
    glPointSize(4);
    m_basic_shader->bind();
    m_basic_shader->set_and_commit_uniform_value("model", glm::mat4(1.0f));
    m_basic_shader->set_uniform_value("view", m_camera->get_view_matrix());
    m_basic_shader->set_uniform_value("projection", m_camera->get_projection_matrix());
    m_basic_shader->set_and_commit_uniform_value("color", glm::vec4{0.0f, 0.0f, 0.0f, 1.0f});
    m_control_points_vertex_array->bind();
    glDrawArrays(GL_POINTS, 0, m_control_points_vertex_array->get_vertex_buffer().get_count());
    m_control_points_vertex_array->unbind();
    m_basic_shader->unbind();
  }

  if (m_ui->show_control_point_springs()) {
    glLineWidth(1);
    m_basic_shader->bind();
    m_basic_shader->set_and_commit_uniform_value("model", glm::mat4(1.0f));
    m_basic_shader->set_uniform_value("view", m_camera->get_view_matrix());
    m_basic_shader->set_uniform_value("projection", m_camera->get_projection_matrix());
    m_basic_shader->set_and_commit_uniform_value("color", glm::vec4{0.0f, 0.0f, 0.0f, 1.0f});
    m_control_points_vertex_array->bind();
    glDrawElements(GL_LINES, m_control_points_vertex_array->get_element_buffer().get_count(), GL_UNSIGNED_INT, 0);
    m_control_points_vertex_array->unbind();
    m_basic_shader->unbind();
  }

  if (m_ui->show_control_frame()) {
    glLineWidth(2);
    m_basic_shader->bind();
    m_basic_shader->set_and_commit_uniform_value("model", m_control_frame_model_mat);
    m_basic_shader->set_uniform_value("view", m_camera->get_view_matrix());
    m_basic_shader->set_uniform_value("projection", m_camera->get_projection_matrix());
    m_basic_shader->set_and_commit_uniform_value("color", glm::vec4{0.0f, 0.0f, 0.0f, 1.0f});
    m_control_frame_vertex_array->bind();
    glDrawElements(GL_LINES, m_control_frame_vertex_array->get_draw_size(), GL_UNSIGNED_INT, 0);
    m_control_frame_vertex_array->unbind();
    m_basic_shader->unbind();
  }

  if (m_ui->show_control_frame_springs()) {
    glLineWidth(1);
    m_basic_shader->bind();
    m_basic_shader->set_and_commit_uniform_value("model", glm::mat4(1.0f));
    m_basic_shader->set_uniform_value("view", m_camera->get_view_matrix());
    m_basic_shader->set_uniform_value("projection", m_camera->get_projection_matrix());
    m_basic_shader->set_and_commit_uniform_value("color", glm::vec4{0.0f, 0.0f, 0.0f, 1.0f});
    m_control_frame_springs_vertex_array->bind();
    glDrawElements(GL_LINES, m_control_frame_springs_vertex_array->get_draw_size(), GL_UNSIGNED_INT, 0);
    m_control_frame_springs_vertex_array->unbind();
    m_basic_shader->unbind();
  }

  if (m_ui->show_bounding_box()) {
    m_phong_shader->bind();
    m_phong_shader->set_and_commit_uniform_value("model", m_bounding_box_model_mat);
    m_phong_shader->set_uniform_value("view", m_camera->get_view_matrix());
    m_phong_shader->set_uniform_value("projection", m_camera->get_projection_matrix());
    m_phong_shader->set_and_commit_uniform_value("color", glm::vec4{0.1f, 0.1f, 0.1f, 0.3f});
    m_bounding_box_vertex_array->bind();
    glDrawArrays(GL_TRIANGLES, 0, m_bounding_box_vertex_array->get_draw_size());
    m_bounding_box_vertex_array->unbind();
    m_phong_shader->unbind();
  }

  if (m_ui->show_bezier_cube()) {

  }

  if (m_ui->show_model()) {

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
    auto rpy = m_ui->get_frame_orientation();
    auto q = glm::quat(glm::vec3{rpy.y, rpy.x, rpy.z});
    auto pos = m_ui->get_frame_position();
    m_control_frame_model_mat = glm::translate(glm::mat4(1.0f), pos) * glm::mat4(q);
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
    break;
  }
}