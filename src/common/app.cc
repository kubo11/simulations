#include "app.hh"

App::App() : m_reference_time(std::chrono::high_resolution_clock::now()) {}

void App::run() {
  while (m_run) {
    auto now = std::chrono::high_resolution_clock::now();
    float dt = std::chrono::duration<float, std::chrono::seconds::period>(now - m_reference_time).count();
    m_reference_time = now;
    update(dt);
  }
}