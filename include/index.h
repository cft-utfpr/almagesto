#ifndef INDEX_H
#define INDEX_H

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Astro Pan/Tilt Control</title>
</head>
<body>
  <h1>Almagesto</h1>
  
  <select id="constSelect"></select>
  <button onclick="calculateTarget()">Aim Tracker</button>

  <br>
  <div class="data-box" id="resultBox" style="display:none;">
    <h2>Target Computed</h2>
    <p><strong>Pan (Azimuth):</strong> <span id="panVal">--</span>&deg;</p>
    <p><strong>Tilt (Altitude):</strong> <span id="tiltVal">--</span>&deg;</p>
  </div>

  <script>
    // Complete 88 Constellations Database (RA in hours, Dec in degrees)
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

    // Populate dropdown alphabetically
    const select = document.getElementById("constSelect");
    constellations.sort((a, b) => a.name.localeCompare(b.name)).forEach((c, index) => {
      let opt = document.createElement("option");
      opt.value = index;
      opt.innerHTML = c.name;
      select.appendChild(opt);
    });

    // Send coordinates to ESP32 API
    function calculateTarget() {
      const target = constellations[select.value];
      
      fetch(`/target?ra=${target.ra}&dec=${target.dec}`)
        .then(response => response.json())
        .then(data => {
          document.getElementById("resultBox").style.display = "block";
          document.getElementById("panVal").innerText = data.pan.toFixed(2);
          document.getElementById("tiltVal").innerText = data.tilt.toFixed(2);
        });
    }
  </script>
</body>
</html>
)rawliteral";

#endif