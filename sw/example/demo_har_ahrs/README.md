# Human Activity Recognition (HAR) & AHRS Demo

This project demonstrates a sensor fusion and machine learning application running on the NEORV32 processor. It combines the Mahony AHRS algorithm for orientation estimation with a Convolutional Neural Network (CNN) for Human Activity Recognition.

## Overview

The application processes pre-recorded IMU data (Accelerometer and Gyroscope) to:
1.  **Estimate Orientation**: Uses the Mahony filter to calculate Roll, Pitch, and Yaw.
2.  **Classify Activity**: Feeds sensor data into a 1D CNN to classify the user's activity (e.g., Walking, Standing, Laying).

## Credits & References

### Mahony AHRS
The AHRS implementation is based on the library by Paul Stoffregen:
- [MahonyAHRS GitHub Repository](https://github.com/PaulStoffregen/MahonyAHRS)

### Dataset
The test data used in this demo is derived from the "Human Activity Recognition Using Smartphones" dataset:
- [UCI Machine Learning Repository: HAR Dataset](https://archive.ics.uci.edu/dataset/240/human+activity+recognition+using+smartphones)

## Building and Running

1.  Navigate to the project directory.
2.  Run `make exe` to compile.
3.  Upload the executable to your NEORV32 setup.
4.  Connect a terminal to UART0 to view the output.
