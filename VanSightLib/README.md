# VanSightLib

Shared library for VanSight IoT system providing BLE communication and protocol handling.

## Features

- **BLE Communication**: Ready-to-use BLE Server and Client classes
- **Protocol Handling**: Command parsing and response building
- **Type Safety**: Strongly typed protocol definitions
- **Easy Integration**: Simple API for VanSight devices

## Installation

### PlatformIO

Add to your `platformio.ini`:

```ini
lib_deps =
    VanSightLib
```

### Arduino IDE

1. Download this library
2. Place in Arduino libraries folder
3. Restart Arduino IDE

## Usage

### BLE Server (Hub)

```cpp
#include <VanSightLib.h>

VanSightBLEServer server;

void setup() {
    server.begin("VanSightHub");
    server.onCommand([](const VanSightCommand& cmd) {
        // Handle command
        VanSightResponse response;
        response.status = "ok";
        return response;
    });
}

void loop() {
    server.update();
}
```

### BLE Client (Display)

```cpp
#include <VanSightLib.h>

VanSightBLEClient client;

void setup() {
    client.begin();
    client.onResponse([](const VanSightResponse& response) {
        // Handle response
    });
}

void loop() {
    client.update();
    
    // Send command
    VanSightCommand cmd;
    cmd.type = CMD_RELAY_TOGGLE;
    cmd.relay = 1;
    client.sendCommand(cmd);
}
```

## API Reference

See [API Documentation](docs/API.md) for detailed reference.

## License

MIT License - see LICENSE file for details.
