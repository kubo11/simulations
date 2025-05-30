#ifndef SIMULATIONS_BLACKHOLE_UI
#define SIMULATIONS_BLACKHOLE_UI

#include "pch.hh"

#include "framebuffer.hh"
#include "message_queue.hh"
#include "ui.hh"
#include "window.hh"

enum BlackHoleMessage { UpdateDistance, UpdateMass };

class BlackHoleUI : public UI {
 public:
  BlackHoleUI(Window& window, std::shared_ptr<MessageQueueWriter<BlackHoleMessage>> message_queue);

  float get_distance();
  float get_mass();

 protected:
  virtual void draw() override;

 private:
  std::mutex m_ui_mtx;
  Window& m_window;
  std::shared_ptr<MessageQueueWriter<BlackHoleMessage>> m_message_queue;

  float m_mass = 20.0f;
  float m_distance = 1e3f;

  void show_property_panel();
};

#endif  // SIMULATIONS_JELLY_UI