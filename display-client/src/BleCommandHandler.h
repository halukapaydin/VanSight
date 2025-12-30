//
// Created by haluk on 12/29/25.
//

#ifndef DISPLAY_CLIENT_BLECOMMANDHANDLER_H
#define DISPLAY_CLIENT_BLECOMMANDHANDLER_H
#include "ble/BleClientManager.h"


class BleCommandHandler
{
public:
    // Singleton erişim fonksiyonu
    static BleCommandHandler& instance() {
        static BleCommandHandler _instance;
        return _instance;
    }

    // Kopyalama / atama engelleniyor
    BleCommandHandler(const BleCommandHandler&) = delete;
    BleCommandHandler& operator=(const BleCommandHandler&) = delete;

    void setDataReceivedCallback(DataReceivedCallback callback);
    void setClientStateCallback(ClientStateCallback callback);
    void toggleRelay(int index);
    void allRelaysOff();
    void allStatus();
    bool begin();

private:
    BleCommandHandler() = default;
    ~BleCommandHandler() = default;

    BleClientManager _ble_client_manager;

    void reconnect();



};


#endif //DISPLAY_CLIENT_BLECOMMANDHANDLER_H
