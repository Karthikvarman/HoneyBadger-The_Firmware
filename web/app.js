document.addEventListener('DOMContentLoaded', () => {

  const sim = new FirmwareSimulator();
  let scene3d = null;

  const waveformHistory = [];
  const MAX_WAVE_POINTS = 120;

  const hudUptime = document.getElementById('hud-uptime');
  const hudCpuLoad = document.getElementById('hud-cpu-load');
  const hudWdg = document.getElementById('hud-wdg');
  const hudMcuState = document.getElementById('hud-mcu-state');

  const btnReset = document.getElementById('btn-reset');
  const btnPause = document.getElementById('btn-pause-sim');
  const textPause = document.getElementById('text-pause');

  const telemSeq = document.getElementById('telem-seq');
  const telemCrc = document.getElementById('telem-crc');
  const telemBuf = document.getElementById('telem-buf');
  const rawHexDisplay = document.getElementById('raw-hex-display');
  const sensorTableBody = document.getElementById('sensor-table-body');

  const motSetpoint = document.getElementById('mot-setpoint');
  const motActual = document.getElementById('mot-actual');
  const motError = document.getElementById('mot-error');
  const motPwm = document.getElementById('mot-pwm');
  const setpointSlider = document.getElementById('setpoint-slider');
  const sliderTargetVal = document.getElementById('slider-target-val');
  const chkAutoSetpoint = document.getElementById('chk-auto-setpoint');
  const encStateA = document.getElementById('enc-state-a');
  const encStateB = document.getElementById('enc-state-b');
  const stallState = document.getElementById('stall-state');

  const taskTableBody = document.getElementById('task-table-body');
  const consoleOutput = document.getElementById('console-output');
  const btnClearConsole = document.getElementById('btn-clear-console');

  const compCard = document.getElementById('component-card');
  const cardTitle = document.getElementById('card-title');
  const cardSub = document.getElementById('card-sub');
  const cardContent = document.getElementById('card-content');
  const cardClose = document.getElementById('card-close');

  const busIndicators = {
    UART1: document.getElementById('bus-ind-uart1'),
    UART2: document.getElementById('bus-ind-uart2'),
    SPI1:  document.getElementById('bus-ind-spi1'),
    SPI2:  document.getElementById('bus-ind-spi2'),
    I2C1:  document.getElementById('bus-ind-i2c'),
    UART3: document.getElementById('bus-ind-uart3')
  };

  scene3d = new ECU3DScene('canvas-container', (data) => {

    cardTitle.textContent = data.name;
    cardSub.textContent = data.subtitle;

    let html = '';
    for (const item of data.details) {
      html += `<div class="card-item"><span class="key">${item.k}</span><span class="val">${item.v}</span></div>`;
    }
    cardContent.innerHTML = html;
    compCard.classList.remove('hidden');
  });

  cardClose.addEventListener('click', () => {
    compCard.classList.add('hidden');
  });

  sim.onBusActivity = (busName, action, target) => {
    if (scene3d) {
      scene3d.triggerBusPulse(busName, action, target);
    }

    const ind = busIndicators[busName];
    if (ind) {
      ind.classList.add('active');
      setTimeout(() => {
        ind.classList.remove('active');
      }, 140);
    }
  };

  sim.onLogMessage = (type, text) => {
    const entry = document.createElement('div');
    entry.className = `log-entry ${type}`;
    entry.textContent = text;
    consoleOutput.appendChild(entry);
    consoleOutput.scrollTop = consoleOutput.scrollHeight;
  };

  btnClearConsole.addEventListener('click', () => {
    consoleOutput.innerHTML = '';
  });

  document.querySelectorAll('.cam-btn').forEach(btn => {
    btn.addEventListener('click', () => {
      document.querySelectorAll('.cam-btn').forEach(b => b.classList.remove('active'));
      btn.classList.add('active');
      const camName = btn.getAttribute('data-cam');
      if (scene3d) scene3d.setCameraPreset(camName);
    });
  });

  document.querySelectorAll('.tab-btn').forEach(btn => {
    btn.addEventListener('click', () => {
      document.querySelectorAll('.tab-btn').forEach(b => b.classList.remove('active'));
      document.querySelectorAll('.tab-pane').forEach(p => p.classList.remove('active'));

      btn.classList.add('active');
      const targetId = btn.getAttribute('data-tab');
      const targetPane = document.getElementById(targetId);
      if (targetPane) targetPane.classList.add('active');
    });
  });

  setpointSlider.addEventListener('input', (e) => {
    const val = parseInt(e.target.value);
    sliderTargetVal.textContent = `${val} counts`;
    sim.receiveMotorSetpoint(val);
  });

  document.querySelectorAll('.btn-preset').forEach(btn => {
    btn.addEventListener('click', () => {
      const pos = parseInt(btn.getAttribute('data-pos'));
      setpointSlider.value = pos;
      sliderTargetVal.textContent = `${pos} counts`;
      sim.receiveMotorSetpoint(pos);
    });
  });

  chkAutoSetpoint.addEventListener('change', (e) => {
    sim.autoSetpointEnabled = e.target.checked;
  });

  btnReset.addEventListener('click', () => {
    sim.reset();
  });

  btnPause.addEventListener('click', () => {
    sim.isRunning = !sim.isRunning;
    if (sim.isRunning) {
      textPause.textContent = "Pause";
      hudMcuState.textContent = "RUNNING";
      hudMcuState.style.color = "#10b981";
    } else {
      textPause.textContent = "Resume";
      hudMcuState.textContent = "HALTED";
      hudMcuState.style.color = "#f43f5e";
    }
  });

  const canvas = document.getElementById('pid-waveform-canvas');
  const ctx = canvas.getContext('2d');

  function renderWaveform() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);

    ctx.strokeStyle = '#e2e8f0';
    ctx.lineWidth = 1;
    for (let y = 15; y < canvas.height; y += 30) {
      ctx.beginPath();
      ctx.moveTo(0, y);
      ctx.lineTo(canvas.width, y);
      ctx.stroke();
    }

    if (waveformHistory.length < 2) return;

    const centerY = canvas.height / 2;
    const scaleY = (canvas.height / 2) / 1100;
    const stepX = canvas.width / (MAX_WAVE_POINTS - 1);

    ctx.strokeStyle = '#64748b';
    ctx.lineWidth = 1.8;
    ctx.setLineDash([4, 4]);
    ctx.beginPath();
    for (let i = 0; i < waveformHistory.length; i++) {
      const x = i * stepX;
      const y = centerY - (waveformHistory[i].setpoint * scaleY);
      if (i === 0) ctx.moveTo(x, y);
      else ctx.lineTo(x, y);
    }
    ctx.stroke();
    ctx.setLineDash([]);

    ctx.strokeStyle = '#2563eb';
    ctx.lineWidth = 2.5;
    ctx.beginPath();
    for (let i = 0; i < waveformHistory.length; i++) {
      const x = i * stepX;
      const y = centerY - (waveformHistory[i].actual * scaleY);
      if (i === 0) ctx.moveTo(x, y);
      else ctx.lineTo(x, y);
    }
    ctx.stroke();
  }

  function renderSensorTable() {
    let rowsHtml = '';
    for (const s of sim.sensors) {
      rowsHtml += `
        <tr>
          <td><strong>${s.name}</strong></td>
          <td>${s.bus}</td>
          <td>${s.period}ms</td>
          <td>P${s.prio}</td>
          <td><span class="status-badge ${s.status.toLowerCase()}">${s.status}</span></td>
          <td>${s.val} ${s.unit}</td>
        </tr>
      `;
    }
    sensorTableBody.innerHTML = rowsHtml;
  }

  function renderTaskTable() {
    let rowsHtml = '';
    for (const t of sim.tasks) {
      rowsHtml += `
        <tr>
          <td><strong>${t.name}</strong></td>
          <td>${t.period}ms</td>
          <td>P${t.prio}</td>
          <td>${t.count}</td>
          <td>${t.worstUs} μs</td>
          <td><span class="status-pill active">READY</span></td>
        </tr>
      `;
    }
    taskTableBody.innerHTML = rowsHtml;
  }

  let lastFrameTime = performance.now();

  function loop() {
    const now = performance.now();
    const dt = now - lastFrameTime;
    lastFrameTime = now;

    if (sim.isRunning) {
      const steps = Math.min(25, Math.max(1, Math.round(dt)));
      for (let s = 0; s < steps; s++) {
        sim.tick(1);
      }
    }

    if (scene3d) {
      scene3d.updateFromFirmware(sim);
    }

    hudUptime.textContent = `${sim.sysTickMs} ms`;
    hudWdg.textContent = (sim.sysTickMs % 2000 < 1950) ? "ARMED" : "REFRESH";

    telemSeq.textContent = `#${sim.telemetry.seq.toString().padStart(3, '0')}`;
    telemBuf.textContent = (sim.activeWriteBuffer === 'A') ? "BUF_A" : "BUF_B";

    if (sim.telemetry.lastPktBytes.length > 0) {

      const b = sim.telemetry.lastPktBytes;
      const sof = `<span class="hex-header">${b[0].toString(16).padStart(2, '0').toUpperCase()}</span>`;
      const id  = `<span class="hex-header">${b[1].toString(16).padStart(2, '0').toUpperCase()}</span>`;
      const seq = `${b[2].toString(16).padStart(2, '0').toUpperCase()}`;
      const len = `${b[3].toString(16).padStart(2, '0').toUpperCase()} ${b[4].toString(16).padStart(2, '0').toUpperCase()}`;
      const payloadHex = b.slice(5, b.length - 3).map(byte => byte.toString(16).padStart(2, '0').toUpperCase()).join(' ');
      const crc = `<span class="hex-crc">${b[b.length - 3].toString(16).padStart(2, '0').toUpperCase()} ${b[b.length - 2].toString(16).padStart(2, '0').toUpperCase()}</span>`;
      const eof = `<span class="hex-header">${b[b.length - 1].toString(16).padStart(2, '0').toUpperCase()}</span>`;
      rawHexDisplay.innerHTML = `${sof} ${id} ${seq} ${len} ${payloadHex} ${crc} ${eof}`;
    }

    const m = sim.motor;
    const degTarget = (m.setpoint * 360 / m.countsPerRev).toFixed(1);
    const degActual = (m.currentPosition * 360 / m.countsPerRev).toFixed(1);
    motSetpoint.textContent = `${m.setpoint} cnt (${degTarget}°)`;
    motActual.textContent = `${m.currentPosition} cnt (${degActual}°)`;
    motError.textContent = `${m.error} cnt`;
    const pwmPct = ((Math.abs(m.outputPwm) / m.pwmMax) * 100).toFixed(0);
    const dirStr = m.dirA ? "CW" : "CCW";
    motPwm.textContent = `${m.outputPwm} / 999 (${pwmPct}% ${dirStr})`;

    encStateA.textContent = m.encChannelA ? "HIGH" : "LOW";
    encStateA.style.color = m.encChannelA ? "#10b981" : "#64748b";
    encStateB.textContent = m.encChannelB ? "HIGH" : "LOW";
    encStateB.style.color = m.encChannelB ? "#10b981" : "#64748b";

    if (m.stallDetected) {
      stallState.textContent = "STALL ACTIVE";
      stallState.className = "enc-state text-rose";
    } else {
      stallState.textContent = "NORMAL";
      stallState.className = "enc-state text-emerald";
    }

    waveformHistory.push({ setpoint: m.setpoint, actual: m.currentPosition });
    if (waveformHistory.length > MAX_WAVE_POINTS) {
      waveformHistory.shift();
    }
    renderWaveform();

    if (sim.sysTickMs % 100 < 20) {
      renderSensorTable();
      renderTaskTable();
    }

    requestAnimationFrame(loop);
  }

  renderSensorTable();
  renderTaskTable();

  requestAnimationFrame(loop);
});
