#ifndef SIMULATIONS_SPRING_SIMULATION
#define SIMULATIONS_SPRING_SIMULATION

#include "pch.hh"

#include "simulation.hh"
#include "spring.hh"

class SpringSimulation : public Simulation {
 public:
  SpringSimulation(float dt, std::function<void(void)> callback, Spring& spring);
  virtual ~SpringSimulation() override = default;

  virtual void loop() override;

 private:
  Spring& m_spring;
};

#endif  // SIMULATIONS_SPRING_SIMULATION