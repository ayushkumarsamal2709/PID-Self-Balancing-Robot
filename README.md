# PID Self-Balancing Robot

## Overview
This project presents the design and implementation of a two-wheel self-balancing robot using Arduino Uno and MPU6050. The system employs a PID (Proportional–Integral–Derivative) control algorithm to maintain upright stability by continuously correcting tilt deviations.

## Working Principle
The MPU6050 sensor measures the tilt angle (θ) of the robot.  
The tilt error is defined as:

e(t) = θ_set − θ(t)

where θ_set is the desired vertical angle (0°).  

The PID controller generates motor control output:

u(t) = Kp e(t) + Ki ∫e(t)dt + Kd de(t)/dt

The motor driver applies corrective motion to restore balance.

## Hardware Components
- Arduino Uno
- MPU6050 (GY-521)
- Motor Driver Shield
- DC Gear Motors
- 2 × Li-ion Batteries (4V each)
- Robot Chassis

## Software Implementation
- Real-time tilt estimation
- PID control loop
- Motor PWM control
- Serial monitoring for debugging

## Experimental Results
The robot responds correctly to tilt disturbances:
- Forward tilt → Forward motor motion
- Backward tilt → Reverse motor motion

Full autonomous balancing was limited due to tuning and mechanical constraints.

## Applications
- Control systems education
- Robotics research
- Embedded systems learning
- Dynamic stabilization studies

## Future Improvements
- Advanced PID tuning
- State-space control implementation
- Addition of wheel encoders
- Improved mechanical alignment

## Author
Ayush Kumar Samal
