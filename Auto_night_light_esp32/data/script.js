let dragging = false;

async function fetchStatus() {
  try {
    const res = await fetch('/api/status');
    const data = await res.json();
    render(data);
  } catch (e) { console.log('status fetch failed', e); }
}

function render(data) {
  document.getElementById('statusText').textContent = data.led ? 'ON' : 'OFF';
  document.getElementById('statusDot').className = 'status-dot ' + (data.led ? 'dot-on' : 'dot-off');
  document.getElementById('bulbIcon').className = 'bulb-icon' + (data.led ? ' lit' : '');

  document.getElementById('brightnessVal').textContent = data.brightness + ' / 255';
  document.getElementById('brightnessFill').style.width = (data.brightness / 255 * 100) + '%';

  document.getElementById('lightLevelVal').textContent = data.lightLevel;

  if (!dragging) {
    document.getElementById('thresholdSlider').value = data.threshold;
    document.getElementById('thresholdVal').textContent = data.threshold;
  }

  document.getElementById('autoBtn').classList.toggle('active', data.mode === 'auto');
  document.getElementById('manualBtn').classList.toggle('active', data.mode === 'manual');

  const manual = data.mode === 'manual';
  document.getElementById('onBtn').disabled = !manual;
  document.getElementById('offBtn').disabled = !manual;
}

async function setMode(mode) {
  await fetch('/api/mode/' + mode);
  fetchStatus();
}

async function setLed(state) {
  await fetch('/api/led/' + state);
  fetchStatus();
}

const slider = document.getElementById('thresholdSlider');
slider.addEventListener('input', () => {
  dragging = true;
  document.getElementById('thresholdVal').textContent = slider.value;
});
slider.addEventListener('change', async () => {
  await fetch('/api/threshold', {
    method: 'POST',
    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
    body: 'value=' + slider.value
  });
  dragging = false;
  fetchStatus();
});

fetchStatus();
setInterval(fetchStatus, 1500);
