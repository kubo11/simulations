#include "spring_simulation.hh"

SpringSimulation::SpringSimulation(float dt, std::function<void(void)> callback, Spring& spring) : Simulation(dt, callback), m_spring(spring) {}

void SpringSimulation::loop() {
    m_spring.update(m_dt);
}