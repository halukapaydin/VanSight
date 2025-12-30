//
// Created by haluk on 12/29/25.
//

#include "BleCommandHandler.h"

#include "protocol/protocol.h"
#include "json/JsonCodec.h"

void BleCommandHandler::setDataReceivedCallback(DataReceivedCallback callback)
{
    this->_ble_client_manager.setDataReceivedCallback(callback);
}

void BleCommandHandler::setClientStateCallback(ClientStateCallback callback)
{
    this->_ble_client_manager.setClientStateCallback(callback);
}


void BleCommandHandler::reconnect()
{
    if (!this->_ble_client_manager.isConnected())
    {
        this->_ble_client_manager.begin();
    }
}

void BleCommandHandler::toggleRelay(int index)
{
    reconnect();
    Command command;
    command.type = CommandType::RELAY_TOGGLE;
    command.parameter.item_number = index;
    this->_ble_client_manager.sendData(JsonCodec::instance().serializeCommand(command));
}
void BleCommandHandler::allRelaysOff()
{
    reconnect();
    Command command;
    command.type = CommandType::ALL_RELAYS_OFF;
    this->_ble_client_manager.sendData(JsonCodec::instance().serializeCommand(command));
}

void BleCommandHandler::allStatus()
{
    reconnect();
    Command command;
    command.type = CommandType::ALL_STATUS;
    this->_ble_client_manager.sendData(JsonCodec::instance().serializeCommand(command));
}
bool BleCommandHandler::begin()
{
    return this->_ble_client_manager.begin();
}