#ifndef BLE_CONFIG_H
#define BLE_CONFIG_H

// BLE Service ve Characteristic UUIDs
#define BLE_SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define BLE_CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// BLE Device Names
#define BLE_SERVER_NAME "VanSight_Server"
#define BLE_CLIENT_NAME "VanSight_Client"

// Connection Parameters
#define BLE_SCAN_DURATION       5    // Scan duration in seconds
#define BLE_RECONNECT_INTERVAL  3000 // Reconnect attempt interval in milliseconds
#define BLE_CONNECTION_TIMEOUT  10000 // Connection timeout in milliseconds

// Data Buffer
#define BLE_MAX_DATA_LENGTH     512  // Maximum data length for string transfer

#endif // BLE_CONFIG_H
