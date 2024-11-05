#include "whirligig_simulation.hh"

WhirligigSimulation::WhirligigSimulation(float dt, std::function<void(void)> callback, Whirligig& whirligig)
    : Simulation(dt, callback), m_whirligig(whirligig) {}

void WhirligigSimulation::loop() { m_whirligig.update(m_dt); }