#include "blackhole_app.hh"

BlackHoleApp::BlackHoleApp() : App("BlackHole"), m_visualization_mtx() {
  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glDisable(GL_CULL_FACE);

  m_camera->set_max_dist(1e4f);
  m_camera->set_distance(1e3f);
  m_window->set_clear_color(glm::vec4{0.0f, 0.0f, 0.0f, 1.0f});

  auto [message_queue_reader, message_queue_writer] = MessageQueue<BlackHoleMessage>::create();
  m_message_queue = std::move(message_queue_reader);

  m_ui = std::make_unique<BlackHoleUI>(*m_window, message_queue_writer);

  m_shader = std::move(ShaderProgram::load("src/blackhole/shaders/shader"));

  CubemapTextureConfig config;
  config.wrapping_u = TextureWrapping::ClampToEdge;
  config.wrapping_v = TextureWrapping::ClampToEdge;
  config.wrapping_w = TextureWrapping::ClampToEdge;
  config.generate_mipmaps = true;
  m_skybox = CubemapTexture::load({
    "resources/textures/right.png",
    "resources/textures/left.png",
    "resources/textures/top.png",
    "resources/textures/bottom.png",
    "resources/textures/front.png",
    "resources/textures/back.png"
  }, config);

  auto vertices = std::vector<UVVertex>{
    {{-1.0f, -1.0f}}, {{3.0f, -1.0f}}, {{-1.0f, 3.0f}}
  };
  m_fullscreen_triangle_vertex_array = VertexArray<UVVertex>::create(vertices, UVVertex::get_vertex_attributes());

  handle_message(BlackHoleMessage::UpdateDistance);
  handle_message(BlackHoleMessage::UpdateMass);
}

void BlackHoleApp::update(float dt) {
  m_message_queue->foreach([this](auto msg) {
    this->handle_message(std::move(msg));
  });
  m_window->clear();
  render_visualization();
  m_ui->update();
  m_window->update();
}

void BlackHoleApp::render_visualization() {
  std::lock_guard<std::mutex> guard(m_visualization_mtx);

  m_shader->set_uniform_value("invProjView", glm::inverse(m_camera->get_projection_matrix() * m_camera->get_view_matrix()));
  m_shader->set_uniform_value("cameraPosition", m_camera->get_position());
  m_shader->set_uniform_value("skybox", 0);

  m_shader->bind();
  m_skybox->bind(0);
  m_fullscreen_triangle_vertex_array->bind();
  glDrawArrays(GL_TRIANGLES, 0, m_fullscreen_triangle_vertex_array->get_draw_size());
  m_fullscreen_triangle_vertex_array->unbind();
  m_skybox->unbind(0);
  m_shader->unbind();
}

void BlackHoleApp::handle_message(BlackHoleMessage msg) {
  switch (msg) {
  case BlackHoleMessage::UpdateDistance:
    m_camera->set_distance(m_ui->get_distance());
    break;
  case BlackHoleMessage::UpdateMass:
    m_shader->set_uniform_value("blackHoleMass", m_ui->get_mass());
    break;
  }
}