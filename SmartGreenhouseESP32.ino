#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

// WiFi
const char* ssid = "Redmi Note 12 Pro 5G";
const char* password = "Ranuswadha";

WebServer server(80);

// DHT
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// RGB Pins (COMMON ANODE)
#define B1 25
#define G1 26
#define R1 27

#define B2 19
#define G2 21
#define R2 22

#define LDR_PIN 34

int crop1 = 1;
int crop2 = 2;

// ---------- PWM SETUP (ESP32 v3) ----------
void setupPWM() {
  ledcAttach(R1, 5000, 8);
  ledcAttach(G1, 5000, 8);
  ledcAttach(B1, 5000, 8);

  ledcAttach(R2, 5000, 8);
  ledcAttach(G2, 5000, 8);
  ledcAttach(B2, 5000, 8);
}

// ---------- LDR SMOOTH ----------
int readLDR() {
  int total = 0;
  for (int i = 0; i < 10; i++) {
    total += analogRead(LDR_PIN);
    delay(2);
  }
  return total / 10;
}

// ---------- HTML ----------
String htmlPage() {

  // ===== Selected dropdown logic =====
  String c1_1 = (crop1==1) ? "selected" : "";
  String c1_2 = (crop1==2) ? "selected" : "";
  String c1_3 = (crop1==3) ? "selected" : "";
  String c1_4 = (crop1==4) ? "selected" : "";

  String c2_1 = (crop2==1) ? "selected" : "";
  String c2_2 = (crop2==2) ? "selected" : "";
  String c2_3 = (crop2==3) ? "selected" : "";
  String c2_4 = (crop2==4) ? "selected" : "";

  // ===== Crop names =====
  String cropNames[] = {"","Lettuce","Tomato","Spinach","Strawberry"};

  String page = R"rawliteral(
  <!DOCTYPE html>
  <html>
  <head>
    <title>Smart Greenhouse</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">

    <style>
      body {
        font-family: Arial;
        background: linear-gradient(to right, #dff5e1, #e3f2fd);
        text-align: center;
        margin: 0;
      }

      h2 {
        padding: 15px;
        color: #1b5e20;
      }

      .container {
        display: flex;
        flex-wrap: wrap;
        justify-content: center;
      }

      .card {
        background: white;
        padding: 15px;
        margin: 10px;
        border-radius: 15px;
        width: 250px;
        box-shadow: 0 4px 12px rgba(0,0,0,0.1);
      }

      select, input {
        padding: 8px;
        margin: 5px;
        border-radius: 8px;
        border: 1px solid #ccc;
      }

      .value {
        font-size: 22px;
        font-weight: bold;
        color: #2e7d32;
      }

      .label {
        color: #555;
      }
    </style>
  </head>

  <body>

    <h2>Smart Greenhouse Dashboard</h2>

    <div class="card">
      <form action="/set">
        <h3>Select Crops</h3>

        LED1:
        <select name="c1">
          <option value="1" %C1_1%>Lettuce</option>
          <option value="2" %C1_2%>Tomato</option>
          <option value="3" %C1_3%>Spinach</option>
          <option value="4" %C1_4%>Strawberry</option>
        </select><br>

        LED2:
        <select name="c2">
          <option value="1" %C2_1%>Lettuce</option>
          <option value="2" %C2_2%>Tomato</option>
          <option value="3" %C2_3%>Spinach</option>
          <option value="4" %C2_4%>Strawberry</option>
        </select><br>

        <input type="submit" value="Update">
      </form>
    </div>

    <div class="container">

      <div class="card">
        <div class="label">LED1 Crop</div>
        <div class="value">%LED1%</div>
      </div>

      <div class="card">
        <div class="label">LED2 Crop</div>
        <div class="value">%LED2%</div>
      </div>

      <div class="card">
        <div class="label">Temperature</div>
        <div class="value"><span id="t">--</span> &deg;C</div>
      </div>

      <div class="card">
        <div class="label">Humidity</div>
        <div class="value"><span id="h">--</span> %</div>
      </div>

      <div class="card">
        <div class="label">Light (LDR)</div>
        <div class="value"><span id="l">--</span></div>
      </div>

    </div>

    <script>
      setInterval(() => {
        fetch("/data")
        .then(r => r.json())
        .then(d => {
          document.getElementById("t").innerText = d.temp;
          document.getElementById("h").innerText = d.hum;
          document.getElementById("l").innerText = d.ldr;
        });
      }, 1000);
    </script>

  </body>
  </html>
  )rawliteral";

  page.replace("%C1_1%", c1_1);
  page.replace("%C1_2%", c1_2);
  page.replace("%C1_3%", c1_3);
  page.replace("%C1_4%", c1_4);

  page.replace("%C2_1%", c2_1);
  page.replace("%C2_2%", c2_2);
  page.replace("%C2_3%", c2_3);
  page.replace("%C2_4%", c2_4);

  // Replace crop names
  page.replace("%LED1%", cropNames[crop1]);
  page.replace("%LED2%", cropNames[crop2]);

  return page;
}

// ---------- ROUTES ----------
void handleRoot() {
  server.send(200, "text/html", htmlPage());
}

void handleSet() {
  if (server.hasArg("c1")) crop1 = server.arg("c1").toInt();
  if (server.hasArg("c2")) crop2 = server.arg("c2").toInt();
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleData() {
  int ldr = readLDR();
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) {
    server.send(200, "application/json", "{\"temp\":0,\"hum\":0,\"ldr\":0}");
    return;
  }

  String json = "{";
  json += "\"temp\":" + String(temp) + ",";
  json += "\"hum\":" + String(hum) + ",";
  json += "\"ldr\":" + String(ldr);
  json += "}";

  server.send(200, "application/json", json);
}

// ---------- LED CONTROL ----------
void controlLED(int crop, int ldr, float temp, int rPin, int gPin, int bPin) {

  int r = 0, g = 0, b = 0;

// VERY BRIGHT SUNLIGHT
if (ldr > 3500) {
  if (crop == 1) { r=0; g=0; b=0; }              // Lettuce OFF
  else if (crop == 2) { r=50; g=0; b=0; }        // Tomato minimal red
  else if (crop == 3) { r=20; g=30; b=20; }      // Spinach minimal
  else if (crop == 4) { r=50; g=0; b=30; }       // Strawberry low
}

// BRIGHT
else if (ldr > 2800) {
  if (crop == 1) { b=40; }
  else if (crop == 2) { r=80; }
  else if (crop == 3) { r=40; g=60; b=40; }
  else if (crop == 4) { r=80; b=50; }
}

// MODERATE BRIGHT
else if (ldr > 2200) {
  if (crop == 1) { b=80; }
  else if (crop == 2) { r=120; }
  else if (crop == 3) { r=70; g=90; b=70; }
  else if (crop == 4) { r=120; b=80; }
}

// MEDIUM
else if (ldr > 1600) {
  if (crop == 1) { b=140; }
  else if (crop == 2) { r=160; }
  else if (crop == 3) { r=100; g=130; b=100; }
  else if (crop == 4) { r=160; b=100; }
}

// LOW LIGHT
else if (ldr > 800) {
  if (crop == 1) { b=200; }
  else if (crop == 2) { r=200; }
  else if (crop == 3) { r=130; g=170; b=130; }
  else if (crop == 4) { r=200; b=120; }
}

// VERY DARK
else {
  if (crop == 1) { b=255; }
  else if (crop == 2) { r=255; }
  else if (crop == 3) { r=150; g=200; b=150; }
  else if (crop == 4) { r=255; b=140; }
}


// ===== TEMPERATURE CONTROL =====

if (temp > 35) {
  r *= 0.5;
  g *= 0.8;
  b *= 0.8;
}
else if (temp > 30) {
  r *= 0.7;
  g *= 0.9;
  b *= 0.9;
}
else if (temp >= 22 && temp <= 30) {
  // optimal → no change
}
else if (temp > 18) {
  r *= 1.05;
  g *= 1.05;
  b *= 1.05;
}
else {
  r *= 1.15;
  g *= 1.15;
  b *= 1.15;
}


// ===== SAFETY =====
r = constrain(r, 0, 255);
g = constrain(g, 0, 255);
b = constrain(b, 0, 255);

  r = constrain(r, 0, 255);
  g = constrain(g, 0, 255);
  b = constrain(b, 0, 255);

  // COMMON ANODE FIX
  ledcWrite(rPin, 255 - r);
  ledcWrite(gPin, 255 - g);
  ledcWrite(bPin, 255 - b);
}

// ---------- SETUP ----------
void setup() {
  Serial.begin(115200);

  WiFi.setSleep(false);
  dht.begin();
  setupPWM();

  WiFi.begin(ssid, password);

  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/set", handleSet);
  server.on("/data", handleData);

  server.begin();
}

// ---------- LOOP ----------
void loop() {
  server.handleClient();

  int ldr = readLDR();
  float temp = dht.readTemperature();

  if (!isnan(temp)) {
    controlLED(crop1, ldr, temp, R1, G1, B1);
    controlLED(crop2, ldr, temp, R2, G2, B2);
  }

  delay(500);
}
