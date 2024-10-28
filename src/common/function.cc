#include "function.hh"

Function::Function() : m_rand_label(rand()) {}

ConstFunction::ConstFunction(float A) : A(A) {}

float ConstFunction::operator()(float t) const { return A; }

std::string ConstFunction::to_string() const { return "A"; }

bool ConstFunction::show_ui() {
  ImGui::Text("A    ");
  ImGui::SameLine();
  return ImGui::DragFloat(get_unique_label("##A").c_str(), &this->A);
}

std::unique_ptr<Function> ConstFunction::copy() { return std::unique_ptr<Function>(new ConstFunction(A)); }

StepFunction::StepFunction(float A, float B, float c) : A(A), B(B), c(c) {}

float StepFunction::operator()(float t) const { return (t < c) ? A : B; }

std::string StepFunction::to_string() const { return "(t < c) ? A : B"; }

bool StepFunction::show_ui() {
  bool ret = false;
  ImGui::Text("A    ");
  ImGui::SameLine();
  if (ImGui::DragFloat(get_unique_label("##A").c_str(), &this->A)) ret = true;
  ImGui::Text("B    ");
  ImGui::SameLine();
  if (ImGui::DragFloat(get_unique_label("##B").c_str(), &this->B)) ret = true;
  ImGui::Text("c    ");
  ImGui::SameLine();
  if (ImGui::DragFloat(get_unique_label("##c").c_str(), &this->c)) ret = true;
  return ret;
}

std::unique_ptr<Function> StepFunction::copy() { return std::unique_ptr<Function>(new StepFunction(A, B, c)); }

SinStepFunction::SinStepFunction(float A, float omega, float fi) : A(A), omega(omega), fi(fi) {}

float SinStepFunction::operator()(float t) const { return glm::sign(A * glm::sin(omega * t + fi)); }

std::string SinStepFunction::to_string() const { return "sgn(A·sin(wt+o))"; }

bool SinStepFunction::show_ui() {
  bool ret = false;
  ImGui::Text("A    ");
  ImGui::SameLine();
  if (ImGui::DragFloat(get_unique_label("##A").c_str(), &this->A)) ret = true;
  ImGui::Text("w    ");
  ImGui::SameLine();
  if (ImGui::DragFloat(get_unique_label("##omega").c_str(), &this->omega)) ret = true;
  ImGui::Text("o    ");
  ImGui::SameLine();
  if (ImGui::DragFloat(get_unique_label("##fi").c_str(), &this->fi)) ret = true;
  return ret;
}

std::unique_ptr<Function> SinStepFunction::copy() {
  return std::unique_ptr<Function>(new SinStepFunction(A, omega, fi));
}

SinFunction::SinFunction(float A, float omega, float fi) : A(A), omega(omega), fi(fi) {}

float SinFunction::operator()(float t) const { return A * glm::sin(omega * t + fi); }

std::string SinFunction::to_string() const { return "A·sin(wt+o)"; }

bool SinFunction::show_ui() {
  bool ret = false;
  ImGui::Text("A    ");
  ImGui::SameLine();
  if (ImGui::DragFloat(get_unique_label("##A").c_str(), &this->A)) ret = true;
  ImGui::Text("w    ");
  ImGui::SameLine();
  if (ImGui::DragFloat(get_unique_label("##omega").c_str(), &this->omega)) ret = true;
  ImGui::Text("o    ");
  ImGui::SameLine();
  if (ImGui::DragFloat(get_unique_label("##fi").c_str(), &this->fi)) ret = true;
  return ret;
}

std::unique_ptr<Function> SinFunction::copy() { return std::unique_ptr<Function>(new SinFunction(A, omega, fi)); }