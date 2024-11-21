#include "jelly_app.hh"

JellyApp::JellyApp() : App("Jelly"), m_visualization_mtx() {
  m_camera->rotate(glm::vec2(0.0f, 4.0f), 1e-3f);

  auto [message_queue_reader, message_queue_writer] = MessageQueue<JellyMessage>::create();
  m_message_queue = std::move(message_queue_reader);

  m_ui = std::make_unique<JellyUI>(*m_window, message_queue_writer);

  m_simulation = std::make_unique<Simulation<Jelly>>(
      m_ui->get_dt(),
      [this](const Jelly& jelly) {
        m_ui->update_jelly_data(jelly);
        this->update_visualization_data(jelly);
      });

  m_simulation->apply([&ui = *m_ui, this](Jelly& jelly){
    ui.update_jelly_parameters(jelly);
    ui.reset_jelly(jelly);
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
}

void JellyApp::update_visualization_data(const Jelly& jelly) {
  std::lock_guard<std::mutex> guard(m_visualization_mtx);
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
      ui.reset_jelly(jelly);
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

  default:
    break;
  }
}