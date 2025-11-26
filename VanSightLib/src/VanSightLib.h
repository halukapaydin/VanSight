#ifndef VANSIGHT_LIB_H
#define VANSIGHT_LIB_H

/**
 * VanSightLib - Shared library for VanSight IoT System
 * 
 * This library provides common functionality for VanSight devices:
 * - BLE communication (Server and Client)
 * - Protocol handling (Command parsing and Response building)
 * - Configuration constants and UUIDs
 * 
 * Version: 1.0.0
 * Author: VanSight Team
 */

// Configuration
#include "config/VanSightConfig.h"
#include "config/VanSightUUIDs.h"

// Protocol
#include "protocol/VanSightProtocol.h"
#include "protocol/CommandParser.h"
#include "protocol/ResponseBuilder.h"

// Communication
#include "communication/ESPNowManager.h"
#include "communication/CommandManager.h"
#include "communication/BleManager.h"
#include "communication/BleCommandManager.h"

#endif // VANSIGHT_LIB_H
