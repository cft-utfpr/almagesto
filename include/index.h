#ifndef INDEX_H
#define INDEX_H

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Astro Pan/Tilt Control</title>
  <style>
    :root {
      --bg-color: #0b0f19;
      --card-bg: #1a2333;
      --text-primary: #f8fafc;
      --text-secondary: #94a3b8;
      --accent: #38bdf8;
      --accent-hover: #0284c7;
      --border-color: #334155;
      --success: #22c55e;
      --error: #ef4444;
    }

    body {
      font-family: 'Segoe UI', system-ui, -apple-system, sans-serif;
      background-color: var(--bg-color);
      color: var(--text-primary);
      display: flex;
      justify-content: center;
      align-items: center;
      min-height: 100vh;
      margin: 0;
      background-image: radial-gradient(circle at 50% 0%, #1e293b 0%, transparent 70%);
    }

    .container {
      background: var(--card-bg);
      padding: 2.5rem 2rem;
      border-radius: 16px;
      box-shadow: 0 20px 40px rgba(0, 0, 0, 0.4), 0 0 0 1px var(--border-color);
      width: 100%;
      max-width: 380px;
      text-align: center;
    }

    h1 {
      margin-top: 0;
      margin-bottom: 1.5rem;
      color: var(--accent);
      font-weight: 300;
      letter-spacing: 3px;
      text-transform: uppercase;
      font-size: 1.75rem;
    }

    select {
      width: 100%;
      padding: 12px 16px;
      margin-bottom: 1.25rem;
      background: var(--bg-color);
      color: var(--text-primary);
      border: 1px solid var(--border-color);
      border-radius: 8px;
      font-size: 1rem;
      outline: none;
      transition: border-color 0.2s;
      cursor: pointer;
      appearance: none;
      background-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' fill='none' viewBox='0 0 24 24' stroke='%2394a3b8'%3E%3Cpath stroke-linecap='round' stroke-linejoin='round' stroke-width='2' d='M19 9l-7 7-7-7'%3E%3C/path%3E%3C/svg%3E");
      background-repeat: no-repeat;
      background-position: right 12px center;
      background-size: 16px;
    }

    select:focus {
      border-color: var(--accent);
      box-shadow: 0 0 0 2px rgba(56, 189, 248, 0.2);
    }

    button {
      width: 100%;
      padding: 14px;
      background: var(--accent);
      color: #0f172a;
      border: none;
      border-radius: 8px;
      font-size: 1.1rem;
      font-weight: 600;
      cursor: pointer;
      transition: all 0.2s ease;
      letter-spacing: 0.5px;
    }

    button:hover {
      background: var(--accent-hover);
      color: #fff;
      transform: translateY(-1px);
      box-shadow: 0 4px 12px rgba(2, 132, 199, 0.4);
    }

    button:active {
      transform: translateY(0);
    }

    .data-box, .status-box {
      margin-top: 1.5rem;
      padding: 1.5rem;
      background: rgba(11, 15, 25, 0.6);
      border: 1px solid var(--border-color);
      border-radius: 12px;
      text-align: left;
      animation: fadeIn 0.4s ease-out;
    }

    .data-box h2 {
      font-size: 1rem;
      margin-top: 0;
      margin-bottom: 1rem;
      color: var(--text-secondary);
      border-bottom: 1px solid var(--border-color);
      padding-bottom: 0.75rem;
      text-transform: uppercase;
      letter-spacing: 1px;
    }

    .data-box p {
      margin: 0.75rem 0;
      display: flex;
      justify-content: space-between;
      align-items: center;
      font-size: 1.1rem;
      color: var(--text-secondary);
    }

    .data-box span {
      font-weight: 600;
      color: var(--text-primary);
      font-family: monospace;
      font-size: 1.2rem;
    }

    /* New SSE Status Box Styling */
    .status-header {
      display: flex;
      align-items: center;
      gap: 8px;
      font-size: 0.85rem;
      text-transform: uppercase;
      letter-spacing: 1px;
      color: var(--text-secondary);
      margin-bottom: 0.5rem;
    }

    .status-dot {
      width: 8px;
      height: 8px;
      border-radius: 50%;
      background: var(--text-secondary); /* Default offline color */
      transition: background-color 0.3s, box-shadow 0.3s;
    }

    .status-dot.connected {
      background: var(--success);
      box-shadow: 0 0 8px var(--success);
      animation: pulse 2s infinite;
    }

    .status-dot.error {
      background: var(--error);
      box-shadow: 0 0 8px var(--error);
    }

    #liveStatus {
      margin: 0;
      font-family: monospace;
      font-size: 1rem;
      color: var(--accent);
      min-height: 1.5rem;
    }

    @keyframes fadeIn {
      from { opacity: 0; transform: translateY(10px); }
      to { opacity: 1; transform: translateY(0); }
    }

    @keyframes pulse {
      0% { opacity: 1; }
      50% { opacity: 0.5; }
      100% { opacity: 1; }
    }
  </style>
</head>
<body>

  <div class="container">
    <h1>Almagesto</h1>
    
    <select id="constSelect"></select>
    <button onclick="calculateTarget()">Aim Tracker</button>

    <div class="data-box" id="resultBox" style="display:none;">
      <h2>Target Computed</h2>
      <p>Pan (Azimuth) <span id="panVal">--&deg;</span></p>
      <p>Tilt (Altitude) <span id="tiltVal">--&deg;</span></p>
    </div>

    <!-- New SSE Live Status Box -->
    <div class="status-box">
      <div class="status-header">
        <div class="status-dot" id="statusDot"></div> Live Status
      </div>
      <p id="liveStatus">Waiting for connection...</p>
    </div>
  </div>

  <script>
    // Complete 88 Constellations Database
    const constellations = [
      {name: "Andromeda", ra: 0.8, dec: 37.4}, {name: "Antlia", ra: 10.3, dec: -32.5},
      {name: "Apus", ra: 16.1, dec: -75.3}, {name: "Aquarius", ra: 22.3, dec: -10.8},
      {name: "Aquila", ra: 19.3, dec: 3.4}, {name: "Ara", ra: 17.4, dec: -53.6},
      {name: "Aries", ra: 2.6, dec: 20.8}, {name: "Auriga", ra: 6.0, dec: 42.0},
      {name: "Boötes", ra: 14.7, dec: 31.2}, {name: "Caelum", ra: 4.7, dec: -38.0},
      {name: "Camelopardalis", ra: 5.8, dec: 69.4}, {name: "Cancer", ra: 8.6, dec: 19.8},
      {name: "Canes Venatici", ra: 13.1, dec: 40.1}, {name: "Canis Major", ra: 6.8, dec: -22.0},
      {name: "Canis Minor", ra: 7.6, dec: 6.4}, {name: "Capricornus", ra: 21.0, dec: -18.0},
      {name: "Carina", ra: 8.7, dec: -63.2}, {name: "Cassiopeia", ra: 1.3, dec: 62.3},
      {name: "Centaurus", ra: 13.1, dec: -47.3}, {name: "Cepheus", ra: 22.0, dec: 71.0},
      {name: "Cetus", ra: 1.7, dec: -7.2}, {name: "Chamaeleon", ra: 10.7, dec: -79.2},
      {name: "Circinus", ra: 15.3, dec: -63.0}, {name: "Columba", ra: 5.8, dec: -35.1},
      {name: "Coma Berenices", ra: 12.8, dec: 23.3}, {name: "Corona Australis", ra: 18.6, dec: -41.1},
      {name: "Corona Borealis", ra: 15.8, dec: 32.6}, {name: "Corvus", ra: 12.4, dec: -18.4},
      {name: "Crater", ra: 11.4, dec: -15.9}, {name: "Crux", ra: 12.4, dec: -60.2},
      {name: "Cygnus", ra: 20.6, dec: 42.0}, {name: "Delphinus", ra: 20.7, dec: 11.7},
      {name: "Dorado", ra: 5.2, dec: -59.4}, {name: "Draco", ra: 17.0, dec: 65.4},
      {name: "Equuleus", ra: 21.2, dec: 7.8}, {name: "Eridanus", ra: 3.3, dec: -29.9},
      {name: "Fornax", ra: 2.8, dec: -31.6}, {name: "Gemini", ra: 7.1, dec: 22.6},
      {name: "Grus", ra: 22.5, dec: -46.4}, {name: "Hercules", ra: 17.4, dec: 27.5},
      {name: "Horologium", ra: 3.3, dec: -53.3}, {name: "Hydra", ra: 11.6, dec: -14.5},
      {name: "Hydrus", ra: 2.3, dec: -70.0}, {name: "Indus", ra: 21.0, dec: -59.7},
      {name: "Lacerta", ra: 22.5, dec: 46.0}, {name: "Leo", ra: 10.7, dec: 13.1},
      {name: "Leo Minor", ra: 10.2, dec: 32.1}, {name: "Lepus", ra: 5.6, dec: -19.0},
      {name: "Libra", ra: 15.2, dec: -15.2}, {name: "Lupus", ra: 15.2, dec: -42.7},
      {name: "Lynx", ra: 8.0, dec: 47.5}, {name: "Lyra", ra: 18.9, dec: 36.7},
      {name: "Mensa", ra: 5.4, dec: -77.5}, {name: "Microscopium", ra: 21.0, dec: -36.3},
      {name: "Monoceros", ra: 7.1, dec: -5.7}, {name: "Musca", ra: 12.6, dec: -70.2},
      {name: "Norma", ra: 15.9, dec: -51.3}, {name: "Octans", ra: 23.0, dec: -82.2},
      {name: "Ophiuchus", ra: 17.4, dec: -7.9}, {name: "Orion", ra: 5.6, dec: 0.3},
      {name: "Pavo", ra: 19.6, dec: -65.8}, {name: "Pegasus", ra: 22.7, dec: 19.4},
      {name: "Perseus", ra: 3.2, dec: 45.0}, {name: "Phoenix", ra: 0.9, dec: -48.8},
      {name: "Pictor", ra: 5.7, dec: -53.5}, {name: "Pisces", ra: 0.5, dec: 13.7},
      {name: "Piscis Austrinus", ra: 22.3, dec: -30.6}, {name: "Puppis", ra: 7.2, dec: -31.2},
      {name: "Pyxis", ra: 8.9, dec: -27.4}, {name: "Reticulum", ra: 3.9, dec: -60.0},
      {name: "Sagitta", ra: 19.6, dec: 18.8}, {name: "Sagittarius", ra: 19.1, dec: -28.5},
      {name: "Scorpius", ra: 16.9, dec: -30.7}, {name: "Sculptor", ra: 0.4, dec: -32.1},
      {name: "Scutum", ra: 18.7, dec: -9.9}, {name: "Serpens", ra: 16.9, dec: 6.1},
      {name: "Sextans", ra: 10.3, dec: -2.6}, {name: "Taurus", ra: 4.7, dec: 14.9},
      {name: "Telescopium", ra: 19.3, dec: -51.0}, {name: "Triangulum", ra: 2.2, dec: 31.5},
      {name: "Triangulum Australe", ra: 16.1, dec: -65.4}, {name: "Tucana", ra: 23.8, dec: -65.8},
      {name: "Ursa Major", ra: 11.3, dec: 50.7}, {name: "Ursa Minor", ra: 15.0, dec: 77.7},
      {name: "Vela", ra: 9.6, dec: -47.2}, {name: "Virgo", ra: 13.4, dec: -4.2},
      {name: "Volans", ra: 7.8, dec: -69.8}, {name: "Vulpecula", ra: 20.2, dec: 24.4}
    ];

    // Populate dropdown
    const select = document.getElementById("constSelect");
    constellations.sort((a, b) => a.name.localeCompare(b.name)).forEach((c, index) => {
      let opt = document.createElement("option");
      opt.value = index;
      opt.innerHTML = c.name;
      select.appendChild(opt);
    });

    // --- Server-Sent Events (SSE) Implementation ---
    const statusDot = document.getElementById("statusDot");
    const liveStatus = document.getElementById("liveStatus");

    // Connect to the ESP32 SSE endpoint
    const evtSource = new EventSource("/events");

    evtSource.onopen = () => {
      statusDot.className = "status-dot connected";
      liveStatus.innerText = "System ready. Awaiting target.";
    };

    // Listen for incoming plain-text messages
    evtSource.onmessage = (event) => {
      liveStatus.innerText = event.data;
    };

    evtSource.onerror = () => {
      statusDot.className = "status-dot error";
      liveStatus.innerText = "Connection lost. Reconnecting...";
    };
    // -----------------------------------------------

    // Send coordinates to ESP32 API
    function calculateTarget() {
      const target = constellations[select.value];
      
      document.getElementById("panVal").innerHTML = "...";
      document.getElementById("tiltVal").innerHTML = "...";
      document.getElementById("resultBox").style.display = "block";

      // Fire off the HTTP request to start movement
      fetch(`/target?ra=${target.ra}&dec=${target.dec}`)
        .then(response => response.json())
        .then(data => {
          document.getElementById("panVal").innerHTML = data.pan.toFixed(2) + "&deg;";
          document.getElementById("tiltVal").innerHTML = data.tilt.toFixed(2) + "&deg;";
        })
        .catch(err => {
          console.error("API error", err);
          document.getElementById("panVal").innerHTML = "Error";
          document.getElementById("tiltVal").innerHTML = "Error";
        });
    }
  </script>
</body>
</html>
)rawliteral";

#endif