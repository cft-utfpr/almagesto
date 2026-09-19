#ifndef INDEX_H
#define INDEX_H

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Almagesto</title>
  <style>
    body { font-family: sans-serif; text-align: center; margin-top: 40px; background: #121212; color: white; }
    button { padding: 12px 20px; font-size: 16px; margin: 8px; border: none; border-radius: 5px; color: white; cursor: pointer; }
    .red { background: #e74c3c; } .green { background: #2ecc71; } .blue { background: #3498db; } .off { background: #7f8c8d; }
  </style>
</head>
<body>
  <h1>test</h1>
  <p><a href="/set?r=128&g=0&b=0"><button class="red">Red</button></a></p>
  <p><a href="/set?r=0&g=128&b=0"><button class="green">Green</button></a></p>
  <p><a href="/set?r=0&g=0&b=128"><button class="blue">Blue</button></a></p>
  <p><a href="/set?r=0&g=0&b=0"><button class="off">Off</button></a></p>
</body>
</html>
)rawliteral";

#endif