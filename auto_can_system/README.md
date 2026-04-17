# auto_can_system

ESP32 dual-node autonomous can collector.

## Layout
- `firmware/robot/robot_main.ino` – robot ESP32 (motors, encoders on front wheels only, IR, TOF, servo, stepper, RGB, ESP-NOW).
- `firmware/station/station_main.ino` – station ESP32 (3 limit switches, single OLED summary, status LED, ESP-NOW).
- `firmware/shared/comm_defs.h` – shared message structs/state enums.
- `tests/pair/*.ino` – hardware block tests (motors+encoders, servo+IR, RGB, TOF, stepper, limit+OLEDs, ESP-NOW pair).
- `tests/full/full_system_dryrun.ino` – state-machine dry run without motors.
- `web/index.html` + `web/sample_status.json` – static status dashboard.

## Flash order
1) Put the real peer MACs into robot/station and ESP-NOW tests (`stationMac` / `robotMac`).
2) Flash `station_main.ino` (DevKit); OLEDs should show Empty and link once Robot is on.
3) Flash `robot_main.ino`; tune `COUNTS_PER_CM`, `COUNTS_PER_TURN`, distances `D1_CM/D2_CM/RETURN_CM`, and lift steps.

## Beginner tuning knobs (robot_main.ino)
- Speeds: `PWM_CRUISE`, `PWM_TURN`, `PWM_MIN`.
- Distances: `D1_CM`, `D2_CM`, `RETURN_CM`.
- Encoders (front wheels only): `COUNTS_PER_CM`, `COUNTS_PER_TURN` (90° right).
- Safety: `TOF_STOP_MM`, `TOF_RESUME_MM`.
- Servo: `SERVO_OPEN_US`, `SERVO_CLOSE_US`, hold `GRIP_MS`.
- Lift: `STEP_DELAY_US` (speed), `STEP_COUNT_UP/DOWN` (travel).

## Pinouts (robot)
- L298N drive: IN1=25, IN2=26, ENA=27 (left); IN3=32, IN4=33, ENB=14 (right).
- Encoders (front): L_A=34, L_B=35; R_A=36, R_B=39.
- IR: 4 (L), 5 (R), active LOW.
- RGB LED: R=2, G=12, B=0.
- Servo: 13.
- Stepper: DIR=16, STEP=17, EN=18.
- I2C (TOF): SDA=21, SCL=22.

## Pinouts (station)
- Limit switches: 19, 23, 15 (active LOW).
- OLED: I2C 21/22, addr 0x3C (or 0x3D if you move the jumper).
- Status LED: 2/12/0.

## Stepper wiring (your colors)
- Coil A: Red ↔ Blue
- Coil B: Green ↔ Black
- Step/dir driver: A+/A− → Red/Blue, B+/B− → Green/Black.
- L298N as bipolar stepper bridge: OUT1/OUT2 = Red/Blue, OUT3/OUT4 = Green/Black. Swap a coil if direction is reversed.

## Web dashboard access (no robot control)
- Easiest: serve locally. From `auto_can_system`, run `npx serve web` (or install once: `npm i -g serve`; then `serve web`). It will print `http://<your_pc_ip>:3000`. Open that from phone/PC on the same Wi‑Fi.
- Static file: open `web/index.html` directly in a browser; update `statusUrl` inside the file to wherever you expose JSON (e.g., a small Python/Node bridge that reads serial and serves JSON).
- Current firmware does not host Wi‑Fi or HTTP. If you want the ESP32 to join Wi‑Fi with SSID/password and serve JSON, we can add a small Wi‑Fi + HTTP handler later.

## Tests
- Motors/tachs: `tests/pair/motor_encoder_test.ino`
- Servo+IR: `tests/pair/servo_ir_test.ino`
- RGB LED: `tests/pair/rgb_led_test.ino`
- TOF: `tests/pair/tof_test.ino` (Adafruit_VL53L0X)
- Stepper: `tests/pair/stepper_test.ino`
- Limit+OLEDs: `tests/pair/limit_oled_station_test.ino`
- ESP-NOW link: `tests/pair/espnow_pair_robot.ino`, `espnow_pair_station.ino`
- Dry-run: `tests/full/full_system_dryrun.ino`
