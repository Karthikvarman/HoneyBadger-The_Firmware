# HoneyBadger — Automotive Bare-Metal ECU Firmware & 3D Digital Twin

**HoneyBadger** is an automotive-grade bare-metal Electronic Control Unit (ECU) firmware implementation designed for ARM Cortex-M (72 MHz) microcontrollers, paired with a real-time interactive 3D WebGL Digital Twin and telemetry diagnostic station.

---

## Architecture Overview

HoneyBadger implements a **Time-Triggered Co-operative (TTC)** deterministic scheduler that coordinates multi-sensor acquisition, hardware bus arbitration, closed-loop DC motor control, and continuous DMA telemetry streaming without an RTOS overhead.

```
                  +-----------------------------------+
                  |      HoneyBadger TTC Scheduler    |
                  |     (10ms Base Tick / SysTick)    |
                  +-----------------+-----------------+
                                    |
     +------------------------------+------------------------------+
     |                              |                              |
[Sensor Acquisition]       [Closed-Loop PID Control]     [Diagnostic Telemetry]
- S1..S10 Multi-Bus        - 10ms Discrete PID Loop      - Ping-Pong Double Buffer
- Hardware Bus Arbiter     - Dual-Channel Optical Enc    - CRC-16 Telemetry Frame
- SPI / I2C / UART         - Anti-Windup & Clamping      - UART3 DMA TX Stream
```

### Key Subsystems

1. **Deterministic TTC Scheduler (`scheduler.c`)**:
   - Fixed 10ms base tick with static priority dispatching (P0 highest to P5 lowest).
   - Dynamic CPU load profiling, execution timing, and hardware watchdog servicing.

2. **Multi-Peripheral Hardware Bus Arbiter (`bus_arbiter.c`)**:
   - Mutex-guaranteed sharing of buses (SPI1, SPI2, I2C1) across asynchronous sensor tasks with non-blocking re-queueing and starvation prevention.

3. **Sensor Acquisition Subsystem (`sensor_tasks.c`)**:
   - **UART1**: S1 Wheel Speed (500ms, P1)
   - **UART2**: S2 Transmission Temp (250ms, P2)
   - **SPI1**: S3 IMU Accelerometer (10ms, P0), S4 IMU Gyroscope (1000ms, P5)
   - **SPI2**: S5 Battery Voltage (800ms, P3), S6 Battery Current (1500ms, P4)
   - **I2C1**: S7 Throttle Position, S8 Brake Pressure, S9 Steering Angle, S10 Ambient Temp (450ms, P3)

4. **Closed-Loop DC Motor & Optical Encoder (`motor_control.c`, `pid_controller.c`)**:
   - 10ms discrete PID position control with configurable $K_p$, $K_i$, $K_d$.
   - Anti-windup integrator clamping and stall detection.
   - Dual-channel quadrature optical encoder simulation.

5. **Double-Buffered Telemetry Store (`data_store.c`, `telemetry.c`)**:
   - Lock-free ping-pong buffer allowing ISR-safe writes while DMA streams frozen snapshots.
   - CRC-16 CCITT (0x1021) frame verification and CRC-8 (0x07) command verification.

---

## 3D Digital Twin & Web Diagnostics

Located in `web/`, the digital twin provides a browser-based engineering dashboard:
- **Three.js 3D CAD Board**: High-detail PCB rendering with solder mask, ENIG gold traces, MCU QFP-64 package, IC chips, spinning DC motor, and slotted optical disk.
- **Real-Time Digital Twin**: Mirrors the TTC scheduler, PID loop dynamics, quadrature pulses, and telemetry packet generation directly in-browser.
- **Diagnostic Station**: Live setpoint step-response graphing, sensor telemetry tables, scheduler task monitor, and streaming UART3 hex console.

### Launching the Digital Twin
```bash
cd web
python -m http.server 8080
```
Open `http://localhost:8080` in any modern web browser.

---

## Project Structure

```
HoneyBadger/
├── Application/         # High-level tasks (motor control, sensors, telemetry)
├── Config/              # Hardware pins, sensor intervals, PID tuning parameters
├── Core/                # main.c system entry point and core loop
├── Docs/                # Architecture specification and design documentation
├── HAL/                 # Hardware Abstraction Layer (GPIO, UART, SPI, I2C, Timer)
├── Middleware/          # TTC Scheduler, Bus Arbiter, PID, CRC, Packet Builder/Parser
├── Tests/               # Automated unit test suite (31 tests, 100% pass)
├── web/                 # 3D Digital Twin Web Application
│   ├── index.html       # Engineering workstation HUD layout
│   ├── style.css        # CAD Titanium/Charcoal theme styling
│   ├── scene3d.js       # Three.js CAD scene, PCB, motor & encoder models
│   ├── firmware_sim.js  # In-browser TTC scheduler & PID firmware model
│   └── app.js           # Digital Twin UI orchestration
├── CMakeLists.txt       # CMake build configuration
└── .gitignore
```

---

## Building and Testing

### Prerequisites
- CMake 3.15+
- C99-compliant compiler (MSVC, GCC, or Clang)

### Compilation
```bash
# Configure build
cmake -B build -S .

# Build main firmware executable
cmake --build build --config Debug --target HoneyBadger

# Build test suite executable
cmake --build build --config Debug --target HoneyBadger_Tests
```

### Running Unit Tests
```bash
./build/Debug/HoneyBadger_Tests.exe
```

Test Results:
```
==================================================
      HoneyBadger Firmware Automated Test Suite     
==================================================
=== Running CRC Tests === [PASS]
=== Running Bus Arbiter Tests === [PASS]
=== Running Data Store Double-Buffer Tests === [PASS]
=== Running PID Controller Tests === [PASS]
=== Running Packet Builder Tests === [PASS]
=== Running Packet Parser Tests === [PASS]
=== Running Scheduler Tests === [PASS]
--------------------------------------------------
Results: 31 / 31 tests PASSED (100.0%)
==================================================
```
