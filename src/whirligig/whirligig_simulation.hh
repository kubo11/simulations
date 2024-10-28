#ifndef SIMULATIONS_WHIRLIGIG_SIMULATION
#define SIMULATIONS_WHIRLIGIG_SIMULATION

#include "pch.hh"

#include "simulation.hh"
#include "whirligig.hh"

class WhirligigSimulation : public Simulation {
public:
  WhirligigSimulation(float dt, std::function<void(void)> callback, Whirligig& whirligig);
  virtual ~WhirligigSimulation() override = default;

  virtual void loop() override;
private:
  Whirligig& m_whirligig;
};

#endif // SIMULATIONS_WHIRLIGIG_SIMULATION