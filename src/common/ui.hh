#ifndef SIMULATIONS_COMMON_UI
#define SIMULATIONS_COMMON_UI

#include "pch.hh"

#include "window.hh"

class UI {
 public:
  UI(Window& window);
  virtual ~UI();

  void update();

 protected:
  virtual void draw() = 0;

 private:
  void start_frame();
  void end_frame();
};

#endif  // SIMULATIONS_COMMON_UI