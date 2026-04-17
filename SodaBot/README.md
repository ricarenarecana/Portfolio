# SodaBot (Gantry Style Robot)

This folder contains the SodaBot project, an automated gantry-style robot system with ESP32-based firmware, web dashboard, and hardware tests.

## Contents
- firmware/
  - get_mac/
    - get_mac.ino
  - robot/
    - robot_main/
      - robot_main.ino
  - shared/
    - comm_defs.h
  - station/
    - station_main/
      - station_main.ino
- tests/
  - full/
    - full_system_dryrun.ino
  - pair/
    - drive_grip_stepper_test/
      - drive_grip_stepper_test.ino
    - gripper_stepper_test/
      - gripper_stepper_test.ino
    - limit_oled_station_test/
      - limit_oled_station_test.ino
    - stepper_servo_turns_test/
      - stepper_servo_turns_test.ino
    - stepper_test/
      - stepper_test.ino
    - espnow_pair_robot.ino
    - espnow_pair_station.ino
    - i2c_scan_station.ino
    - limit_switch_debug.ino
    - motor_encoder_test.ino
    - rgb_led_test.ino
    - servo_ir_test.ino
    - tof_test.ino
- web/
  - index.html
  - sample_status.json
- README.md — This file

## Features
- ESP32 dual-node autonomous can collector
- Modular firmware for robot and station
- Hardware block tests for all subsystems
- Web dashboard for status monitoring

## How to Use
1. Flash the appropriate firmware to robot and station ESP32 boards.
2. Use the tests in `tests/pair/` and `tests/full/` to verify hardware.
3. Serve the web dashboard locally or open `web/index.html` for status display.

---

See code and comments for wiring, configuration, and further details.
