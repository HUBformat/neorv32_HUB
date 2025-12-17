#ifndef HAR_H
#define HAR_H

#ifdef __cplusplus
extern "C" {
#endif

// Activity Classes
#define ACTIVITY_STATIC  0
#define ACTIVITY_WALKING 1
#define ACTIVITY_RUNNING 2

// Input Dimensions
#define HAR_WINDOW_SIZE 64
#define HAR_CHANNELS    6

/**
 * @brief Adds a new 6-axis IMU sample to the internal circular buffer.
 * 
 * @param ax Accelerometer X
 * @param ay Accelerometer Y
 * @param az Accelerometer Z
 * @param gx Gyroscope X
 * @param gy Gyroscope Y
 * @param gz Gyroscope Z
 */
void add_imu_sample_to_buffer(float ax, float ay, float az, float gx, float gy, float gz);

/**
 * @brief Runs the HAR inference pipeline on the current buffer.
 * 
 * @return int The predicted activity class ID (0=STATIC, 1=WALKING, 2=RUNNING)
 */
int run_har_inference(void);

#ifdef __cplusplus
}
#endif

#endif // HAR_H
