#include "HTTPApiHandler.h"

HTTPApiHandler::HTTPApiHandler(AsyncWebServer& server, CommandHandler& cmdHandler)
    : _server(server), _commandHandler(cmdHandler) {
}

void HTTPApiHandler::begin() {
    Serial.println("Initializing HTTP API Handler...");
    setupRoutes();
    Serial.println("✓ HTTP API routes configured\n");
}

void HTTPApiHandler::logRequest(const char* method, const char* endpoint, const String& params) {
    Serial.printf("\n[HTTP %s] %s", method, endpoint);
    if (params.length() > 0) {
        Serial.printf("?%s", params.c_str());
    }
    Serial.println();
}

void HTTPApiHandler::logResponse(JsonDocument& response) {
    Serial.println("[HTTP Response]:");
    serializeJsonPretty(response, Serial);
    Serial.println("\n");
}

void HTTPApiHandler::handleAPIRequest(AsyncWebServerRequest *request, const char* endpoint, const String& cmd) {
    logRequest("GET", endpoint, "");
    
    JsonDocument response;
    _commandHandler.processCommand((uint8_t*)cmd.c_str(), cmd.length(), response);
    
    logResponse(response);
    
    String jsonResponse;
    serializeJson(response, jsonResponse);
    request->send(200, "application/json", jsonResponse);
}

void HTTPApiHandler::setupRoutes() {
    // Enable CORS for all responses
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Content-Type");
    
    // Root endpoint - Interactive Control Panel
    _server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", R"rawliteral(
<!DOCTYPE html>
<html lang="tr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>VanSightHub Kontrol</title>
    <style>
        *{margin:0;padding:0;box-sizing:border-box}body{font-family:'Segoe UI',sans-serif;background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);min-height:100vh;padding:20px}.container{max-width:1200px;margin:0 auto}.header{background:rgba(255,255,255,0.95);backdrop-filter:blur(10px);border-radius:20px;padding:30px;margin-bottom:20px;box-shadow:0 8px 32px rgba(0,0,0,0.1)}h1{color:#667eea;font-size:2.5em;margin-bottom:10px}.status{display:inline-block;padding:8px 16px;background:#10b981;color:white;border-radius:20px;font-size:0.9em;font-weight:600}.grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(350px,1fr));gap:20px;margin-bottom:20px}.card{background:rgba(255,255,255,0.95);backdrop-filter:blur(10px);border-radius:20px;padding:25px;box-shadow:0 8px 32px rgba(0,0,0,0.1)}.card h2{color:#333;margin-bottom:20px;font-size:1.5em;display:flex;align-items:center;gap:10px}.relay-grid{display:grid;grid-template-columns:repeat(4,1fr);gap:10px}.relay-btn{padding:15px;border:none;border-radius:12px;font-size:1em;font-weight:600;cursor:pointer;transition:all 0.3s ease;background:#e5e7eb;color:#6b7280}.relay-btn.on{background:linear-gradient(135deg,#10b981 0%,#059669 100%);color:white;box-shadow:0 4px 12px rgba(16,185,129,0.4)}.relay-btn:hover{transform:translateY(-2px);box-shadow:0 6px 16px rgba(0,0,0,0.15)}.relay-btn:active{transform:translateY(0)}.control-group{display:flex;gap:10px;margin-top:15px}.btn{flex:1;padding:12px;border:none;border-radius:12px;font-size:1em;font-weight:600;cursor:pointer;transition:all 0.3s ease}.btn-primary{background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);color:white}.btn-danger{background:linear-gradient(135deg,#ef4444 0%,#dc2626 100%);color:white}.btn:hover{transform:translateY(-2px);box-shadow:0 6px 16px rgba(0,0,0,0.2)}.sensor-item{background:#f9fafb;padding:15px;border-radius:12px;margin-bottom:10px}.sensor-header{display:flex;justify-content:space-between;align-items:center;margin-bottom:10px}.sensor-name{font-weight:600;color:#374151}.sensor-value{font-size:1.2em;font-weight:700;color:#667eea}.progress-bar{width:100%;height:8px;background:#e5e7eb;border-radius:4px;overflow:hidden}.progress-fill{height:100%;background:linear-gradient(90deg,#10b981 0%,#059669 100%);transition:width 0.3s ease}.sensor-details{display:flex;justify-content:space-between;margin-top:8px;font-size:0.85em;color:#6b7280}.refresh-btn{background:linear-gradient(135deg,#3b82f6 0%,#2563eb 100%);color:white;border:none;padding:10px 20px;border-radius:8px;cursor:pointer;font-weight:600;margin-top:10px;width:100%}.refresh-btn:hover{transform:translateY(-2px);box-shadow:0 4px 12px rgba(59,130,246,0.4)}@media (max-width:768px){.relay-grid{grid-template-columns:repeat(3,1fr)}h1{font-size:1.8em}}
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🚐 VanSightHub</h1>
            <span class="status" id="connectionStatus">● Bağlı</span>
        </div>
        <div class="grid">
            <div class="card">
                <h2><span>⚡</span> Röle Kontrolü</h2>
                <div class="relay-grid" id="relayGrid"></div>
                <div class="control-group">
                    <button class="btn btn-primary" onclick="allRelaysOn()">Tümünü Aç</button>
                    <button class="btn btn-danger" onclick="allRelaysOff()">Tümünü Kapat</button>
                </div>
            </div>
            <div class="card">
                <h2><span>📊</span> Seviye Sensörleri</h2>
                <div id="sensorContainer"></div>
                <button class="refresh-btn" onclick="refreshSensors()">🔄 Yenile</button>
            </div>
        </div>
    </div>
    <script>
        const API_BASE='';let relayStates=Array(16).fill(false);window.onload=function(){initializeRelays();loadAllStatus();setInterval(refreshSensors,5000)};function initializeRelays(){const grid=document.getElementById('relayGrid');for(let i=1;i<=16;i++){const btn=document.createElement('button');btn.className='relay-btn';btn.id=`relay-${i}`;btn.textContent=`R${i}`;btn.onclick=()=>toggleRelay(i);grid.appendChild(btn)}}async function toggleRelay(num){try{const response=await fetch(`${API_BASE}/relay/toggle?num=${num}`);const data=await response.json();if(data.status==='ok'){updateRelayButton(num,data.data.state==='on')}}catch(error){console.error('Hata:',error);updateConnectionStatus(false)}}async function allRelaysOn(){try{const response=await fetch(`${API_BASE}/relays/on`);const data=await response.json();if(data.status==='ok'){for(let i=1;i<=16;i++){updateRelayButton(i,true)}}}catch(error){console.error('Hata:',error)}}async function allRelaysOff(){try{const response=await fetch(`${API_BASE}/relays/off`);const data=await response.json();if(data.status==='ok'){for(let i=1;i<=16;i++){updateRelayButton(i,false)}}}catch(error){console.error('Hata:',error)}}function updateRelayButton(num,isOn){const btn=document.getElementById(`relay-${num}`);if(isOn){btn.classList.add('on')}else{btn.classList.remove('on')}relayStates[num-1]=isOn}async function refreshSensors(){try{const response=await fetch(`${API_BASE}/sensors`);const data=await response.json();if(data.status==='ok'){displaySensors(data.data.sensors);updateConnectionStatus(true)}}catch(error){console.error('Hata:',error);updateConnectionStatus(false)}}function displaySensors(sensors){const container=document.getElementById('sensorContainer');container.innerHTML='';sensors.forEach(sensor=>{const sensorDiv=document.createElement('div');sensorDiv.className='sensor-item';sensorDiv.innerHTML=`<div class="sensor-header"><span class="sensor-name">Sensör ${sensor.id}</span><span class="sensor-value">${sensor.level}%</span></div><div class="progress-bar"><div class="progress-fill" style="width: ${sensor.level}%"></div></div><div class="sensor-details"><span>Direnç: ${sensor.resistance.toFixed(1)}Ω</span><span>Seviye: ${sensor.level}%</span></div>`;container.appendChild(sensorDiv)})}async function loadAllStatus(){try{const response=await fetch(`${API_BASE}/status`);const data=await response.json();if(data.status==='ok'){data.data.relays.forEach((state,index)=>{updateRelayButton(index+1,state===1)});displaySensors(data.data.sensors);updateConnectionStatus(true)}}catch(error){console.error('Hata:',error);updateConnectionStatus(false)}}function updateConnectionStatus(isConnected){const status=document.getElementById('connectionStatus');if(isConnected){status.textContent='● Bağlı';status.style.background='#10b981'}else{status.textContent='● Bağlantı Yok';status.style.background='#ef4444'}}
    </script>
</body>
</html>
)rawliteral");
    });
    
    // Relay endpoints
    _server.on("/relay/on", HTTP_GET, [this](AsyncWebServerRequest *request){
        if (!request->hasParam("num")) {
            request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing num parameter\"}");
            return;
        }
        int relayNum = request->getParam("num")->value().toInt();
        String params = "num=" + String(relayNum);
        logRequest("GET", "/relay/on", params);
        
        String cmd = "{\"cmd\":\"relay_on\",\"relay\":" + String(relayNum) + "}";
        JsonDocument response;
        _commandHandler.processCommand((uint8_t*)cmd.c_str(), cmd.length(), response);
        
        logResponse(response);
        
        String jsonResponse;
        serializeJson(response, jsonResponse);
        request->send(200, "application/json", jsonResponse);
    });
    
    _server.on("/relay/off", HTTP_GET, [this](AsyncWebServerRequest *request){
        if (!request->hasParam("num")) {
            request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing num parameter\"}");
            return;
        }
        int relayNum = request->getParam("num")->value().toInt();
        String params = "num=" + String(relayNum);
        logRequest("GET", "/relay/off", params);
        
        String cmd = "{\"cmd\":\"relay_off\",\"relay\":" + String(relayNum) + "}";
        JsonDocument response;
        _commandHandler.processCommand((uint8_t*)cmd.c_str(), cmd.length(), response);
        
        logResponse(response);
        
        String jsonResponse;
        serializeJson(response, jsonResponse);
        request->send(200, "application/json", jsonResponse);
    });
    
    _server.on("/relay/toggle", HTTP_GET, [this](AsyncWebServerRequest *request){
        if (!request->hasParam("num")) {
            request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing num parameter\"}");
            return;
        }
        int relayNum = request->getParam("num")->value().toInt();
        String params = "num=" + String(relayNum);
        logRequest("GET", "/relay/toggle", params);
        
        String cmd = "{\"cmd\":\"relay_toggle\",\"relay\":" + String(relayNum) + "}";
        JsonDocument response;
        _commandHandler.processCommand((uint8_t*)cmd.c_str(), cmd.length(), response);
        
        logResponse(response);
        
        String jsonResponse;
        serializeJson(response, jsonResponse);
        request->send(200, "application/json", jsonResponse);
    });
    
    _server.on("/relay/status", HTTP_GET, [this](AsyncWebServerRequest *request){
        if (!request->hasParam("num")) {
            request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing num parameter\"}");
            return;
        }
        int relayNum = request->getParam("num")->value().toInt();
        String params = "num=" + String(relayNum);
        logRequest("GET", "/relay/status", params);
        
        String cmd = "{\"cmd\":\"relay_status\",\"relay\":" + String(relayNum) + "}";
        JsonDocument response;
        _commandHandler.processCommand((uint8_t*)cmd.c_str(), cmd.length(), response);
        
        logResponse(response);
        
        String jsonResponse;
        serializeJson(response, jsonResponse);
        request->send(200, "application/json", jsonResponse);
    });
    
    _server.on("/relays/on", HTTP_GET, [this](AsyncWebServerRequest *request){
        handleAPIRequest(request, "/relays/on", "{\"cmd\":\"all_relays_on\"}");
    });
    
    _server.on("/relays/off", HTTP_GET, [this](AsyncWebServerRequest *request){
        handleAPIRequest(request, "/relays/off", "{\"cmd\":\"all_relays_off\"}");
    });
    
    _server.on("/relays/status", HTTP_GET, [this](AsyncWebServerRequest *request){
        handleAPIRequest(request, "/relays/status", "{\"cmd\":\"all_relay_status\"}");
    });
    
    // Sensor endpoints
    _server.on("/sensor", HTTP_GET, [this](AsyncWebServerRequest *request){
        if (!request->hasParam("num")) {
            request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing num parameter\"}");
            return;
        }
        int sensorNum = request->getParam("num")->value().toInt();
        String params = "num=" + String(sensorNum);
        logRequest("GET", "/sensor", params);
        
        String cmd = "{\"cmd\":\"sensor_status\",\"sensor\":" + String(sensorNum) + "}";
        JsonDocument response;
        _commandHandler.processCommand((uint8_t*)cmd.c_str(), cmd.length(), response);
        
        logResponse(response);
        
        String jsonResponse;
        serializeJson(response, jsonResponse);
        request->send(200, "application/json", jsonResponse);
    });
    
    _server.on("/sensors", HTTP_GET, [this](AsyncWebServerRequest *request){
        handleAPIRequest(request, "/sensors", "{\"cmd\":\"all_sensor_status\"}");
    });
    
    _server.on("/status", HTTP_GET, [this](AsyncWebServerRequest *request){
        handleAPIRequest(request, "/status", "{\"cmd\":\"all_status\"}");
    });
}
