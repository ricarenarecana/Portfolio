// Common ESP-NOW message definitions for auto_can_system
#pragma once
#include <stdint.h>

// Robot -> Station
struct RobotStatus {
  uint8_t state;          // state enum
  uint8_t can_index;      // 0-2 current target
  uint8_t collected_bits; // bitmask of collected bins
  uint16_t tof_mm;        // last TOF reading in mm
  bool obstacle;          // true if stopped by obstacle
};

// Station -> Robot
struct StationReport {
  bool limit[3];   // limit switch per bin
  bool reset_cmd;  // request reset run
  bool ack;        // station alive/ready
};

enum RobotState : uint8_t {
  STATE_INIT = 0,
  STATE_NAV_SEG1,
  STATE_NAV_Y,
  STATE_SEARCH_CAN,
  STATE_LOWER,
  STATE_GRIP,
  STATE_LIFT,
  STATE_CLEAR_TURN,
  STATE_WAIT_BIN_CONFIRM,
  STATE_RETURN,
  STATE_DONE,
  STATE_OBSTACLE_HALT
};
