#include "har.h"
#include "har_weights.h"
#include <float.h>
#include <stdint.h>

// =========================================================================================
// Model Architecture Constants
// =========================================================================================
// Input: 64 steps, 6 channels
// Conv1D: 8 filters, kernel 4, stride 1
// Output Conv1D: (64 - 4) + 1 = 61 steps, 8 channels
// MaxPool1D: pool 2, stride 2
// Output MaxPool: 61 / 2 = 30 steps (integer math), 8 channels
// Flatten: 30 * 8 = 240 elements
// Dense: 240 inputs, 3 outputs

#define CONV_FILTERS    8
#define CONV_KERNEL     4
#define CONV_STRIDE     1
#define CONV_OUTPUT_LEN 61 // (64 - 4)/1 + 1

#define POOL_SIZE       2
#define POOL_STRIDE     2
#define POOL_OUTPUT_LEN 30 // 61 / 2 (integer truncation)

#define DENSE_INPUTS    240 // 30 * 8
#define DENSE_OUTPUTS   3

// =========================================================================================
// Buffers
// =========================================================================================

// Circular buffer for input data
// Size must be power of 2 for bitwise wrapping if we wanted to be fancy, 
// but here we use explicit masking for 64 (which is power of 2).
static float input_buffer[HAR_WINDOW_SIZE][HAR_CHANNELS];
static int buffer_head = 0;

// Intermediate buffers for layer outputs
// We need to linearize the input buffer first
static float linear_input[HAR_WINDOW_SIZE * HAR_CHANNELS];
static float conv_output[CONV_OUTPUT_LEN * CONV_FILTERS];
static float pool_output[POOL_OUTPUT_LEN * CONV_FILTERS];
static float dense_output[DENSE_OUTPUTS];

// =========================================================================================
// Helper Functions
// =========================================================================================

void add_imu_sample_to_buffer(float ax, float ay, float az, float gx, float gy, float gz) {
    input_buffer[buffer_head][0] = ax;
    input_buffer[buffer_head][1] = ay;
    input_buffer[buffer_head][2] = az;
    input_buffer[buffer_head][3] = gx;
    input_buffer[buffer_head][4] = gy;
    input_buffer[buffer_head][5] = gz;

    // Increment head and wrap around using bitwise AND (64 - 1 = 63 = 0x3F)
    // This avoids division/modulo operator.
    buffer_head = (buffer_head + 1) & 0x3F;
}

static void linearize_buffer(float* dest) {
    // The buffer_head points to the *oldest* data point to be overwritten next?
    // Actually, in a standard circular buffer, head is where we write.
    // So the oldest sample is at buffer_head (the one we are about to overwrite next time, 
    // but currently it holds the oldest valid sample if the buffer is full).
    // Let's assume the buffer is full for simplicity.
    
    int idx = buffer_head;
    for (int i = 0; i < HAR_WINDOW_SIZE; i++) {
        for (int c = 0; c < HAR_CHANNELS; c++) {
            dest[i * HAR_CHANNELS + c] = input_buffer[idx][c];
        }
        idx = (idx + 1) & 0x3F;
    }
}

// =========================================================================================
// Layer Implementations
// =========================================================================================

// Conv1D Layer
// Input: [steps_in * channels_in]
// Output: [steps_out * filters]
// Weights: [kernel * channels_in * filters]
void layer_conv1d(const float* input, float* output, const float* weights, const float* bias,
                  int steps_in, int channels_in, int filters, int kernel_size, int stride) {
    
    // Loop until we hit the boundary.
    
    int out_step = 0;
    for (int i = 0; i <= steps_in - kernel_size; i += stride) {
        for (int f = 0; f < filters; f++) {
            float sum = bias[f];
            
            // Convolution operation
            for (int k = 0; k < kernel_size; k++) {
                for (int c = 0; c < channels_in; c++) {
                    // Input index: (current_step + kernel_offset) * channels + channel
                    int input_idx = (i + k) * channels_in + c;
                    
                    // Weight index: kernel_offset * (channels * filters) + channel * filters + filter
                    // Assuming weights are stored as [Kernel][Channel][Filter]
                    int weight_idx = k * (channels_in * filters) + c * filters + f;
                    
                    sum += input[input_idx] * weights[weight_idx];
                }
            }
            output[out_step * filters + f] = sum;
        }
        out_step++;
    }
}

// ReLU Activation
void layer_relu(float* data, int size) {
    for (int i = 0; i < size; i++) {
        if (data[i] < 0.0f) {
            data[i] = 0.0f;
        }
    }
}

// MaxPool1D Layer
// Input: [steps_in * channels]
// Output: [steps_out * channels]
void layer_maxpool1d(const float* input, float* output, int steps_in, int channels, int pool_size, int stride) {
    int out_step = 0;
    // Loop through input steps with stride
    // Condition: i + pool_size <= steps_in ensures we have a full pool window
    // If we want "valid" padding behavior.
    for (int i = 0; i + pool_size <= steps_in; i += stride) {
        for (int c = 0; c < channels; c++) {
            float max_val = -FLT_MAX;
            for (int p = 0; p < pool_size; p++) {
                int input_idx = (i + p) * channels + c;
                float val = input[input_idx];
                if (val > max_val) {
                    max_val = val;
                }
            }
            output[out_step * channels + c] = max_val;
        }
        out_step++;
    }
}

// Dense Layer
// Input: [inputs]
// Output: [outputs]
// Weights: [inputs * outputs] (Flattened [Input][Output])
void layer_dense(const float* input, float* output, const float* weights, const float* bias, int inputs, int outputs) {
    for (int o = 0; o < outputs; o++) {
        float sum = bias[o];
        for (int i = 0; i < inputs; i++) {
            // Weight index: input_idx * outputs + output_idx
            int weight_idx = i * outputs + o;
            sum += input[i] * weights[weight_idx];
        }
        output[o] = sum;
    }
}

// =========================================================================================
// Inference Pipeline
// =========================================================================================

int run_har_inference(void) {
    // 1. Linearize Input
    linearize_buffer(linear_input);

    // 2. Conv1D
    // Input: 64 steps, 6 channels
    // Output: 61 steps, 8 filters
    layer_conv1d(linear_input, conv_output, conv_weights, conv_biases, 
                 HAR_WINDOW_SIZE, HAR_CHANNELS, CONV_FILTERS, CONV_KERNEL, CONV_STRIDE);

    // 3. ReLU
    layer_relu(conv_output, CONV_OUTPUT_LEN * CONV_FILTERS);

    // 4. MaxPool1D
    // Input: 61 steps, 8 channels (filters from conv become channels here)
    // Output: 30 steps, 8 channels
    layer_maxpool1d(conv_output, pool_output, CONV_OUTPUT_LEN, CONV_FILTERS, POOL_SIZE, POOL_STRIDE);

    // 5. Flatten
    // Implicitly done because pool_output is already a flat array of floats in memory.
    // Size: 30 * 8 = 240.

    // 6. Dense
    // Input: 240
    // Output: 3
    layer_dense(pool_output, dense_output, dense_weights, dense_biases, DENSE_INPUTS, DENSE_OUTPUTS);

    // 7. ArgMax
    int predicted_class = 0;
    float max_score = dense_output[0];
    
    for (int i = 1; i < DENSE_OUTPUTS; i++) {
        if (dense_output[i] > max_score) {
            max_score = dense_output[i];
            predicted_class = i;
        }
    }

    return predicted_class;
}
