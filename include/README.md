# Game Playing Robot - Valorant Controller Firmware

## Overview

ESP32-based firmware for controlling 5 servo motors to play Valorant via a gaming controller. The system uses serial communication from a PC-based game logic program to receive commands and control the servos with smooth interpolation.

## Architecture

### System Components

```
PC Game Logic + GUI
    ↓ (Serial/USB)
ESP32 Firmware
    ├─ Serial Input Handler (parse MODE and SERVO commands)
    ├─ Mode Manager (state machine)
    ├─ Servo Controller (PWM + smooth movement)
    ├─ Mode Handler (mode-specific logic)
    └─ Response Builder (feedback to PC)
    ↓ (PWM signals)
5 Servo Motors
```

### State Machine

```
IDLE
  ↓
MODE_SET (game mode selected by GUI)
  ↓
RUNNING (mode active, receiving servo commands)
  ↓
STOPPED (game stopped, servos at neutral)
  ↓
IDLE
```

## Communication Protocol

### Command Types

#### 1. Mode Commands (PC → ESP32)

Control game mode from GUI:

```
MODE,SET,VALORANT\n     # Set game to VALORANT mode
MODE,START\n            # Start the game (transition to RUNNING)
MODE,STOP\n             # Stop the game (return to neutral)
MODE,STATUS\n           # Request current status
```

#### 2. Servo Commands (PC → ESP32)

Send servo positions during gameplay (only in RUNNING state):

```
SERVO,LJX,LJY,RJX,RJY,TRIGGER[,DURATION]\n

Parameters:
  LJX:       Left Joystick X angle (0-180°)
  LJY:       Left Joystick Y angle (0-180°)
  RJX:       Right Joystick X angle (0-180°)
  RJY:       Right Joystick Y angle (0-180°)
  TRIGGER:   Trigger button angle (0=unpressed, 180=pressed)
  DURATION:  Movement time in ms (optional, default 200ms)

Example:
  SERVO,90,90,90,90,0,200\n     # Center all, trigger unpressed, move over 200ms
```

### Response Messages (ESP32 → PC)

```
OK\n                              # Command executed successfully
ERR,<CODE>,<MESSAGE>\n            # Error with code and description
STATUS,<MODE>,<STATE>\n           # Status response
DEBUG,<MESSAGE>\n                 # Debug information (optional)
```

**Error Codes:**

- `100`: Invalid mode name
- `101`: Cannot change mode while running
- `102`: Invalid state transition
- `200`: Servo position out of range (0-180)
- `201`: SERVO command sent but mode not RUNNING
- `202`: Incomplete servo command
- `203`: Invalid duration parameter
- `300`: Malformed command
- `301`: Unknown command type
- `302`: Serial buffer overflow

### Example Communication Sequence

```
[Startup]
→ MODE,SET,VALORANT\n
← STATUS,VALORANT,MODE_SET\n

[Start Game]
→ MODE,START\n
← STATUS,VALORANT,RUNNING\n

[During Gameplay - repeated every 50ms]
→ SERVO,90,95,85,100,50,100\n
← OK\n

[Stop Game]
→ MODE,STOP\n
← STATUS,VALORANT,STOPPED\n
```

## Servo Configuration

### Servo Mapping

| Servo ID | GPIO Pin | Purpose          | Neutral |
| -------- | -------- | ---------------- | ------- |
| 0        | GPIO 25  | Left Joystick X  | 90°     |
| 1        | GPIO 26  | Left Joystick Y  | 90°     |
| 2        | GPIO 27  | Right Joystick X | 90°     |
| 3        | GPIO 32  | Right Joystick Y | 90°     |
| 4        | GPIO 33  | Trigger Button   | 0°      |

### PWM Settings

- **Frequency**: 50 Hz (standard servo frequency)
- **Resolution**: 16-bit
- **Pulse Range**: 1000-2000 µs (0-180°)
- **Baud Rate**: 115200

## File Structure

```
src/
├── main.cpp                    # Main firmware loop & command routing
├── response_builder.cpp        # Serial response messages
├── serial_handler.cpp          # Command parsing
├── mode_manager.cpp            # State machine logic
├── mode_handler.cpp            # Mode-specific callbacks
├── servo_controller.cpp        # PWM & servo control
└── servo_config.cpp            # Servo calibration data

include/
├── protocol.h                  # Protocol definitions & enums
├── response_builder.h          # Response message interface
├── serial_handler.h            # Command parsing interface
├── mode_manager.h              # State machine interface
├── mode_handler.h              # Mode handler interface
├── servo_controller.h          # Servo control interface
├── servo_config.h              # Servo configuration
├── mode_config.h               # Mode configurations
├── racing_stepper.h            # [STUB] Racing mode stepper
└── racing_paddle.h             # [STUB] Racing mode paddles
```

## Key Features

✅ **Modular Design**: Clean separation of concerns  
✅ **Smooth Interpolation**: Configurable movement duration  
✅ **State Protection**: SERVO commands only in RUNNING state  
✅ **Timeout Detection**: Automatic neutral on 5s inactivity  
✅ **Error Feedback**: Detailed error codes to PC  
✅ **Future-Ready**: Placeholder stubs for racing mode  
✅ **Non-blocking**: Uses timer-based interpolation

## Safety Features

1. **Failsafe**: On error, servos hold last valid position
2. **Timeout**: If no command for 5 seconds while RUNNING, move to neutral
3. **Range Validation**: All servo positions validated (0-180°)
4. **State Gating**: SERVO commands rejected unless mode RUNNING
5. **Neutral Fallback**: MODE,STOP moves all servos to neutral

## Testing Checklist

- [ ] Serial communication at 115200 baud
- [ ] MODE,SET,VALORANT → returns correct STATUS
- [ ] MODE,START → transitions to RUNNING
- [ ] SERVO commands → servos move smoothly
- [ ] Duration parameter → servos move over specified time
- [ ] MODE,STOP → servos return to neutral
- [ ] SERVO command in non-RUNNING state → ERR,201
- [ ] Invalid angle (>180) → ERR,200
- [ ] Timeout after 5s inactivity → auto-neutral

## Future Extensions

### Racing Mode (Ready to implement)

- [ ] Stepper motor control for steering wheel
- [ ] 2 paddle servo motors for gas/brake
- [ ] MODE,SET,RACING support
- [ ] Merge with VALORANT mode in mode manager

### Additional Features

- [ ] EEPROM calibration storage
- [ ] Per-servo speed limiting
- [ ] Acceleration profiles
- [ ] LED status indicators
- [ ] WiFi support (as alternative to Serial)

## Calibration Notes

If servos need tuning, adjust `servo_config.cpp`:

```cpp
// Example: Adjust neutral position for left joystick X
ServoCalibration servoConfigs[NUM_SERVOS] = {
    {SERVO_LJ_X, SERVO_LJ_X_PIN, 0, 0, 180, 85, SERVO_PWM_MIN_US, SERVO_PWM_MAX_US},  // Changed 90 → 85
    // ...
};
```

## Dependencies

- Arduino Framework (via PlatformIO)
- ESP32 SDK (espressif32 platform)
- None (standard libraries only)

## Build & Upload

```bash
# Build
pio run

# Upload to ESP32
pio run -t upload

# Monitor serial output
pio device monitor --baud 115200
```

## Troubleshooting

**Q: Servos not moving?**

- Check GPIO pin connections
- Verify PWM frequency (should be 50 Hz)
- Test with `MODE,SET,VALORANT` → `MODE,START` → `SERVO,90,90,90,90,0,200`

**Q: Servo movements jerky?**

- Increase duration parameter in SERVO command
- Check servo power supply (may be insufficient)

**Q: Timeout moving servos to neutral unexpectedly?**

- Ensure PC sends commands within 5 seconds in RUNNING state
- Adjust COMMAND_TIMEOUT_MS in protocol.h if needed

**Q: Serial communication errors?**

- Verify baud rate is 115200
- Check USB cable connection
- Reset ESP32 if needed
