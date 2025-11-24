#include "espnow_stub.h"
#include <time.h>
#include <unistd.h>

// Global state
static esp_now_recv_cb_t recv_callback = NULL;
static esp_now_send_cb_t send_callback = NULL;
static bool initialized = false;
static uint8_t peer_addr[6] = {0};
static bool has_peer = false;

// Simulated relay states
static bool relay_states[16] = {false};

esp_err_t esp_now_init(void) {
    printf("[ESP-NOW STUB] Initializing...\n");
    initialized = true;
    
    // Initialize random seed for simulation
    srand(time(NULL));
    
    return ESP_OK;
}

esp_err_t esp_now_deinit(void) {
    printf("[ESP-NOW STUB] Deinitializing...\n");
    initialized = false;
    recv_callback = NULL;
    send_callback = NULL;
    return ESP_OK;
}

esp_err_t esp_now_register_recv_cb(esp_now_recv_cb_t cb) {
    printf("[ESP-NOW STUB] Receive callback registered\n");
    recv_callback = cb;
    return ESP_OK;
}

esp_err_t esp_now_register_send_cb(esp_now_send_cb_t cb) {
    printf("[ESP-NOW STUB] Send callback registered\n");
    send_callback = cb;
    return ESP_OK;
}

esp_err_t esp_now_add_peer(const esp_now_peer_info_t *peer) {
    if (!peer) return ESP_FAIL;
    
    memcpy(peer_addr, peer->peer_addr, 6);
    has_peer = true;
    
    printf("[ESP-NOW STUB] Peer added: %02X:%02X:%02X:%02X:%02X:%02X\n",
           peer_addr[0], peer_addr[1], peer_addr[2],
           peer_addr[3], peer_addr[4], peer_addr[5]);
    
    return ESP_OK;
}

bool esp_now_is_peer_exist(const uint8_t *peer_addr_check) {
    return has_peer;
}

esp_err_t esp_now_send(const uint8_t *peer_addr_send, const uint8_t *data, size_t len) {
    if (!initialized || !has_peer) {
        printf("[ESP-NOW STUB] ✗ Not initialized or no peer\n");
        return ESP_FAIL;
    }
    
    printf("[ESP-NOW STUB] Sending %zu bytes: %.*s\n", len, (int)len, data);
    
    // Trigger send callback
    if (send_callback) {
        send_callback(peer_addr, ESP_NOW_SEND_SUCCESS);
    }
    
    // Parse command and simulate response
    char cmd_buffer[256];
    if (len < sizeof(cmd_buffer)) {
        memcpy(cmd_buffer, data, len);
        cmd_buffer[len] = '\0';
        
        // Simple JSON parsing
        if (strstr(cmd_buffer, "relay_toggle")) {
            // Extract relay number
            char *relay_str = strstr(cmd_buffer, "\"relay\":");
            if (relay_str) {
                int relay_num = atoi(relay_str + 8);
                if (relay_num >= 1 && relay_num <= 16) {
                    relay_states[relay_num - 1] = !relay_states[relay_num - 1];
                    espnow_stub_simulate_relay_response(relay_num, relay_states[relay_num - 1]);
                }
            }
        }
        else if (strstr(cmd_buffer, "all_relays_off")) {
            printf("[ESP-NOW STUB] All relays OFF\n");
            for (int i = 0; i < 16; i++) {
                relay_states[i] = false;
            }
            espnow_stub_simulate_all_status();
        }
        else if (strstr(cmd_buffer, "all_status")) {
            espnow_stub_simulate_all_status();
        }
    }
    
    return ESP_OK;
}

void espnow_stub_simulate_relay_response(uint8_t relayNum, bool state) {
    if (!recv_callback) return;
    
    char response[256];
    snprintf(response, sizeof(response),
             "{\"status\":\"ok\",\"data\":{\"relay\":%d,\"state\":\"%s\"}}",
             relayNum, state ? "on" : "off");
    
    printf("[ESP-NOW STUB] Simulating response: %s\n", response);
    
    // Simulate small delay
    usleep(50000); // 50ms
    
    // Trigger receive callback
    recv_callback(peer_addr, (const uint8_t*)response, strlen(response));
}

void espnow_stub_simulate_all_status(void) {
    if (!recv_callback) return;
    
    // Build relay states array
    char relay_array[64] = "[";
    for (int i = 0; i < 16; i++) {
        char buf[8];
        snprintf(buf, sizeof(buf), "%d%s", relay_states[i] ? 1 : 0, i < 15 ? "," : "");
        strcat(relay_array, buf);
    }
    strcat(relay_array, "]");
    
    // Simulate sensor data (random levels for demo)
    int sensor1 = 60 + (rand() % 40); // 60-100%
    int sensor2 = 30 + (rand() % 50); // 30-80%
    int sensor3 = 10 + (rand() % 40); // 10-50%
    
    char response[512];
    snprintf(response, sizeof(response),
             "{\"status\":\"ok\",\"data\":{\"relays\":%s,\"sensors\":["
             "{\"id\":1,\"resistance\":45.2,\"level\":%d},"
             "{\"id\":2,\"resistance\":120.5,\"level\":%d},"
             "{\"id\":3,\"resistance\":200.0,\"level\":%d}"
             "]}}",
             relay_array, sensor1, sensor2, sensor3);
    
    printf("[ESP-NOW STUB] Simulating all status response\n");
    printf("[ESP-NOW STUB] Sensor levels: Clean=%d%%, Gray=%d%%, Black=%d%%\n",
           sensor1, sensor2, sensor3);
    
    // Simulate small delay
    usleep(100000); // 100ms
    
    // Trigger receive callback
    recv_callback(peer_addr, (const uint8_t*)response, strlen(response));
}
