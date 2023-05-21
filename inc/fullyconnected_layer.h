#ifndef MORSE_FL_FULLYCONNECTED_LAYER
#define MORSE_FL_FULLYCONNECTED_LAYER

#include <memory>
#include <utility>

#include "./fullyconnected_layer.h"
#include "./genann.h"
#include "arm_nnfunctions.h"
#include "../inc/weight_bias.h"

namespace kimdictor_morse_fl {
namespace fullyconnected_layer {

extern genann* fc;
extern double fc_input[fc_input_dim];
typedef std::unique_ptr<double[], decltype(free)> output_array_t;

bool Init();
void ApplyInput(q7_t* input, int count, double scale, int8_t zeropoint);
const double* Inference();
std::pair<int, double> FindMax(const double* input);
};  // namespace fullyconnected_layer
};  // namespace kimdictor_morse_fl

#endif