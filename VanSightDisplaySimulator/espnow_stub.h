#ifndef ESPNOW_STUB_H
#define ESPNOW_STUB_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

// ESP-NOW stub types and constants
typedef enum {
    ESP_NOW_SEND_SUCCESS = 0,
    ESP_NOW_SEND_FAIL
} esp_now_send_status_t;

typedef int esp_err_t;
#define ESP_OK 0
#define ESP_FAIL -1

// Peer info structure
typedef struct {
    uint8_t peer_addr[6];
    uint8_t channel;
    bool encrypt;
} esp_now_peer_info_t;

// Callback types
typedef void (*esp_now_recv_cb_t)(const uint8_t *mac_addr, const uint8_t *data, int data_len);
typedef void (*esp_now_send_cb_t)(const uint8_t *mac_addr, esp_now_send_status_t status);

#ifdef __cplusplus
extern "C" {
#endif

// ESP-NOW API stubs
esp_err_t esp_now_init(void);
esp_err_t esp_now_deinit(void);
esp_err_t esp_now_register_recv_cb(esp_now_recv_cb_t cb);
esp_err_t esp_now_register_send_cb(esp_now_send_cb_t cb);
esp_err_t esp_now_add_peer(const esp_now_peer_info_t *peer);
esp_err_t esp_now_send(const uint8_t *peer_addr, const uint8_t *data, size_t len);
bool esp_now_is_peer_exist(const uint8_t *peer_addr);

// Simulation functions
void espnow_stub_simulate_relay_response(uint8_t relayNum, bool state);
void espnow_stub_simulate_all_status(void);

#ifdef __cplusplus
}
#endif

#endif // ESPNOW_STUB_H
