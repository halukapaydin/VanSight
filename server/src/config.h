#ifndef CONFIG_H
#define CONFIG_H
// ============================================================================
// PROTOCOL CONSTANTS
// ============================================================================
const int MAX_RELAYS = 16;
const int MAX_SENSORS = 3;


const char* WIFI_SSID = "VanSight";
const char* WIFI_PASS = "password123";

// #define SLAVE_ADDRESS 0x08
#define I2C_HUB_ADDR 0x20
#define I2C_SERVER_ADDR 0x10

#define SDA_PIN 21
#define SCL_PIN 22

#endif // CONFIG_H
