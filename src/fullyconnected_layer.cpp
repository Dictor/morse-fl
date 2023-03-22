#include "../inc/fullyconnected_layer.h"


using namespace kimdictor_morse_fl;
using namespace fullyconnected_layer;

genann* fullyconnected_layer::fc;
double fullyconnected_layer::fc_input[];

bool fullyconnected_layer::Init() {
  fc = genann_init(fc_input_dim, 0, fc_hidden_dim, fc_output_dim);
  memcpy(fc->weight, fc_weight, sizeof(fc_weight));
  return true;
}

void fullyconnected_layer::ApplyInput(q7_t* input, int count, double scale) {
  for (int i = 0; i < count; i++) {
    fc_input[i] = input[i] * scale;
  }
}

const double* fullyconnected_layer::Inference() {
  return genann_run(fc, fc_input);
}

std::pair<int, double> fullyconnected_layer::FindMax(const double* input) {
  double value = 0.f;
  int index = 0;
  for (int i = 0; i < fc_output_dim; i++) {
    if (input[i] >= value) {
      value = input[i];
      index = i;
    }
  }
  return std::make_pair(index, value);
}