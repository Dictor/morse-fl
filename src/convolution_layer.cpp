#include "../inc/convolution_layer.h"

#include <stdlib.h>
#include <string.h>
#include <zephyr/logging/log.h>

#include <memory>

#include "../inc/weight_bias.h"

LOG_MODULE_REGISTER(convolution_layer);

using namespace kimdictor_morse_fl;
using namespace convolution_layer;

bool convolution_layer::ConvolutionLayer1(q7_t *in, q7_t *out) {
  conv1_init();

  const int out_size =
      conv1_out_dims.n * conv1_out_dims.h * conv1_out_dims.w * conv1_out_dims.c;
  std::unique_ptr<q7_t[], decltype(free) *> out_buf(
      (q7_t *)malloc(out_size * sizeof(q7_t)), free);
  if (out_buf.get() == nullptr) {
    LOG_ERR("failed to allocate buffer, sz=%d", out_size);
    return false;
  }

  cmsis_nn_context ctx;
  ctx.size =
      arm_convolve_1_x_n_s8_get_buffer_size(&conv1_in_dims, &conv1_filter_dims);
  std::unique_ptr<uint8_t[], decltype(free) *> ctx_buf(
      (uint8_t *)malloc(ctx.size), free);
  ctx.buf = ctx_buf.get();
  if (ctx.buf == nullptr) {
    LOG_ERR("failed to allocate convolution buffer, sz=%d", ctx.size);
    return false;
  }

  arm_status ret;
  ret = arm_convolve_1_x_n_s8(
      &ctx, &conv1_params, &conv1_quant, &conv1_in_dims, in, &conv1_filter_dims,
      conv1_weight, &conv1_bias_dims, conv1_bias, &conv1_out_dims, out_buf.get());
  if (ret != ARM_MATH_SUCCESS) {
    LOG_ERR("failed to calculate convolution layer1 (%d)", ret);
    return false;
  }

  ret = arm_max_pool_s8(&ctx, &max1_params, &max1_in_dims, out_buf.get(),
                        &max1_filter_dims, &max1_out_dims, out);
  if (ret != ARM_MATH_SUCCESS) {
    LOG_ERR("failed to calculate maxpool layer1 (%d)", ret);
    return false;
  }

  arm_relu_q7(out, max1_out_dims.c * max1_out_dims.h * max1_out_dims.w);
  return true;
}

bool convolution_layer::ConvolutionLayer2(q7_t *in, q7_t *out) {
  conv2_init();

  const int out_size =
      conv2_out_dims.n * conv2_out_dims.h * conv2_out_dims.w * conv2_out_dims.c;
  std::unique_ptr<q7_t[], decltype(free) *> out_buf(
      (q7_t *)malloc(out_size * sizeof(q7_t)), free);
  if (out_buf.get() == nullptr) {
    LOG_ERR("failed to allocate buffer, sz=%d", out_size);
    return false;
  }

  cmsis_nn_context ctx;
  ctx.size =
      arm_convolve_1_x_n_s8_get_buffer_size(&conv2_in_dims, &conv2_filter_dims);
  std::unique_ptr<uint8_t[], decltype(free) *> ctx_buf(
      (uint8_t *)malloc(ctx.size), free);
  ctx.buf = ctx_buf.get();
  if (ctx.buf == nullptr) {
    LOG_ERR("failed to allocate convolution buffer, sz=%d", ctx.size);
    return false;
  }

  arm_status ret;
  ret = arm_convolve_1_x_n_s8(
      &ctx, &conv2_params, &conv2_quant, &conv2_in_dims, in, &conv2_filter_dims,
      conv2_weight, &conv2_bias_dims, conv2_bias, &conv2_out_dims, out_buf.get());
  if (ret != ARM_MATH_SUCCESS) {
    LOG_ERR("failed to calculate convolution layer2 (%d)", ret);
    return false;
  }

  ret = arm_max_pool_s8(&ctx, &max2_params, &max2_in_dims, out_buf.get(),
                        &max2_filter_dims, &max2_out_dims, out);
  if (ret != ARM_MATH_SUCCESS) {
    LOG_ERR("failed to calculate maxpool layer2 (%d)", ret);
    return false;
  }
  
  arm_relu_q7(out, max2_out_dims.c * max2_out_dims.h * max2_out_dims.w);
  return true;
}