class FirmwareSimulator {
  constructor() {
    this.sysTickMs = 0;
    this.isRunning = true;
    this.speedMultiplier = 1.0;

    this.sensors = [
      { id: 1, name: "S1",  bus: "UART1", period: 500,  prio: 1, status: "OK", len: 2, raw: [0x1A, 0x2B], val: 24.5, unit: "°C", lastRead: 0 },
      { id: 2, name: "S2",  bus: "UART2", period: 250,  prio: 2, status: "OK", len: 2, raw: [0x3C, 0x4D], val: 101.3, unit: "kPa", lastRead: 0 },
      { id: 3, name: "S3",  bus: "SPI1",  period: 10,   prio: 3, status: "OK", len: 4, raw: [0xDE, 0xAD, 0xBE, 0xEF], val: 1.024, unit: "g", lastRead: 0 },
      { id: 4, name: "S4",  bus: "SPI1",  period: 1000, prio: 4, status: "OK", len: 2, raw: [0x11, 0x22], val: 58.2, unit: "%RH", lastRead: 0 },
      { id: 5, name: "S5",  bus: "SPI2",  period: 800,  prio: 5, status: "OK", len: 2, raw: [0x33, 0x44], val: 12.4, unit: "V", lastRead: 0 },
      { id: 6, name: "S6",  bus: "SPI2",  period: 1500, prio: 6, status: "OK", len: 2, raw: [0x55, 0x66], val: 3.82, unit: "A", lastRead: 0 },
      { id: 7, name: "S7",  bus: "I2C1",  period: 450,  prio: 7, status: "OK", len: 2, raw: [0x77, 0x88], val: 420, unit: "ppm", lastRead: 0 },
      { id: 8, name: "S8",  bus: "I2C1",  period: 450,  prio: 7, status: "OK", len: 2, raw: [0x99, 0xAA], val: 1450, unit: "lux", lastRead: 0 },
      { id: 9, name: "S9",  bus: "I2C1",  period: 450,  prio: 7, status: "OK", len: 2, raw: [0xBB, 0xCC], val: 0.12, unit: "rad/s", lastRead: 0 },
      { id: 10, name: "S10", bus: "I2C1", period: 450, prio: 7, status: "OK", len: 2, raw: [0xDD, 0xEE], val: 28.1, unit: "psi", lastRead: 0 }
    ];

    this.bufferA = JSON.parse(JSON.stringify(this.sensors));
    this.bufferB = JSON.parse(JSON.stringify(this.sensors));
    this.activeWriteBuffer = 'A';

    this.busLocks = {
      SPI1: { isLocked: false, owner: null },
      SPI2: { isLocked: false, owner: null },
      I2C1: { isLocked: false, owner: null }
    };

    this.tasks = [
      { name: "UART3_TX",     period: 500,  prio: 0, tick: 0, ready: false, count: 0, worstUs: 152 },
      { name: "S1_UART1",     period: 500,  prio: 1, tick: 0, ready: false, count: 0, worstUs: 98 },
      { name: "S2_UART2",     period: 250,  prio: 2, tick: 0, ready: false, count: 0, worstUs: 94 },
      { name: "S3_SPI1",      period: 10,   prio: 3, tick: 0, ready: false, count: 0, worstUs: 140 },
      { name: "S4_SPI1",      period: 1000, prio: 4, tick: 0, ready: false, count: 0, worstUs: 135 },
      { name: "S5_SPI2",      period: 800,  prio: 5, tick: 0, ready: false, count: 0, worstUs: 138 },
      { name: "S6_SPI2",      period: 1500, prio: 6, tick: 0, ready: false, count: 0, worstUs: 142 },
      { name: "I2C_S7toS10",  period: 450,  prio: 7, tick: 0, ready: false, count: 0, worstUs: 1980 }
    ];

    this.tasks.sort((a, b) => a.prio - b.prio);

    this.motor = {
      setpoint: 0,
      currentPosition: 0,
      angularPosRad: 0,
      angularVelRadSec: 0,
      error: 0,
      prevError: 0,
      integral: 0,
      outputPwm: 0,
      dirA: 0,
      dirB: 0,
      kp: 500,
      ki: 50,
      kd: 100,
      integralMax: 500000,
      pwmMax: 999,
      stallDetected: false,
      stallCounterMs: 0,
      lastEncoderSample: 0,
      countsPerRev: 1000,
      encChannelA: false,
      encChannelB: false
    };

    this.telemetry = {
      seq: 0,
      lastPacketHex: "",
      lastPktBytes: [],
      crcValid: true,
      lastTxTime: 0
    };

    this.autoSetpointEnabled = true;
    this.lastSetpointRxTime = 0;
    this.setpointSequence = [250, 500, 0, -500, -250, 750, 0];
    this.setpointSeqIdx = 0;

    this.onBusActivity = null;
    this.onLogMessage = null;
  }

  calcCrc16(bytes) {
    let crc = 0xFFFF;
    for (let i = 0; i < bytes.length; i++) {
      crc ^= (bytes[i] << 8) & 0xFFFF;
      for (let bit = 0; bit < 8; bit++) {
        if (crc & 0x8000) {
          crc = ((crc << 1) ^ 0x1021) & 0xFFFF;
        } else {
          crc = (crc << 1) & 0xFFFF;
        }
      }
    }
    return crc;
  }

  calcCrc8(bytes) {
    let crc = 0x00;
    for (let i = 0; i < bytes.length; i++) {
      crc ^= bytes[i];
      for (let bit = 0; bit < 8; bit++) {
        if (crc & 0x80) {
          crc = ((crc << 1) ^ 0x07) & 0xFF;
        } else {
          crc = (crc << 1) & 0xFF;
        }
      }
    }
    return crc;
  }

  tick(dtMs = 1) {
    if (!this.isRunning) return;

    this.sysTickMs += dtMs;

    for (const task of this.tasks) {
      task.tick += dtMs;
      if (task.tick >= task.period) {
        task.ready = true;
        task.tick = 0;
      }
    }

    for (const task of this.tasks) {
      if (task.ready) {
        task.ready = false;
        task.count++;
        this.executeTask(task);
      }
    }

    if (this.sysTickMs % 10 === 0) {
      this.executePID(0.010);
    }

    if (this.autoSetpointEnabled && (this.sysTickMs - this.lastSetpointRxTime >= 5000)) {
      this.lastSetpointRxTime = this.sysTickMs;
      const target = this.setpointSequence[this.setpointSeqIdx];
      this.setpointSeqIdx = (this.setpointSeqIdx + 1) % this.setpointSequence.length;
      this.receiveMotorSetpoint(target);
    }
  }

  executeTask(task) {
    switch (task.name) {
      case "UART3_TX":
        this.taskTelemetryTX();
        break;
      case "S1_UART1":
        this.readSensor(1, "UART1");
        break;
      case "S2_UART2":
        this.readSensor(2, "UART2");
        break;
      case "S3_SPI1":
        this.readSensor(3, "SPI1");
        break;
      case "S4_SPI1":
        this.readSensor(4, "SPI1");
        break;
      case "S5_SPI2":
        this.readSensor(5, "SPI2");
        break;
      case "S6_SPI2":
        this.readSensor(6, "SPI2");
        break;
      case "I2C_S7toS10":
        this.readI2CSensors();
        break;
    }
  }

  readSensor(sensorId, busName) {
    const s = this.sensors.find(item => item.id === sensorId);
    if (!s) return;

    if (busName.startsWith("SPI")) {
      if (this.busLocks[busName].isLocked && this.busLocks[busName].owner !== sensorId) {
        s.status = "BUS_ERR";
        return;
      }
      this.busLocks[busName].isLocked = true;
      this.busLocks[busName].owner = sensorId;
    }

    const jitter = (Math.random() - 0.5) * 0.4;
    s.val = parseFloat((s.val + jitter * 0.05).toFixed(2));
    s.raw[0] = (s.raw[0] + 1) & 0xFF;
    s.lastRead = this.sysTickMs;
    s.status = "OK";

    const targetBuffer = (this.activeWriteBuffer === 'A') ? this.bufferA : this.bufferB;
    const bufEntry = targetBuffer.find(item => item.id === sensorId);
    if (bufEntry) {
      bufEntry.val = s.val;
      bufEntry.status = s.status;
      bufEntry.lastRead = s.lastRead;
      bufEntry.raw = [...s.raw];
    }

    if (busName.startsWith("SPI")) {
      this.busLocks[busName].isLocked = false;
      this.busLocks[busName].owner = null;
    }

    if (this.onBusActivity) {
      this.onBusActivity(busName, "read", s.name);
    }
  }

  readI2CSensors() {
    this.busLocks.I2C1.isLocked = true;
    this.busLocks.I2C1.owner = "S7-S10";

    for (let id = 7; id <= 10; id++) {
      const s = this.sensors.find(item => item.id === id);
      if (s) {
        s.val = parseFloat((s.val + (Math.random() - 0.5) * 0.1).toFixed(2));
        s.raw[0] = (s.raw[0] + 1) & 0xFF;
        s.lastRead = this.sysTickMs;
        s.status = "OK";

        const targetBuffer = (this.activeWriteBuffer === 'A') ? this.bufferA : this.bufferB;
        const bufEntry = targetBuffer.find(item => item.id === id);
        if (bufEntry) {
          bufEntry.val = s.val;
          bufEntry.status = s.status;
          bufEntry.lastRead = s.lastRead;
          bufEntry.raw = [...s.raw];
        }
      }
    }

    this.busLocks.I2C1.isLocked = false;
    this.busLocks.I2C1.owner = null;

    if (this.onBusActivity) {
      this.onBusActivity("I2C1", "read", "S7-S10");
    }
  }

  taskTelemetryTX() {

    this.activeWriteBuffer = (this.activeWriteBuffer === 'A') ? 'B' : 'A';
    const snapshotBuffer = (this.activeWriteBuffer === 'A') ? this.bufferB : this.bufferA;

    this.telemetry.seq = (this.telemetry.seq + 1) & 0xFF;
    const pktBytes = [];

    pktBytes.push(0xAA);
    pktBytes.push(0x01);
    pktBytes.push(this.telemetry.seq);

    const lenOffset = pktBytes.length;
    pktBytes.push(0x00, 0x00);

    const t = this.sysTickMs;
    pktBytes.push(t & 0xFF, (t >> 8) & 0xFF, (t >> 16) & 0xFF, (t >> 24) & 0xFF);

    const payloadStart = pktBytes.length;

    for (const sens of snapshotBuffer) {
      pktBytes.push(sens.id);
      pktBytes.push(sens.status === "OK" ? 0x00 : 0x01);
      pktBytes.push(sens.len);
      for (let b = 0; b < sens.len; b++) {
        pktBytes.push(sens.raw[b] !== undefined ? sens.raw[b] : 0x00);
      }
    }

    const payloadLen = pktBytes.length - payloadStart;
    pktBytes[lenOffset] = payloadLen & 0xFF;
    pktBytes[lenOffset + 1] = (payloadLen >> 8) & 0xFF;

    const crcPayload = pktBytes.slice(1);
    const crc = this.calcCrc16(crcPayload);

    pktBytes.push(crc & 0xFF);
    pktBytes.push((crc >> 8) & 0xFF);
    pktBytes.push(0x55);

    this.telemetry.lastPktBytes = pktBytes;
    this.telemetry.lastPacketHex = pktBytes.map(b => b.toString(16).padStart(2, '0').toUpperCase()).join(' ');
    this.telemetry.crcValid = true;
    this.telemetry.lastTxTime = this.sysTickMs;

    if (this.onBusActivity) {
      this.onBusActivity("UART3", "tx", `PKT #${this.telemetry.seq}`);
    }
    if (this.onLogMessage) {
      this.onLogMessage("tx", `[${(this.sysTickMs/1000).toFixed(3)}s] UART3 TX: Telemetry Frame #${this.telemetry.seq} (CRC16=0x${crc.toString(16).toUpperCase()}) sent via DMA.`);
    }
  }

  receiveMotorSetpoint(newTargetCounts) {

    const pos = Math.max(-1000000, Math.min(1000000, newTargetCounts));
    const posBytes = [
      pos & 0xFF,
      (pos >> 8) & 0xFF,
      (pos >> 16) & 0xFF,
      (pos >> 24) & 0xFF
    ];
    const crcPayload = [0x01, ...posBytes];
    const crc8 = this.calcCrc8(crcPayload);

    this.motor.setpoint = pos;

    if (this.onBusActivity) {
      this.onBusActivity("UART3", "rx", `SET_POS: ${pos}`);
    }
    if (this.onLogMessage) {
      this.onLogMessage("rx", `[${(this.sysTickMs/1000).toFixed(3)}s] UART3 RX: Motor Target updated to ${pos} counts (${(pos * 360 / this.motor.countsPerRev).toFixed(1)}°). CRC-8 Valid.`);
    }
  }

  executePID(dt = 0.010) {
    const m = this.motor;

    const error = m.setpoint - m.currentPosition;
    m.error = error;

    const pTerm = (m.kp * error) / 1000;

    m.integral += error;
    if (m.integral > m.integralMax) m.integral = m.integralMax;
    if (m.integral < -m.integralMax) m.integral = -m.integralMax;
    const iTerm = (m.ki * m.integral) / 1000;

    const derivative = error - m.prevError;
    const dTerm = (m.kd * derivative) / 1000;
    m.prevError = error;

    let output = Math.round(pTerm + iTerm + dTerm);
    if (output > m.pwmMax) output = m.pwmMax;
    if (output < -m.pwmMax) output = -m.pwmMax;

    m.outputPwm = output;

    if (output >= 0) {
      m.dirA = 1;
      m.dirB = 0;
    } else {
      m.dirA = 0;
      m.dirB = 1;
    }

    const motorTorque = (output / m.pwmMax) * 20.0;
    const friction = m.angularVelRadSec * 2.8;
    const angularAccel = motorTorque - friction;

    m.angularVelRadSec += angularAccel * dt;
    m.angularPosRad += m.angularVelRadSec * dt;

    const calculatedCounts = Math.round((m.angularPosRad / (2 * Math.PI)) * m.countsPerRev);
    m.currentPosition = calculatedCounts;

    const phase = m.angularPosRad * (m.countsPerRev / (2 * Math.PI));
    m.encChannelA = Math.sin(phase * 2 * Math.PI) > 0;
    m.encChannelB = Math.cos(phase * 2 * Math.PI) > 0;

    const deltaCount = Math.abs(m.currentPosition - m.lastEncoderSample);
    if (Math.abs(output) > 500 && deltaCount < 2) {
      m.stallCounterMs += 10;
      if (m.stallCounterMs > 1000) {
        m.stallDetected = true;
      }
    } else {
      m.stallCounterMs = 0;
      m.stallDetected = false;
    }
    m.lastEncoderSample = m.currentPosition;
  }

  reset() {
    this.sysTickMs = 0;
    this.motor.currentPosition = 0;
    this.motor.angularPosRad = 0;
    this.motor.angularVelRadSec = 0;
    this.motor.integral = 0;
    this.motor.prevError = 0;
    this.motor.setpoint = 0;
    this.telemetry.seq = 0;
    for (const task of this.tasks) {
      task.tick = 0;
      task.ready = false;
      task.count = 0;
    }
    if (this.onLogMessage) {
      this.onLogMessage("sys", `[0.000s] SYSTEM RESET EXECUTED. Microcontroller re-initialized.`);
    }
  }
}

window.FirmwareSimulator = FirmwareSimulator;
