#include <neorv32.h>
#include "MahonyAHRS.h"
#include "har.h"
#include "test_data.h"
#include <stdio.h>

// Define a sample frequency (UCI HAR is 50Hz)
#define SAMPLE_FREQ 50.0f

// Instantiate the Mahony filter
Mahony filter;

extern "C" {
    // Main entry point
    int main(void);
}

int main(void) {
    // Initialize the NEORV32 Runtime Environment
    neorv32_rte_setup();

    neorv32_uart0_printf("Mahony AHRS + HAR Demo on NEORV32 (UCI HAR Data)\n");

    // Initialize the filter
    filter.begin(SAMPLE_FREQ);

    neorv32_uart0_printf("Starting loop...\n");

    int sample_count = 0;
    int data_idx = 0;

    while(1) {
        // Get data from the test dataset
        float ax = test_data[data_idx].ax;
        float ay = test_data[data_idx].ay;
        float az = test_data[data_idx].az;
        float gx = test_data[data_idx].gx;
        float gy = test_data[data_idx].gy;
        float gz = test_data[data_idx].gz;

        // Update the filter
        // Note: Mahony expects Gyro in degrees/s? 
        // The library code says: "Convert gyroscope degrees/sec to radians/sec" inside update().
        // UCI HAR data is already in radians/sec.
        // So we should convert rad/s back to deg/s for the library, OR modify the library.
        // Let's convert rad/s to deg/s here to match the library expectation.
        // 1 rad = 57.29578 deg
        float gx_deg = gx * 57.29578f;
        float gy_deg = gy * 57.29578f;
        float gz_deg = gz * 57.29578f;

        filter.update(gx_deg, gy_deg, gz_deg, ax, ay, az, 0, 0, 0); // No magnetometer in this dataset

        // Add sample to HAR buffer
        // HAR model likely expects the raw units (g and rad/s) as trained.
        add_imu_sample_to_buffer(ax, ay, az, gx, gy, gz);

        // Get orientation
        float roll = filter.getRoll();
        float pitch = filter.getPitch();
        float yaw = filter.getYaw();

        // Run HAR inference every 32 samples
        sample_count++;
        int activity = -1;
        if (sample_count >= 32) {
            activity = run_har_inference();
            sample_count = 0;
        }

        // Print results
        if (activity != -1) {
            neorv32_uart0_printf("Idx: %d | R: %d, P: %d, Y: %d | Activity: %d\n", data_idx, (int)roll, (int)pitch, (int)yaw, activity);
        }

        // Advance index and wrap around
        data_idx++;
        if (data_idx >= TEST_DATA_LEN) {
            data_idx = 0;
            neorv32_uart0_printf("--- Replaying Data ---\n");
        }

        // Delay to simulate sample rate
        neorv32_aux_delay_ms(neorv32_sysinfo_get_clk(), 1000 / (int)SAMPLE_FREQ); 
    }

    return 0;
}
