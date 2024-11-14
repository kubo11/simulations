#ifndef SIMULATIONS_COMMON_SIMULATION
#define SIMULATIONS_COMMON_SIMULATION

#include "pch.hh"

template <typename T>
concept simulatable = requires(T v, float dt) {
  requires std::default_initializable<T>;
  { v.update(dt) };
};

template<simulatable T>
using UpdateCallback = std::function<void(const T&)>;

template<simulatable T>
using ApplyCallback = std::function<void(T&)>;

template<simulatable T>
class Simulation {
 public:
  Simulation(float dt, const UpdateCallback<T>& callback) : m_dt(dt), m_update_callback(callback), m_simulation_thread(), m_simulation_mtx(), m_simulatable() {}

  virtual ~Simulation() { stop(); }

  void start() {
    std::lock_guard<std::mutex> guard(m_simulation_mtx);
    if (m_run) return;
    m_run = true;
    m_simulation_thread = std::thread(&Simulation<T>::loop, this);
  }

  void stop() {
    {
      std::lock_guard<std::mutex> guard(m_simulation_mtx);
      if (!m_run) return;
      m_run = false;
    }
    m_simulation_thread.join();
  }

  void apply(const ApplyCallback<T>& apply_func) {
    std::lock_guard<std::mutex> guard(m_simulation_mtx);
    apply_func(m_simulatable);
    m_update_callback(m_simulatable);
  }

  void set_skip_frames(unsigned int count) {
    std::lock_guard<std::mutex> guard(m_simulation_mtx);
    if (count > m_skip_frames_count) m_skip_frames_count = count;
  }

  void set_dt(float dt) {
    std::lock_guard<std::mutex> guard(m_simulation_mtx);
    m_dt = dt;
  }

 private:
  float m_dt = 0.001f;
  bool m_run = false;
  unsigned int m_skip_frames_count = 0u;
  std::thread m_simulation_thread;
  std::mutex m_simulation_mtx;
  T m_simulatable;
  const UpdateCallback<T> m_update_callback;

  void loop() {
    static auto prev_time = std::chrono::high_resolution_clock::now();
    while (true) {
      auto curr_time = std::chrono::high_resolution_clock::now();
      auto loop_dt = std::chrono::duration<float, std::chrono::seconds::period>(curr_time - prev_time).count();
      prev_time = curr_time;
      {
        std::lock_guard<std::mutex> guard(m_simulation_mtx);

        if (!m_run) return;
        m_simulatable.update(loop_dt);
        m_update_callback(m_simulatable);
        if (m_skip_frames_count > 0) {
          --m_skip_frames_count;
          continue;
        }
      }
      curr_time = std::chrono::high_resolution_clock::now();
      auto step_dt = std::chrono::duration<float, std::chrono::seconds::period>(curr_time - prev_time).count();
      if (step_dt < m_dt) {
        std::this_thread::sleep_for(std::chrono::duration<float, std::chrono::seconds::period>(m_dt - step_dt));
      }
    }
  }
};

#endif  // SIMULATIONS_COMMON_SIMULATION