class ECU3DScene {
  constructor(containerId, onSelectComponent) {
    this.container = document.getElementById(containerId);
    this.onSelectComponent = onSelectComponent;

    this.scene = null;
    this.camera = null;
    this.renderer = null;
    this.controls = null;

    this.raycastables = [];
    this.raycaster = new THREE.Raycaster();
    this.mouse = new THREE.Vector2();

    this.motorShaftGroup = null;
    this.encoderDiskMesh = null;
    this.mcuActivityLed = null;
    this.sensorLeds = {};
    this.activeParticles = [];

    this.cameraPositions = {
      iso:     { pos: new THREE.Vector3(13, 15, 18), target: new THREE.Vector3(0, 0, 0) },
      top:     { pos: new THREE.Vector3(0, 24, 0.1), target: new THREE.Vector3(0, 0, 0) },
      mcu:     { pos: new THREE.Vector3(0, 8, 7),    target: new THREE.Vector3(0, 0.5, 0) },
      motor:   { pos: new THREE.Vector3(-9, 8, 7),   target: new THREE.Vector3(-6.5, 1.2, 0) },
      sensors: { pos: new THREE.Vector3(8, 8, 7),    target: new THREE.Vector3(6.5, 0.5, 0) }
    };
    this.currentCameraTween = null;

    this.init();
  }

  init() {
    const width = this.container.clientWidth;
    const height = this.container.clientHeight;

    this.scene = new THREE.Scene();
    this.scene.background = new THREE.Color(0x141b26);
    this.scene.fog = new THREE.FogExp2(0x141b26, 0.015);

    this.camera = new THREE.PerspectiveCamera(45, width / height, 0.1, 1000);
    this.camera.position.set(13, 15, 18);

    this.renderer = new THREE.WebGLRenderer({ antialias: true, alpha: false });
    this.renderer.setSize(width, height);
    this.renderer.setPixelRatio(Math.min(window.devicePixelRatio, 2));
    this.renderer.shadowMap.enabled = true;
    this.renderer.shadowMap.type = THREE.PCFSoftShadowMap;
    this.container.appendChild(this.renderer.domElement);

    this.controls = new THREE.OrbitControls(this.camera, this.renderer.domElement);
    this.controls.enableDamping = true;
    this.controls.dampingFactor = 0.05;
    this.controls.maxPolarAngle = Math.PI / 2.05;
    this.controls.minDistance = 4;
    this.controls.maxDistance = 42;
    this.controls.target.set(0, 0, 0);

    this.setupLighting();

    this.buildGroundShadow();
    this.buildPCB();
    this.buildMCU();
    this.buildMotorAndEncoder();
    this.buildSensors();
    this.buildConnectorsAndTraces();

    window.addEventListener('resize', () => this.onResize());
    this.container.addEventListener('pointerdown', (e) => this.onPointerDown(e));

    this.animate();
  }

  setupLighting() {

    const ambientLight = new THREE.AmbientLight(0xffffff, 0.72);
    this.scene.add(ambientLight);

    const keyLight = new THREE.DirectionalLight(0xffffff, 1.05);
    keyLight.position.set(16, 26, 14);
    keyLight.castShadow = true;
    keyLight.shadow.mapSize.width = 2048;
    keyLight.shadow.mapSize.height = 2048;
    keyLight.shadow.camera.near = 0.5;
    keyLight.shadow.camera.far = 60;
    keyLight.shadow.bias = -0.0005;
    const d = 16;
    keyLight.shadow.camera.left = -d;
    keyLight.shadow.camera.right = d;
    keyLight.shadow.camera.top = d;
    keyLight.shadow.camera.bottom = -d;
    this.scene.add(keyLight);

    const fillLight = new THREE.DirectionalLight(0xe2e8f0, 0.45);
    fillLight.position.set(-16, 16, -12);
    this.scene.add(fillLight);

    const topLight = new THREE.PointLight(0xffffff, 0.35, 30);
    topLight.position.set(0, 10, 0);
    this.scene.add(topLight);
  }

  buildGroundShadow() {
    const shadowGeo = new THREE.PlaneGeometry(36, 30);
    const shadowMat = new THREE.ShadowMaterial({ opacity: 0.35 });
    const shadowPlane = new THREE.Mesh(shadowGeo, shadowMat);
    shadowPlane.rotation.x = -Math.PI / 2;
    shadowPlane.position.y = -0.05;
    shadowPlane.receiveShadow = true;
    this.scene.add(shadowPlane);
  }

  buildPCB() {

    const pcbGeo = new THREE.BoxGeometry(22, 0.4, 16);
    const pcbMat = new THREE.MeshStandardMaterial({
      color: 0x15803d,
      roughness: 0.38,
      metalness: 0.12
    });
    const pcb = new THREE.Mesh(pcbGeo, pcbMat);
    pcb.castShadow = true;
    pcb.receiveShadow = true;
    this.scene.add(pcb);

    const borderGeo = new THREE.BoxGeometry(21.6, 0.42, 15.6);
    const borderMat = new THREE.MeshStandardMaterial({
      color: 0xd97706,
      roughness: 0.28,
      metalness: 0.85
    });
    const border = new THREE.Mesh(borderGeo, borderMat);
    this.scene.add(border);

    const standoffGeo = new THREE.CylinderGeometry(0.35, 0.35, 0.8, 16);
    const standoffMat = new THREE.MeshStandardMaterial({
      color: 0x94a3b8,
      metalness: 0.88,
      roughness: 0.22
    });
    const corners = [
      [-10.2, -7.2], [10.2, -7.2], [-10.2, 7.2], [10.2, 7.2]
    ];
    corners.forEach(([x, z]) => {
      const pin = new THREE.Mesh(standoffGeo, standoffMat);
      pin.position.set(x, 0.3, z);
      pin.castShadow = true;
      this.scene.add(pin);
    });

    const silkCanvas = document.createElement('canvas');
    silkCanvas.width = 1024;
    silkCanvas.height = 768;
    const ctx = silkCanvas.getContext('2d');
    ctx.fillStyle = '#15803d';
    ctx.fillRect(0, 0, 1024, 768);

    ctx.fillStyle = '#ffffff';
    ctx.font = 'bold 24px monospace';
    ctx.fillText('HONEYBADGER ECU v1.0', 60, 80);
    ctx.font = '16px monospace';
    ctx.fillText('BARE-METAL AUTOMOTIVE SENSOR & ACTUATOR SYSTEM', 60, 110);
    ctx.fillText('MOTOR & ENCODER (TIM2/TIM3)', 80, 710);
    ctx.fillText('MCU CORE', 480, 280);
    ctx.fillText('SPI1 / SPI2 SENSORS', 720, 200);
    ctx.fillText('I2C1 S7-S10 BUS', 720, 680);
    ctx.fillText('UART1 / UART2', 480, 80);

    const silkTex = new THREE.CanvasTexture(silkCanvas);
    const silkMat = new THREE.MeshBasicMaterial({ map: silkTex, transparent: true, opacity: 0.9 });
    const silkPlane = new THREE.Mesh(new THREE.PlaneGeometry(21.4, 15.4), silkMat);
    silkPlane.rotation.x = -Math.PI / 2;
    silkPlane.position.y = 0.22;
    this.scene.add(silkPlane);
  }

  buildMCU() {
    const mcuGroup = new THREE.Group();
    mcuGroup.position.set(0, 0.2, 0);

    const chipGeo = new THREE.BoxGeometry(4.2, 0.5, 4.2);
    const chipMat = new THREE.MeshStandardMaterial({
      color: 0x1e293b,
      roughness: 0.35,
      metalness: 0.2
    });
    const chip = new THREE.Mesh(chipGeo, chipMat);
    chip.castShadow = true;
    mcuGroup.add(chip);

    const spreaderGeo = new THREE.BoxGeometry(3.0, 0.52, 3.0);
    const spreaderMat = new THREE.MeshStandardMaterial({
      color: 0x334155,
      roughness: 0.3,
      metalness: 0.6
    });
    const spreader = new THREE.Mesh(spreaderGeo, spreaderMat);
    mcuGroup.add(spreader);

    const pinMat = new THREE.MeshStandardMaterial({ color: 0xd97706, metalness: 0.9, roughness: 0.2 });
    const pinGeo = new THREE.BoxGeometry(0.12, 0.08, 0.45);
    for (let i = -1.6; i <= 1.6; i += 0.26) {

      const p1 = new THREE.Mesh(pinGeo, pinMat);
      p1.position.set(i, 0.05, 2.2);
      mcuGroup.add(p1);

      const p2 = new THREE.Mesh(pinGeo, pinMat);
      p2.position.set(i, 0.05, -2.2);
      mcuGroup.add(p2);

      const p3 = new THREE.Mesh(pinGeo, pinMat);
      p3.rotation.y = Math.PI / 2;
      p3.position.set(2.2, 0.05, i);
      mcuGroup.add(p3);

      const p4 = new THREE.Mesh(pinGeo, pinMat);
      p4.rotation.y = Math.PI / 2;
      p4.position.set(-2.2, 0.05, i);
      mcuGroup.add(p4);
    }

    const ledGeo = new THREE.BoxGeometry(0.2, 0.2, 0.2);
    const ledMat = new THREE.MeshBasicMaterial({ color: 0x10b981 });
    this.mcuActivityLed = new THREE.Mesh(ledGeo, ledMat);
    this.mcuActivityLed.position.set(1.6, 0.35, 1.6);
    mcuGroup.add(this.mcuActivityLed);

    chip.userData = {
      type: "MCU",
      name: "HoneyBadger MCU Core",
      subtitle: "ARM Cortex-M @ 72 MHz • Bare-Metal TTC",
      details: [
        { k: "Core Frequency", v: "72.0 MHz (HSE + PLL)" },
        { k: "SysTick Timer", v: "1.0 ms Heartbeat Tick" },
        { k: "Scheduling Mode", v: "Time-Triggered Cooperative" },
        { k: "Memory Footprint", v: "Zero RTOS Overhead" },
        { k: "Watchdog (IWDG)", v: "2.0 s Hardware Protection" }
      ]
    };
    this.raycastables.push(chip);
    this.scene.add(mcuGroup);
  }

  buildMotorAndEncoder() {
    const motorGroup = new THREE.Group();
    motorGroup.position.set(-6.5, 0.2, 0);

    const baseGeo = new THREE.BoxGeometry(3.6, 0.3, 3.6);
    const baseMat = new THREE.MeshStandardMaterial({ color: 0x64748b, metalness: 0.8, roughness: 0.25 });
    const base = new THREE.Mesh(baseGeo, baseMat);
    base.position.y = 0.15;
    base.castShadow = true;
    motorGroup.add(base);

    const bodyGeo = new THREE.CylinderGeometry(1.5, 1.5, 3.2, 32);
    const bodyMat = new THREE.MeshStandardMaterial({
      color: 0xd1d5db,
      metalness: 0.88,
      roughness: 0.22
    });
    const body = new THREE.Mesh(bodyGeo, bodyMat);
    body.rotation.z = Math.PI / 2;
    body.position.set(-0.8, 1.6, 0);
    body.castShadow = true;
    motorGroup.add(body);

    const sinkGeo = new THREE.BoxGeometry(1.6, 1.2, 2.2);
    const sinkMat = new THREE.MeshStandardMaterial({ color: 0x334155, metalness: 0.82, roughness: 0.3 });
    const sink = new THREE.Mesh(sinkGeo, sinkMat);
    sink.position.set(1.4, 0.6, 0);
    sink.castShadow = true;
    motorGroup.add(sink);

    this.motorShaftGroup = new THREE.Group();
    this.motorShaftGroup.position.set(0.6, 1.6, 0);

    const shaftGeo = new THREE.CylinderGeometry(0.18, 0.18, 3.2, 24);
    const shaftMat = new THREE.MeshStandardMaterial({ color: 0xf8fafc, metalness: 0.95, roughness: 0.1 });
    const shaft = new THREE.Mesh(shaftGeo, shaftMat);
    shaft.rotation.z = Math.PI / 2;
    this.motorShaftGroup.add(shaft);

    const diskGeo = new THREE.CylinderGeometry(1.3, 1.3, 0.08, 32);
    const diskMat = new THREE.MeshStandardMaterial({
      color: 0x1e293b,
      metalness: 0.6,
      roughness: 0.35
    });
    this.encoderDiskMesh = new THREE.Mesh(diskGeo, diskMat);
    this.encoderDiskMesh.rotation.z = Math.PI / 2;
    this.encoderDiskMesh.position.x = 0.6;
    this.motorShaftGroup.add(this.encoderDiskMesh);

    for (let a = 0; a < Math.PI * 2; a += Math.PI / 8) {
      const slotGeo = new THREE.BoxGeometry(0.09, 0.25, 0.08);
      const slotMat = new THREE.MeshBasicMaterial({ color: 0x94a3b8 });
      const slot = new THREE.Mesh(slotGeo, slotMat);
      slot.position.set(0.6, Math.cos(a) * 1.05, Math.sin(a) * 1.05);
      this.motorShaftGroup.add(slot);
    }

    motorGroup.add(this.motorShaftGroup);

    const optoGeo = new THREE.BoxGeometry(0.4, 0.6, 0.4);
    const optoMat = new THREE.MeshStandardMaterial({ color: 0x0f172a });

    const optoA = new THREE.Mesh(optoGeo, optoMat);
    optoA.position.set(1.2, 2.7, 0);
    motorGroup.add(optoA);

    const optoB = new THREE.Mesh(optoGeo, optoMat);
    optoB.position.set(1.2, 1.6, 1.1);
    motorGroup.add(optoB);

    const beamMatA = new THREE.MeshBasicMaterial({ color: 0xef4444 });
    const beamA = new THREE.Mesh(new THREE.SphereGeometry(0.08, 8, 8), beamMatA);
    beamA.position.set(1.2, 2.5, 0);
    motorGroup.add(beamA);

    body.userData = {
      type: "MOTOR",
      name: "DC Actuator & Optical Encoder",
      subtitle: "TIM2 Hardware Quadrature + TIM3 20kHz PWM",
      details: [
        { k: "Encoder Resolution", v: "1,000 Counts/Rev (CPR)" },
        { k: "Encoder Mode", v: "TIM2 Quadrature Mode 3" },
        { k: "PID Control Rate", v: "10 ms Loop (TIM4 Interrupt)" },
        { k: "PWM Frequency", v: "20 kHz (TIM3 H-Bridge)" },
        { k: "Stall Safety", v: "Auto-clamp on >1s stall" }
      ]
    };
    this.raycastables.push(body);
    this.scene.add(motorGroup);
  }

  buildSensors() {
    const sensorDefs = [

      { id: 1, name: "S1 (UART1)", bus: "UART1", pos: [-1.8, 0.3, -5.5], color: 0x1d4ed8, period: "500 ms", prio: "1", type: "UART Module" },
      { id: 2, name: "S2 (UART2)", bus: "UART2", pos: [1.8, 0.3, -5.5],  color: 0x1d4ed8, period: "250 ms", prio: "2", type: "UART Module" },

      { id: 3, name: "S3 (SPI1)",  bus: "SPI1",  pos: [6.0, 0.3, -3.8], color: 0x0f172a, period: "10 ms",  prio: "3", type: "SPI Fast IC" },
      { id: 4, name: "S4 (SPI1)",  bus: "SPI1",  pos: [8.5, 0.3, -3.8], color: 0x0f172a, period: "1000 ms", prio: "4", type: "SPI Slow IC" },
      { id: 5, name: "S5 (SPI2)",  bus: "SPI2",  pos: [6.0, 0.3, -1.2], color: 0x0f172a, period: "800 ms",  prio: "5", type: "SPI Sensor IC" },
      { id: 6, name: "S6 (SPI2)",  bus: "SPI2",  pos: [8.5, 0.3, -1.2], color: 0x0f172a, period: "1500 ms", prio: "6", type: "SPI Sensor IC" },

      { id: 7,  name: "S7 (I2C1)",  bus: "I2C1", pos: [4.2, 0.3, 4.5],  color: 0x334155, period: "450 ms", prio: "7", type: "I2C Sensor" },
      { id: 8,  name: "S8 (I2C1)",  bus: "I2C1", pos: [6.0, 0.3, 4.5],  color: 0x334155, period: "450 ms", prio: "7", type: "I2C Sensor" },
      { id: 9,  name: "S9 (I2C1)",  bus: "I2C1", pos: [7.8, 0.3, 4.5],  color: 0x334155, period: "450 ms", prio: "7", type: "I2C Sensor" },
      { id: 10, name: "S10 (I2C1)", bus: "I2C1", pos: [9.6, 0.3, 4.5],  color: 0x334155, period: "450 ms", prio: "7", type: "I2C Sensor" }
    ];

    sensorDefs.forEach(s => {
      const sGroup = new THREE.Group();
      sGroup.position.set(...s.pos);

      const sGeo = new THREE.BoxGeometry(1.2, 0.3, 1.2);
      const sMat = new THREE.MeshStandardMaterial({
        color: s.color,
        roughness: 0.35,
        metalness: 0.3
      });
      const sMesh = new THREE.Mesh(sGeo, sMat);
      sMesh.castShadow = true;
      sGroup.add(sMesh);

      const pinMat = new THREE.MeshStandardMaterial({ color: 0xd97706, metalness: 0.85, roughness: 0.2 });
      const pin1 = new THREE.Mesh(new THREE.BoxGeometry(0.1, 0.08, 0.3), pinMat);
      pin1.position.set(-0.65, 0.04, 0);
      sGroup.add(pin1);
      const pin2 = new THREE.Mesh(new THREE.BoxGeometry(0.1, 0.08, 0.3), pinMat);
      pin2.position.set(0.65, 0.04, 0);
      sGroup.add(pin2);

      const ledGeo = new THREE.SphereGeometry(0.10, 10, 10);
      const ledMat = new THREE.MeshBasicMaterial({ color: 0x94a3b8 });
      const led = new THREE.Mesh(ledGeo, ledMat);
      led.position.set(0.35, 0.2, 0.35);
      sGroup.add(led);
      this.sensorLeds[s.id] = led;

      sMesh.userData = {
        type: "SENSOR",
        name: s.name,
        subtitle: `Bus: ${s.bus} • Priority ${s.prio}`,
        details: [
          { k: "Sensor Category", v: s.type },
          { k: "Sampling Period", v: s.period },
          { k: "Scheduler Priority", v: `P${s.prio}` },
          { k: "Bus Allocation", v: s.bus },
          { k: "Data Storage", v: "Double-Buffered (A/B)" },
          { k: "Diagnostics", v: "Timestamped in Telemetry" }
        ]
      };
      this.raycastables.push(sMesh);
      this.scene.add(sGroup);
    });
  }

  buildConnectorsAndTraces() {

    const connGeo = new THREE.BoxGeometry(2.4, 0.7, 1.0);
    const connMat = new THREE.MeshStandardMaterial({ color: 0x0284c7, metalness: 0.4 });
    const conn = new THREE.Mesh(connGeo, connMat);
    conn.position.set(-2, 0.45, 6.5);
    conn.castShadow = true;
    conn.userData = {
      type: "PORT",
      name: "UART3 Telemetry & Command Port",
      subtitle: "115,200 Baud • DMA Stream",
      details: [
        { k: "TX Telemetry", v: "Serialized Packet @ 500ms" },
        { k: "RX Setpoint", v: "Position Command @ 5s" },
        { k: "DMA Channel", v: "UART3_TX Channel 2" },
        { k: "Integrity Check", v: "CRC-16-CCITT Validated" }
      ]
    };
    this.raycastables.push(conn);
    this.scene.add(conn);

    const traceMat = new THREE.LineBasicMaterial({ color: 0xd97706, transparent: true, opacity: 0.65 });

    const createTrace = (p1, p2) => {
      const points = [
        new THREE.Vector3(p1[0], 0.22, p1[1]),
        new THREE.Vector3((p1[0] + p2[0]) / 2, 0.22, p1[1]),
        new THREE.Vector3((p1[0] + p2[0]) / 2, 0.22, p2[1]),
        new THREE.Vector3(p2[0], 0.22, p2[1])
      ];
      const geom = new THREE.BufferGeometry().setFromPoints(points);
      const line = new THREE.Line(geom, traceMat);
      this.scene.add(line);
    };

    createTrace([0, -2], [-1.8, -5.5]);
    createTrace([0, -2], [1.8, -5.5]);

    createTrace([2, 0], [6.0, -3.8]);
    createTrace([2, 0], [6.0, -1.2]);

    createTrace([2, 1], [4.2, 4.5]);

    createTrace([-2, 0], [-5.0, 0]);

    createTrace([0, 2], [-2, 6.5]);
  }

  triggerBusPulse(busName, action, target) {
    if (target && target.startsWith("S")) {
      const sId = parseInt(target.replace("S", ""));
      if (this.sensorLeds[sId]) {
        this.sensorLeds[sId].material.color.setHex(0x2563eb);
        setTimeout(() => {
          if (this.sensorLeds[sId]) this.sensorLeds[sId].material.color.setHex(0x94a3b8);
        }, 120);
      }
    }

    if (this.mcuActivityLed) {
      this.mcuActivityLed.material.color.setHex(0x2563eb);
      setTimeout(() => {
        if (this.mcuActivityLed) this.mcuActivityLed.material.color.setHex(0x10b981);
      }, 60);
    }

    let startPoint, endPoint;
    if (busName === "UART1") { startPoint = new THREE.Vector3(-1.8, 0.35, -5.5); endPoint = new THREE.Vector3(0, 0.35, -1.8); }
    else if (busName === "UART2") { startPoint = new THREE.Vector3(1.8, 0.35, -5.5); endPoint = new THREE.Vector3(0, 0.35, -1.8); }
    else if (busName === "SPI1") { startPoint = new THREE.Vector3(6.0, 0.35, -3.8); endPoint = new THREE.Vector3(2.0, 0.35, 0); }
    else if (busName === "SPI2") { startPoint = new THREE.Vector3(6.0, 0.35, -1.2); endPoint = new THREE.Vector3(2.0, 0.35, 0); }
    else if (busName === "I2C1") { startPoint = new THREE.Vector3(5.0, 0.35, 4.5); endPoint = new THREE.Vector3(1.8, 0.35, 1.5); }
    else if (busName === "UART3") { startPoint = new THREE.Vector3(0, 0.35, 2.0); endPoint = new THREE.Vector3(-2.0, 0.35, 6.5); }

    if (startPoint && endPoint) {
      this.spawnParticle(startPoint, endPoint, 0x2563eb);
    }
  }

  spawnParticle(from, to, colorHex) {
    const geo = new THREE.SphereGeometry(0.10, 8, 8);
    const mat = new THREE.MeshBasicMaterial({ color: colorHex });
    const p = new THREE.Mesh(geo, mat);
    p.position.copy(from);
    this.scene.add(p);

    this.activeParticles.push({
      mesh: p,
      from: from,
      to: to,
      progress: 0,
      speed: 0.08
    });
  }

  updateFromFirmware(firmwareSim) {

    if (this.motorShaftGroup) {
      this.motorShaftGroup.rotation.x = firmwareSim.motor.angularPosRad;
    }

    for (let i = this.activeParticles.length - 1; i >= 0; i--) {
      const item = this.activeParticles[i];
      item.progress += item.speed;
      item.mesh.position.lerpVectors(item.from, item.to, item.progress);
      if (item.progress >= 1.0) {
        this.scene.remove(item.mesh);
        item.mesh.geometry.dispose();
        item.mesh.material.dispose();
        this.activeParticles.splice(i, 1);
      }
    }

    if (this.currentCameraTween) {
      this.camera.position.lerp(this.currentCameraTween.pos, 0.07);
      this.controls.target.lerp(this.currentCameraTween.target, 0.07);
      if (this.camera.position.distanceTo(this.currentCameraTween.pos) < 0.08) {
        this.currentCameraTween = null;
      }
    }
  }

  setCameraPreset(name) {
    if (this.cameraPositions[name]) {
      this.currentCameraTween = this.cameraPositions[name];
    }
  }

  onPointerDown(event) {
    const rect = this.container.getBoundingClientRect();
    this.mouse.x = ((event.clientX - rect.left) / rect.width) * 2 - 1;
    this.mouse.y = -((event.clientY - rect.top) / rect.height) * 2 + 1;

    this.raycaster.setFromCamera(this.mouse, this.camera);
    const intersects = this.raycaster.intersectObjects(this.raycastables);

    if (intersects.length > 0) {
      const hit = intersects[0].object;
      if (hit.userData && this.onSelectComponent) {
        this.onSelectComponent(hit.userData);
      }
    }
  }

  onResize() {
    if (!this.container) return;
    const width = this.container.clientWidth;
    const height = this.container.clientHeight;
    this.camera.aspect = width / height;
    this.camera.updateProjectionMatrix();
    this.renderer.setSize(width, height);
  }

  animate() {
    requestAnimationFrame(() => this.animate());
    this.controls.update();
    this.renderer.render(this.scene, this.camera);
  }
}

window.ECU3DScene = ECU3DScene;
