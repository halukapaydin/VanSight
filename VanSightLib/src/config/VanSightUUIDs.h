#ifndef VANSIGHT_UUIDS_H
#define VANSIGHT_UUIDS_H

// VanSight BLE Service and Characteristic UUIDs
// These UUIDs must match across all VanSight devices

namespace VanSight {

// Main Service UUID
constexpr const char* SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";

// Command Characteristic (Client writes commands to Server)
constexpr const char* COMMAND_CHAR_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8";

// Response Characteristic (Server notifies Client with responses)
constexpr const char* RESPONSE_CHAR_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a9";

} // namespace VanSight

#endif // VANSIGHT_UUIDS_H
