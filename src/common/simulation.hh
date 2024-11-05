#ifndef SIMULATIONS_COMMON_SIMULATION
#define SIMULATIONS_COMMON_SIMULATION

#include "pch.hh"

class Simulation {
 public:
  Simulation(float dt, std::function<void(void)> callback);
  virtual ~Simulation();

  virtual void loop() = 0;
  void start();
  void stop();
  void add_skip_frames(unsigned int count);
  void set_dt(float dt);

 protected:
  float m_dt;

 private:
  unsigned int m_skip_frames_count = 0u;
  bool m_run = false;
  std::thread m_simulation_thread;
  std::mutex m_simulation_mtx;
  const std::function<void(void)> m_update_callback;

  void _loop();
};

#endif  // SIMULATIONS_COMMON_SIMULATION