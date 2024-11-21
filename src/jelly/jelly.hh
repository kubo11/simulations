#ifndef SIMULATIONS_JELLY
#define SIMULATIONS_JELLY

#include "pch.hh"

class Jelly {
 public:
  Jelly();

  void update(float dt);
  void reset();

  float get_t() const;

 private:
  float m_t;
};

#endif  // SIMULATIONS_JELLY