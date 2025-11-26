#ifndef VANSIGHT_CONFIG_H
#define VANSIGHT_CONFIG_H

namespace VanSight
{
    // Communication Configuration
    constexpr int MAX_PAYLOAD_SIZE = 250; // Maximum BLE payload size
    constexpr int CONNECTION_TIMEOUT_MS = 5000; // Connection timeout
    constexpr int SCAN_DURATION_SEC = 5; // BLE scan duration
    constexpr int RECONNECT_DELAY_MS = 1000; // Delay before reconnect attempt

    // BLE Scan Configuration
    constexpr int SCAN_INTERVAL = 1349; // BLE scan interval
    constexpr int SCAN_WINDOW = 449; // BLE scan window

    // Protocol Configuration
    constexpr int MAX_RELAYS = 16; // Maximum number of relays
    constexpr int MAX_SENSORS = 3; // Maximum number of sensors

    // Retry Configuration
    constexpr int MAX_RETRY_COUNT = 3; // Maximum command retry count
    constexpr int RETRY_DELAY_MS = 500; // Delay between retries

    constexpr uint8_t HUB_MAC_ADDRESS[6] = {0x08, 0xB6, 0x1F, 0xBE, 0x12, 0x24}; // Hub AP MAC Address  // WiFi AP MAC
    constexpr uint8_t HMI_MAC_ADDRESS[6] = {0x94, 0xA9, 0x90, 0x03, 0xDE, 0x24};  // Hub AP MAC Address  // WiFi AP MAC

} // namespace VanSight

#endif // VANSIGHT_CONFIG_H
