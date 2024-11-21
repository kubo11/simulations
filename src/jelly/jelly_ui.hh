#ifndef SIMULATIONS_JELLY_UI
#define SIMULATIONS_JELLY_UI

#include "pch.hh"

#include "framebuffer.hh"
#include "message_queue.hh"
#include "ui.hh"
#include "jelly.hh"
#include "window.hh"

enum JellyMessage { Start, Stop, Restart, Apply, Skip };

class JellyUI : public UI {
 public:
  JellyUI(Window& window, std::shared_ptr<MessageQueueWriter<JellyMessage>> message_queue);

  void update_jelly_data(const Jelly& jelly);
  void update_jelly_parameters(Jelly& jelly);
  void reset_jelly(Jelly& jelly);

  float get_dt();
  unsigned int get_skip_frames_count();

 protected:
  virtual void draw() override;

 private:
  float m_dt = 0.001f;
  int m_skip_frames = 100;

  std::mutex m_ui_mtx;

  Window& m_window;

  bool m_start_button_enabled = false;
  bool m_stop_button_enabled = false;
  bool m_apply_button_enabled = false;
  bool m_skip_button_enabled = false;

  std::shared_ptr<MessageQueueWriter<JellyMessage>> m_message_queue;

  void show_property_panel();
};

#endif  // SIMULATIONS_JELLY_UI