#ifndef SIMULATIONS_COMMON_FUNCTION
#define SIMULATIONS_COMMON_FUNCTION

#include "pch.hh"

class Function {
 public:
  Function();

  virtual float operator()(float t) const = 0;
  virtual std::string to_string() const = 0;
  virtual bool show_ui() = 0;
  virtual std::unique_ptr<Function> copy() = 0;

 protected:
  int m_rand_label;

  std::string get_unique_label(const std::string& label) { return label + std::to_string(m_rand_label); }
};

class ConstFunction : public Function {
 public:
  float A;

  ConstFunction(float A);

  virtual float operator()(float t) const override;
  virtual std::string to_string() const override;
  virtual bool show_ui() override;
  virtual std::unique_ptr<Function> copy() override;
};

class StepFunction : public Function {
 public:
  float A;
  float B;
  float c;

  StepFunction(float A, float B, float step);

  virtual float operator()(float t) const override;
  virtual std::string to_string() const override;
  virtual bool show_ui() override;
  virtual std::unique_ptr<Function> copy() override;
};

class SinStepFunction : public Function {
 public:
  float A;
  float omega;
  float fi;

  SinStepFunction(float A, float omega, float fi);

  virtual float operator()(float t) const override;
  virtual std::string to_string() const override;
  virtual bool show_ui() override;
  virtual std::unique_ptr<Function> copy() override;
};

class SinFunction : public Function {
 public:
  float A;
  float omega;
  float fi;

  SinFunction(float A, float omega, float fi);

  virtual float operator()(float t) const override;
  virtual std::string to_string() const override;
  virtual bool show_ui() override;
  virtual std::unique_ptr<Function> copy() override;
};

#endif  // SIMULATIONS_COMMON_FUNCTION