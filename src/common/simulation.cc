#include "simulation.hh"

Simulation::Simulation(float dt, std::function<void(void)> callback)
    : m_dt(dt), m_update_callback(callback), m_simulation_thread(), m_simulation_mtx() {}

Simulation::~Simulation() { stop(); }

void Simulation::start() {
  {
    std::lock_guard<std::mutex> guard(m_simulation_mtx);
    if (m_run) return;
    m_run = true;
  }
  m_simulation_thread = std::thread(&Simulation::_loop, this);
}

void Simulation::stop() {
  {
    std::lock_guard<std::mutex> guard(m_simulation_mtx);
    if (!m_run) return;
    m_run = false;
  }
  m_simulation_thread.join();
}

void Simulation::add_skip_frames(unsigned int count) {
  {
    std::lock_guard<std::mutex> guard(m_simulation_mtx);
    m_skip_frames_count += count;
  }
}

void Simulation::set_dt(float dt) {
  {
    std::lock_guard<std::mutex> guard(m_simulation_mtx);
    m_dt = dt;
  }
}

void Simulation::_loop() {
  while (true) {
    auto beg = std::chrono::high_resolution_clock::now();
    loop();
    m_update_callback();
    {
      std::lock_guard<std::mutex> guard(m_simulation_mtx);
      if (!m_run) return;
      if (m_skip_frames_count > 0) {
        --m_skip_frames_count;
        continue;
      }
    }
    auto end = std::chrono::high_resolution_clock::now();
    float loop_runtime = std::chrono::duration<float, std::chrono::seconds::period>(beg - end).count();
    if (loop_runtime > m_dt) {
      std::cout << "Simulation running late: " << std::to_string(loop_runtime - m_dt) << std::endl;
    }
    if (loop_runtime < m_dt) {
      std::this_thread::sleep_for(std::chrono::duration<float, std::chrono::seconds::period>(m_dt - loop_runtime));
    }
  }
}