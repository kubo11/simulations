#ifndef SIMULATIONS_COMMON_APP
#define SIMULATIONS_COMMON_APP

#include "pch.hh"

class App {
 public:
  App();
  virtual ~App() = default;

  void run();

 protected:
  bool m_run = true;

  virtual void update(float dt) = 0;

 private:
  std::chrono::high_resolution_clock::time_point m_reference_time;
};

#endif  // SIMULATIONS_COMMON_APP