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
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
    <title>VanSightHub</title>
    <link href="https://fonts.googleapis.com/css2?family=Inter:wght@300;400;600;700&display=swap" rel="stylesheet">
    <link rel="stylesheet" href="https://fonts.googleapis.com/css2?family=Material+Symbols+Rounded:opsz,wght,FILL,GRAD@24,400,1,0" />
    <style>
        :root {
            --bg-color: #0f172a;
            --card-bg: #1e293b;
            --text-primary: #f8fafc;
            --text-secondary: #94a3b8;
            --accent-color: #3b82f6;
            --success-color: #10b981;
            --danger-color: #ef4444;
            --relay-off: #334155;
            --relay-on: #3b82f6;
            --shadow: 0 4px 6px -1px rgba(0, 0, 0, 0.1), 0 2px 4px -1px rgba(0, 0, 0, 0.06);
        }
        * { margin: 0; padding: 0; box-sizing: border-box; -webkit-tap-highlight-color: transparent; }
        body {
            font-family: 'Inter', sans-serif;
            background-color: var(--bg-color);
            color: var(--text-primary);
            min-height: 100vh;
            padding: 20px 16px;
            padding-bottom: 40px;
        }
        .container { max-width: 480px; margin: 0 auto; }
        
        /* Material Symbols */
        .material-symbols-rounded {
            font-variation-settings: 'FILL' 1, 'wght' 400, 'GRAD' 0, 'opsz' 24;
            font-size: 24px;
        }
        
        /* Header */
        .header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 24px;
            padding: 0 4px;
        }
        h1 { font-size: 1.5rem; font-weight: 700; letter-spacing: -0.5px; background: linear-gradient(to right, #60a5fa, #a78bfa); -webkit-background-clip: text; -webkit-text-fill-color: transparent; }
        .status-badge {
            font-size: 0.75rem;
            padding: 6px 12px;
            border-radius: 9999px;
            background: rgba(16, 185, 129, 0.1);
            color: var(--success-color);
            font-weight: 600;
            display: flex;
            align-items: center;
            gap: 6px;
            transition: all 0.3s ease;
        }
        .status-badge.disconnected { background: rgba(239, 68, 68, 0.1); color: var(--danger-color); }
        .status-dot { width: 8px; height: 8px; border-radius: 50%; background-color: currentColor; }
        
        /* Cards */
        .card {
            background-color: var(--card-bg);
            border-radius: 24px;
            padding: 20px;
            margin-bottom: 20px;
            box-shadow: var(--shadow);
            border: 1px solid rgba(255,255,255,0.05);
        }
        .card-header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 16px;
        }
        .card-title { font-size: 1.1rem; font-weight: 600; color: var(--text-primary); display: flex; align-items: center; gap: 8px; }
        
        /* Relay Grid */
        .relay-grid {
            display: grid;
            grid-template-columns: repeat(4, 1fr); /* 4 columns as requested */
            gap: 8px; /* Reduced gap */
            margin-bottom: 20px;
        }
        .relay-btn {
            aspect-ratio: 1;
            border: none;
            border-radius: 12px; /* Slightly smaller radius */
            background-color: var(--relay-off);
            color: var(--text-secondary);
            cursor: pointer;
            transition: all 0.2s cubic-bezier(0.4, 0, 0.2, 1);
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            position: relative;
            overflow: hidden;
            padding: 4px; /* Reduced padding */
            gap: 4px; /* Reduced gap */
        }
        .relay-btn .icon { font-size: 24px; } /* Smaller icon */
        .relay-btn .label { 
            font-size: 0.65rem; /* Smaller text */
            font-weight: 600; 
            text-align: center; 
            line-height: 1.1;
            white-space: nowrap;
            overflow: hidden;
            text-overflow: ellipsis;
            width: 100%;
        }
        
        .relay-btn::after {
            content: '';
            position: absolute;
            inset: 0;
            background: rgba(255,255,255,0);
            transition: background 0.2s;
        }
        .relay-btn:active { transform: scale(0.95); }
        .relay-btn:active::after { background: rgba(255,255,255,0.1); }
        
        .relay-btn.on {
            background-color: var(--relay-on);
            color: white;
            box-shadow: 0 4px 12px rgba(59, 130, 246, 0.4);
        }
        .relay-btn.on .label { color: white; }
        
        /* Global Controls */
        .control-group { display: flex; gap: 12px; }
        .action-btn {
            flex: 1;
            padding: 14px;
            border: none;
            border-radius: 16px;
            font-weight: 600;
            font-size: 0.95rem;
            cursor: pointer;
            transition: transform 0.2s;
        }
        .action-btn:active { transform: scale(0.98); }
        .btn-primary { background: var(--accent-color); color: white; }
        .btn-danger { background: rgba(239, 68, 68, 0.15); color: var(--danger-color); }
        
        /* Sensors */
        .sensor-list { display: flex; flex-direction: column; gap: 16px; }
        .sensor-item {
            background: rgba(0,0,0,0.2);
            padding: 16px;
            border-radius: 16px;
            display: flex;
            align-items: center;
            gap: 16px;
        }
        .sensor-icon {
            width: 48px;
            height: 48px;
            border-radius: 12px;
            background: rgba(59, 130, 246, 0.1);
            color: var(--accent-color);
            display: flex;
            align-items: center;
            justify-content: center;
        }
        .sensor-icon span { font-size: 24px; }
        
        .sensor-content { flex: 1; }
        
        .sensor-info { display: flex; justify-content: space-between; margin-bottom: 8px; }
        .sensor-label { font-size: 0.95rem; font-weight: 600; color: var(--text-primary); }
        .sensor-value { font-weight: 700; color: var(--accent-color); }
        
        .progress-bg {
            height: 8px;
            background: rgba(255,255,255,0.1);
            border-radius: 4px;
            overflow: hidden;
        }
        .progress-fill {
            height: 100%;
            background: linear-gradient(90deg, #3b82f6, #8b5cf6);
            border-radius: 4px;
            width: 0%;
            transition: width 0.5s cubic-bezier(0.4, 0, 0.2, 1);
        }
        
        .refresh-btn {
            width: 100%;
            margin-top: 16px;
            background: rgba(255,255,255,0.05);
            color: var(--text-primary);
        }
        
        /* Loading Overlay */
        #loader {
            position: fixed; inset: 0; background: var(--bg-color);
            display: flex; justify-content: center; align-items: center;
            z-index: 1000; transition: opacity 0.3s;
        }
        .spinner { width: 40px; height: 40px; border: 3px solid rgba(255,255,255,0.1); border-top-color: var(--accent-color); border-radius: 50%; animation: spin 1s linear infinite; }
        @keyframes spin { to { transform: rotate(360deg); } }
        
    </style>
</head>
<body>
    <div id="loader"><div class="spinner"></div></div>

    <div class="container">
        <div class="header">
            <h1>VanSightHub</h1>
            <div class="status-badge" id="connectionStatus">
                <div class="status-dot"></div>
                <span>Bağlanıyor...</span>
            </div>
        </div>

        <div class="card">
            <div class="card-header">
                <div class="card-title">
                    <span class="material-symbols-rounded">electric_bolt</span>
                    Kontrol Paneli
                </div>
            </div>
            <div class="relay-grid" id="relayGrid">
                <!-- Generated by JS -->
            </div>
            <div class="control-group">
                <button class="action-btn btn-primary" onclick="allRelaysOn()">Tümünü Aç</button>
                <button class="action-btn btn-danger" onclick="allRelaysOff()">Tümünü Kapat</button>
            </div>
        </div>

        <div class="card">
            <div class="card-header">
                <div class="card-title">
                    <span class="material-symbols-rounded">water_drop</span>
                    Su Tankları
                </div>
            </div>
            <div class="sensor-list" id="sensorContainer">
                <!-- Generated by JS -->
            </div>
            <button class="action-btn refresh-btn" onclick="refreshSensors()">
                <span class="material-symbols-rounded" style="font-size: 18px; vertical-align: middle; margin-right: 4px;">refresh</span>
                Verileri Yenile
            </button>
        </div>
    </div>

    <script>
        const API_BASE = '';
        let relayStates = Array(16).fill(false);
        
        // Configuration for Relays
        const RELAY_CONFIG = {
            1: { label: 'Koridor', icon: 'light_group' },
            2: { label: 'Sağ Koltuk', icon: 'chair' },
            3: { label: 'Sol Koltuk', icon: 'chair_alt' },
            4: { label: 'Mutfak', icon: 'kitchen' },
            5: { label: 'Banyo', icon: 'bathtub' },
            6: { label: 'WC', icon: 'wc' },
            7: { label: 'Balkon Üst', icon: 'deck' },
            8: { label: 'Balkon Alt', icon: 'balcony' },
            9: { label: 'Projeksiyon', icon: 'videocam' }
        };
        
        // Configuration for Sensors
        const SENSOR_CONFIG = {
            1: { label: 'Temiz Su', icon: 'water_drop', color: '#3b82f6' },
            2: { label: 'Gri Su', icon: 'water_full', color: '#64748b' },
            3: { label: 'Pis Su', icon: 'delete', color: '#ef4444' }
        };
        
        window.onload = function() {
            initializeRelays();
            loadAllStatus();
            setInterval(refreshSensors, 5000);
            
            setTimeout(() => {
                document.getElementById('loader').style.opacity = '0';
                setTimeout(() => document.getElementById('loader').style.display = 'none', 300);
            }, 500);
        };

        function initializeRelays() {
            const grid = document.getElementById('relayGrid');
            grid.innerHTML = '';
            for (let i = 1; i <= 16; i++) {
                const config = RELAY_CONFIG[i] || { label: `Röle ${i}`, icon: 'lightbulb' };
                
                const btn = document.createElement('button');
                btn.className = 'relay-btn';
                btn.id = `relay-${i}`;
                btn.onclick = () => toggleRelay(i);
                
                btn.innerHTML = `
                    <span class="material-symbols-rounded icon">${config.icon}</span>
                    <span class="label">${config.label}</span>
                `;
                
                grid.appendChild(btn);
            }
        }

        async function toggleRelay(num) {
            const currentState = relayStates[num-1];
            updateRelayButton(num, !currentState);
            
            try {
                const response = await fetch(`${API_BASE}/relay/toggle?num=${num}`);
                const data = await response.json();
                if (data.status === 'ok') {
                    updateRelayButton(num, data.data.state === 'on');
                } else {
                    updateRelayButton(num, currentState);
                }
            } catch (error) {
                console.error('Hata:', error);
                updateRelayButton(num, currentState);
                updateConnectionStatus(false);
            }
        }

        async function allRelaysOn() {
            try {
                const response = await fetch(`${API_BASE}/relays/on`);
                const data = await response.json();
                if (data.status === 'ok') {
                    for (let i = 1; i <= 16; i++) updateRelayButton(i, true);
                }
            } catch (error) { console.error('Hata:', error); }
        }

        async function allRelaysOff() {
            try {
                const response = await fetch(`${API_BASE}/relays/off`);
                const data = await response.json();
                if (data.status === 'ok') {
                    for (let i = 1; i <= 16; i++) updateRelayButton(i, false);
                }
            } catch (error) { console.error('Hata:', error); }
        }

        function updateRelayButton(num, isOn) {
            const btn = document.getElementById(`relay-${num}`);
            if (isOn) btn.classList.add('on');
            else btn.classList.remove('on');
            relayStates[num-1] = isOn;
        }

        async function refreshSensors() {
            try {
                const response = await fetch(`${API_BASE}/sensors`);
                const data = await response.json();
                if (data.status === 'ok') {
                    displaySensors(data.data.sensors);
                    updateConnectionStatus(true);
                }
            } catch (error) {
                console.error('Hata:', error);
                updateConnectionStatus(false);
            }
        }

        function displaySensors(sensors) {
            const container = document.getElementById('sensorContainer');
            container.innerHTML = '';
            sensors.forEach(sensor => {
                const config = SENSOR_CONFIG[sensor.id] || { label: `Sensör ${sensor.id}`, icon: 'opacity', color: '#3b82f6' };
                
                const sensorDiv = document.createElement('div');
                sensorDiv.className = 'sensor-item';
                sensorDiv.innerHTML = `
                    <div class="sensor-icon" style="color: ${config.color}; background: ${config.color}20">
                        <span class="material-symbols-rounded">${config.icon}</span>
                    </div>
                    <div class="sensor-content">
                        <div class="sensor-info">
                            <span class="sensor-label">${config.label}</span>
                            <span class="sensor-value" style="color: ${config.color}">${sensor.level}%</span>
                        </div>
                        <div class="progress-bg">
                            <div class="progress-fill" style="width: ${sensor.level}%; background: ${config.color}"></div>
                        </div>
                    </div>
                `;
                container.appendChild(sensorDiv);
            });
        }

        async function loadAllStatus() {
            try {
                const response = await fetch(`${API_BASE}/status`);
                const data = await response.json();
                if (data.status === 'ok') {
                    data.data.relays.forEach((state, index) => {
                        updateRelayButton(index + 1, state === 1);
                    });
                    displaySensors(data.data.sensors);
                    updateConnectionStatus(true);
                }
            } catch (error) {
                console.error('Hata:', error);
                updateConnectionStatus(false);
            }
        }

        function updateConnectionStatus(isConnected) {
            const badge = document.getElementById('connectionStatus');
            const text = badge.querySelector('span');
            if (isConnected) {
                badge.classList.remove('disconnected');
                text.textContent = 'Bağlı';
            } else {
                badge.classList.add('disconnected');
                text.textContent = 'Bağlantı Yok';
            }
        }
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
