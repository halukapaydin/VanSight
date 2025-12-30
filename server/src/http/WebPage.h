#ifndef WEBPAGE_H
#define WEBPAGE_H

const char HTML_PAGE[] PROGMEM = R"rawliteral(

<!DOCTYPE html>
<html lang="tr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
    <title>VanSight Smart Control</title>
    <style>
        :root {
            --bg-color: #f2f2f7;
            --card-bg: #ffffff;
            --accent-blue: #007aff;
            --status-green: #34c759;
            --text-main: #1c1c1e;
            --text-sub: #8e8e93;
        }

        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
            -webkit-tap-highlight-color: transparent;
        }

        body {
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
            background-color: var(--bg-color);
            color: var(--text-main);
            padding: 20px 16px;
            line-height: 1.4;
        }

        header {
            margin-bottom: 24px;
            display: flex;
            justify-content: space-between;
            align-items: center;
        }

        .title-area h1 {
            font-size: 24px;
            font-weight: 700;
            letter-spacing: -0.5px;
        }

        .status-badge {
            font-size: 12px;
            padding: 4px 10px;
            border-radius: 12px;
            background: #e5e5ea;
            color: var(--text-sub);
            display: flex;
            align-items: center;
            gap: 5px;
        }

        .status-badge.online {
            color: var(--status-green);
            background: #e8f9ee;
        }

        .status-dot {
            width: 8px;
            height: 8px;
            border-radius: 50%;
            background: currentColor;
        }

        /* Su Seviye Sensörleri Bölümü */
        .section-title {
            font-size: 17px;
            font-weight: 600;
            margin-bottom: 12px;
            color: var(--text-sub);
            padding-left: 4px;
        }

        .sensor-grid {
            display: grid;
            grid-template-columns: repeat(3, 1fr);
            gap: 12px;
            margin-bottom: 30px;
        }

        .sensor-card {
            background: var(--card-bg);
            padding: 15px 10px;
            border-radius: 16px;
            text-align: center;
            box-shadow: 0 2px 8px rgba(0,0,0,0.04);
        }

        .sensor-val {
            display: block;
            font-size: 20px;
            font-weight: 700;
            color: var(--accent-blue);
        }

        .sensor-label {
            font-size: 11px;
            color: var(--text-sub);
            text-transform: uppercase;
            font-weight: 600;
            margin-top: 4px;
        }

        /* Röle Butonları Bölümü */
        .relay-grid {
            display: grid;
            grid-template-columns: repeat(3, 1fr);
            gap: 16px;
        }

        .relay-card {
            background: var(--card-bg);
            border-radius: 20px;
            padding: 16px;
            display: flex;
            flex-direction: column;
            justify-content: space-between;
            aspect-ratio: 1 / 1;
            transition: all 0.2s ease;
            border: none;
            cursor: pointer;
            box-shadow: 0 4px 12px rgba(0,0,0,0.05);
        }

        .relay-card.active {
            background-color: var(--accent-blue);
        }

        .icon-box {
            width: 40px;
            height: 40px;
            border-radius: 12px;
            background: #f2f2f7;
            display: flex;
            align-items: center;
            justify-content: center;
            margin-bottom: 12px;
            transition: 0.2s;
        }

        .relay-card.active .icon-box {
            background: rgba(255,255,255,0.2);
        }

        .icon-box img {
            width: 24px;
            height: 24px;
            filter: grayscale(1) brightness(0.5);
        }

        .relay-card.active img {
            filter: brightness(0) invert(1);
        }

        .relay-info {
            text-align: left;
        }

        .relay-name {
            font-size: 15px;
            font-weight: 600;
            display: block;
        }

        .relay-status {
            font-size: 13px;
            color: var(--text-sub);
        }

        .relay-card.active .relay-name,
        .relay-card.active .relay-status {
            color: white;
        }

        footer {
            margin-top: 30px;
            display: flex;
            gap: 10px;
        }

        .action-btn {
            flex: 1;
            border: none;
            padding: 14px;
            border-radius: 12px;
            font-weight: 600;
            font-size: 14px;
            background: #e5e5ea;
            color: var(--text-main);
        }

        .action-btn.danger {
            background: #ffe5e5;
            color: #ff3b30;
        }
    </style>
</head>
<body>

<header>
    <div class="title-area">
        <h1>VanSight</h1>
        <p style="font-size: 14px; color: var(--text-sub);">Akıllı Karavan Sistemi</p>
    </div>
    <div id="connectionStatus" class="status-badge">
        <div class="status-dot"></div>
        <span id="statusText">Bağlanıyor</span>
    </div>
</header>

<div class="section-title">SU SEVİYELERİ</div>
<div class="sensor-grid">
    <div class="sensor-card">
        <span class="sensor-val" id="level0">0%</span>
        <span class="sensor-label">Temiz</span>
    </div>
    <div class="sensor-card">
        <span class="sensor-val" id="level1">0%</span>
        <span class="sensor-label">Gri</span>
    </div>
    <div class="sensor-card">
        <span class="sensor-val" id="level2">0%</span>
        <span class="sensor-label">Siyah</span>
    </div>
</div>

<div class="section-title">KONTROLLER</div>
<div class="relay-grid" id="relayGrid">

</div>

<!--const btn = document.createElement('button');-->
<!--btn.className = 'relay-card';-->
<!--btn.id = `relay-${i}`;-->
<!--btn.onclick = () => toggleRelay(i);-->
<!--btn.innerHTML = `-->
<!--<div class="icon-box"><img src="${relay.icon}"></div>-->
<!--<div class="relay-info">-->
<!--    <span class="relay-name">${relay.name}</span>-->
<!--    <span class="relay-status" id="stat-${i}">Kapalı</span>-->
<!--</div>-->
<!--`;-->
<!--grid.appendChild(btn);-->

<script>
    // İkonlar (Base64 SVG)
    const icons = {
        koridor: "data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHdpZHRoPSIyNCIgaGVpZ2h0PSIyNCIgdmlld0JveD0iMCAwIDI0IDI0IiBmaWxsPSJub25lIiBzdHJva2U9ImN1cnJlbnRDb2xvciIgc3Ryb2tlLXdpZHRoPSIyIiBzdHJva2UtbGluZWNhcD0icm91bmQiIHN0cm9rZS1saW5lam9pbj0icm91bmQiPjxwYXRoIGQ9Ik0zIDIyaDE4TTE4IDE4VjZMMTIgMkw2IDZ2MTJNOCAxOGg4Ii8+PC9zdmc+",
        koltuk: "data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHdpZHRoPSIyNCIgaGVpZ2h0PSIyNCIgdmlld0JveD0iMCAwIDI0IDI0IiBmaWxsPSJub25lIiBzdHJva2U9ImN1cnJlbnRDb2xvciIgc3Ryb2tlLXdpZHRoPSIyIiBzdHJva2UtbGluZWNhcD0icm91bmQiIHN0cm9rZS1saW5lam9pbj0icm91bmQiPjxwYXRoIGQ9Ik00IDE4djNoMTZ2LTNNNCAxMWgtMVY3aDF2NE0yMCAxMWgxVjdoLTF2NE00IDExdi00YTIgMiAwIDAgMSAyLTJoMTJhMiAyIDAgMCAxIDIgMnY0TTQgMTFoMTZ2N0g0di03WiIvPjwvc3ZnPg==",
        mutfak: "data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHdpZHRoPSIyNCIgaGVpZ2h0PSIyNCIgdmlld0JveD0iMCAwIDI0IDI0IiBmaWxsPSJub25lIiBzdHJva2U9ImN1cnJlbnRDb2xvciIgc3Ryb2tlLXdpZHRoPSIyIiBzdHJva2UtbGluZWNhcD0icm91bmQiIHN0cm9rZS1saW5lam9pbj0icm91bmQiPjxwYXRoIGQ9Ik0xOCAzdi0zTTE0IDN2LTNNMTAgM3YtM00yMCA4YTIgMiAwIDAgMS0yIDJIMmE0IDQgMCAwIDEgMC04aDE2YTIgMiAwIDAgMSAyIDJ2NlpNMiAxNGgxOGwyIDZIMmwtMi02WiIvPjwvc3ZnPg==",
        wc: "data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHdpZHRoPSIyNCIgaGVpZ2h0PSIyNCIgdmlld0JveD0iMCAwIDI0IDI0IiBmaWxsPSJub25lIiBzdHJva2U9ImN1cnJlbnRDb2xvciIgc3Ryb2tlLXdpZHRoPSIyIiBzdHJva2UtbGluZWNhcD0icm91bmQiIHN0cm9rZS1saW5lam9pbj0icm91bmQiPjxwYXRoIGQ9Ik03IDEyaDEwTTkgMjJoNmEyIDIgMCAwIDAgMi0ydi00Sjd2NGEyIDIgMCAwIDAgMiAyaFpNNSAxMWgxNHYySDVWMTFabTItN2gxMHY0SDdWNFoiLz48L3N2Zz4=",
        banyo: "data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHdpZHRoPSIyNCIgaGVpZ2h0PSIyNCIgdmlld0JveD0iMCAwIDI0IDI0IiBmaWxsPSJub25lIiBzdHJva2U9ImN1cnJlbnRDb2xvciIgc3Ryb2tlLXdpZHRoPSIyIiBzdHJva2UtbGluZWNhcD0icm91bmQiIHN0cm9rZS1saW5lam9pbj0icm91bmQiPjxwYXRoIGQ9Ik03IDExdi00YTUgNSAwIDAgMSAxMCAwdjRNMiAxMWgyME00IDExdjhhMiAyIDAgMCAwIDIgMmgxMmEyIDIgMCAwIDAgMi0ydi04SDRaIi8+PC9zdmc+",
        yatak: "data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHdpZHRoPSIyNCIgaGVpZ2h0PSIyNCIgdmlld0JveD0iMCAwIDI0IDI0IiBmaWxsPSJub25lIiBzdHJva2U9ImN1cnJlbnRDb2xvciIgc3Ryb2tlLXdpZHRoPSIyIiBzdHJva2UtbGluZWNhcD0icm91bmQiIHN0cm9rZS1saW5lam9pbj0icm91bmQiPjxwYXRoIGQ9Ik0yIDIydjI0TTIgMTBoMjBNMiA3djEzYTIgMiAwIDAgMCAyIDJoMTZhMiAyIDAgMCAwIDItM1Y3TTIgMTB2MmgyMHYtMmwtNC0zSDRsLTQgM1oiLz48L3N2Zz4=",
        balkon: "data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHdpZHRoPSIyNCIgaGVpZ2h0PSIyNCIgdmlld0JveD0iMCAwIDI0IDI0IiBmaWxsPSJub25lIiBzdHJva2U9ImN1cnJlbnRDb2xvciIgc3Ryb2tlLXdpZHRoPSIyIiBzdHJva2UtbGluZWNhcD0icm91bmQiIHN0cm9rZS1saW5lam9pbj0icm91bmQiPjxwYXRoIGQ9Ik0yIDIwaDIwTTQgMjB2LTZtMTYgNnYtNk0yIDE0aDIwTTggMTR2LTZNMTIgMTR2LTYiLz48L3N2Zz4=",
        projektor: "data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHdpZHRoPSIyNCIgaGVpZ2h0PSIyNCIgdmlld0JveD0iMCAwIDI0IDI0IiBmaWxsPSJub25lIiBzdHJva2U9ImN1cnJlbnRDb2xvciIgc3Ryb2tlLXdpZHRoPSIyIiBzdHJva2UtbGluZWNhcD0icm91bmQiIHN0cm9rZS1saW5lam9pbj0icm91bmQiPjxwYXRoIGQ9Ik0xMiAxM2E1IDUgMCAxIDAgMC0xMDUgNSAwIDAgMCAwIDEweiBNMTIgMTV2N005IDIyaDZNMiAxMWg0TTE4IDExaDRNNi4zIDYuM2wtMi44LTIuOE0yMC41IDMuNWwtMi44IDIuOCIvPjwvc3ZnPg=="
    };

    const relayNames = [
        {name: "Koridor", icon: icons.koridor},
        {name: "Sağ Koltuk", icon: icons.koltuk},
        {name: "Sol Koltuk", icon: icons.koltuk},
        {name: "Mutfak", icon: icons.mutfak},
        {name: "WC", icon: icons.wc},
        {name: "Banyo", icon: icons.banyo},
        {name: "Yatak", icon: icons.yatak},
        {name: "Balkon Üst", icon: icons.balkon},
        {name: "Balkon Alt", icon: icons.balkon},
        {name: "Projektör", icon: icons.projektor}
    ];

    let ws;

    function initWebSocket() {
        ws = new WebSocket(`ws://${window.location.hostname}/ws`);

        ws.onopen = () => {
            document.getElementById('connectionStatus').classList.add('online');
            document.getElementById('statusText').textContent = 'Bağlı';
            sendCommand('ALL_STATUS');
        };

        ws.onclose = () => {
            document.getElementById('connectionStatus').classList.remove('online');
            document.getElementById('statusText').textContent = 'Kesildi';
            setTimeout(initWebSocket, 3000);
        };

        ws.onmessage = (event) => {
            const data = JSON.parse(event.data);
            if(data.relays) updateRelayUI(data.relays);
            if(data.levels) updateLevelUI(data.levels);
        };
    }

    function createRelays() {
        const grid = document.getElementById('relayGrid');
        relayNames.forEach((relay, i) => {
            const btn = document.createElement('button');
            btn.className = 'relay-card';
            btn.id = `relay-${i}`;
            btn.onclick = () => toggleRelay(i);
            btn.innerHTML = `
                    <div class="icon-box"><img src="${relay.icon}"></div>
                    <div class="relay-info">
                        <span class="relay-name">${relay.name}</span>
                        <span class="relay-status" id="stat-${i}">Kapalı</span>
                    </div>
                `;
            grid.appendChild(btn);



        });
        let btn = document.createElement('button');
        btn.className = 'relay-card';
        btn.id = `btn-all-switch-off`;
        btn.onclick = () => sendCommand('ALL_RELAYS_OFF');
        btn.innerHTML = `
                        <div class="icon-box">
                            <svg width="20" height="20" xmlns='http://www.w3.org/2000/svg' viewBox='0 0 640 640'><!--!Font Awesome Free v7.1.0 by @fontawesome - https://fontawesome.com License - https://fontawesome.com/license/free Copyright 2025 Fonticons, Inc.--><path d='M352 64C352 46.3 337.7 32 320 32C302.3 32 288 46.3 288 64L288 320C288 337.7 302.3 352 320 352C337.7 352 352 337.7 352 320L352 64zM210.3 162.4C224.8 152.3 228.3 132.3 218.2 117.8C208.1 103.3 188.1 99.8 173.6 109.9C107.4 156.1 64 233 64 320C64 461.4 178.6 576 320 576C461.4 576 576 461.4 576 320C576 233 532.6 156.1 466.3 109.9C451.8 99.8 431.9 103.3 421.7 117.8C411.5 132.3 415.1 152.2 429.6 162.4C479.4 197.2 511.9 254.8 511.9 320C511.9 426 425.9 512 319.9 512C213.9 512 128 426 128 320C128 254.8 160.5 197.1 210.3 162.4z'/></svg>
                        </div>
                        <div class="relay-info">
                            <span class="relay-name">Tümünü Kapat</span>
                            <span class="relay-status"></span>
                        </div>
                `;
        grid.appendChild(btn);

        btn = document.createElement('button');
        btn.className = 'relay-card';
        btn.id = `btn-all-status`;
        btn.onclick = () => sendCommand('ALL_STATUS');
        btn.innerHTML = `
                        <div class="icon-box">
                            <svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 640 640"><!--!Font Awesome Free v7.1.0 by @fontawesome - https://fontawesome.com License - https://fontawesome.com/license/free Copyright 2025 Fonticons, Inc.--><path d="M150.6 105.4C138.1 92.9 117.8 92.9 105.3 105.4L41.3 169.4C32.1 178.6 29.4 192.3 34.4 204.3C39.4 216.3 51.1 224 64 224L96 224L96 448C96 501 139 544 192 544L320 544C337.7 544 352 529.7 352 512C352 494.3 337.7 480 320 480L192 480C174.3 480 160 465.7 160 448L160 224L192 224C204.9 224 216.6 216.2 221.6 204.2C226.6 192.2 223.8 178.5 214.7 169.3L150.7 105.3zM489.4 534.6C501.9 547.1 522.2 547.1 534.7 534.6L598.7 470.6C607.9 461.4 610.6 447.7 605.6 435.7C600.6 423.7 588.9 416 576 416L544 416L544 192C544 139 501 96 448 96L320 96C302.3 96 288 110.3 288 128C288 145.7 302.3 160 320 160L448 160C465.7 160 480 174.3 480 192L480 416L448 416C435.1 416 423.4 423.8 418.4 435.8C413.4 447.8 416.2 461.5 425.3 470.7L489.3 534.7z"/></svg>
                        </div>
                        <div class="relay-info">
                            <span class="relay-name">Yenile</span>
                            <span class="relay-status"></span>
                        </div>
                `;
        grid.appendChild(btn);



    }

    function toggleRelay(index) {
        const command = { type: 'RELAY_TOGGLE', item: index };
        if (ws && ws.readyState === WebSocket.OPEN) ws.send(JSON.stringify(command));
    }

    function sendCommand(type) {
        const command = { type: type, item: -1 };
        if (ws && ws.readyState === WebSocket.OPEN) ws.send(JSON.stringify(command));
    }

    function updateRelayUI(states) {
        states.forEach((state, i) => {
            const card = document.getElementById(`relay-${i}`);
            const statText = document.getElementById(`stat-${i}`);
            if (card) {
                if (state) {
                    card.classList.add('active');
                    statText.textContent = 'Açık';
                } else {
                    card.classList.remove('active');
                    statText.textContent = 'Kapalı';
                }
            }
        });
    }

    function updateLevelUI(levels) {
        levels.forEach((val, i) => {
            const el = document.getElementById(`level${i}`);
            if (el) el.textContent = val + '%';
        });
    }

    window.onload = () => {
        createRelays();
        initWebSocket();
    };
</script>
</body>
</html>

)rawliteral";

#endif