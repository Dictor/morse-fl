#ifndef MORSE_FL_CONVOLUTION_LAYER
#define MORSE_FL_CONVOLUTION_LAYER

#include "arm_nnfunctions.h"

namespace kimdictor_morse_fl {
namespace convolution_layer {
void Init();
bool ConvolutionLayer1(q7_t *in, q7_t *out);
bool ConvolutionLayer2(q7_t *in, q7_t *out);
};  // namespace convolution_layer
};  // namespace kimdictor_morse_fl

#endif