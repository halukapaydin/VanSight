//
// Created by haluk on 11/25/25.
//


#include "config.h"
#include "HttpManager.h"
#include "JsonCodec.h"
#include "protocol.h"
#include "ble/BleServerManager.h"
#include "i2c/I2CManager.h"
#include "infrared/InfraredManager.h"
#include "uart/UartManager.h"

BleServerManager ble_server_manager;
HttpManager http_manager;
UartManager uart_manager(Serial2);
InfraredManager& ir = InfraredManager::getInstance();

void onDataReceivedOverBle(String data)
{
    uart_manager.sendData(data);
    Serial.println("ble to serial : " + data);
}

void onDataReceivedOverSerial(const String data)
{
    ble_server_manager.sendData(data);
    http_manager.sendData(data);
    Serial.println("serial to ble & http : " + data);
}

void onDataReceivedOverOverHttp(String data)
{
    uart_manager.sendData(data);
    Serial.println("http to serial : " + data);
}

void toggleRelay(uint8_t index)
{
    Serial.print("toggleRelay: ");
    Serial.println(index);
    Command command;
    command.type = CommandType::RELAY_TOGGLE;
    command.parameter.item_number = index;
    auto serialize_command = JsonCodec::instance().serializeCommand(command);
    uart_manager.sendData(serialize_command);
}
void allRelaysOff()
{
    Serial.print("allRelaysOff");
    Command command;
    command.type = CommandType::ALL_RELAYS_OFF;
    auto serialize_command = JsonCodec::instance().serializeCommand(command);
    uart_manager.sendData(serialize_command);
}

void onIrCommand(uint32_t code)
{
    switch (code)
    {
    case IR_BUTTON_NUMBER_1:
        toggleRelay(0);
        break;
    case IR_BUTTON_NUMBER_2:
        toggleRelay(1);
        break;
    case IR_BUTTON_NUMBER_3:
        toggleRelay(2);
        break;
    case IR_BUTTON_NUMBER_4:
        toggleRelay(3);
        break;
    case IR_BUTTON_NUMBER_5:
        toggleRelay(4);
        break;
    case IR_BUTTON_NUMBER_6:
        toggleRelay(5);
        break;
    case IR_BUTTON_NUMBER_7:
        toggleRelay(6);
        break;
    case IR_BUTTON_NUMBER_8:
        toggleRelay(7);
        break;
    case IR_BUTTON_NUMBER_9:
        toggleRelay(8);
        break;
    case IR_BUTTON_NUMBER_0:
        toggleRelay(9);
        break;
    case IR_BUTTON_STAR:
        toggleRelay(10);
        break;
    case IR_BUTTON_HASH:
        allRelaysOff();
        break;
    case IR_BUTTON_OK:
        toggleRelay(0);
        break;
    default: ;
    }
}


void setup()
{
    Serial.begin(115200);
    delay(1000); // Give serial some time to stabilize
    uart_manager.start(16, 17, 9600);
    Serial.println("ı2c manager initialized on pins 21(SDA), 22(SCL)");

    uart_manager.onDataReceived(onDataReceivedOverSerial);

    // WiFi starts BEFORE BLE for better coexistence
    if (http_manager.start(WIFI_SSID, WIFI_PASS))
    {
        Serial.println("HTTP Manager başlatıldı!");
        Serial.print("Web arayüzüne erişim: http://");
        Serial.println(http_manager.getIPAddress());
    }
    else
    {
        Serial.println("HTTP Manager başlatılamadı!");
        return;
    }

    // HTTP'den gelen verileri UART'a yönlendir
    http_manager.onDataReceived(onDataReceivedOverOverHttp);

    if (ble_server_manager.begin())
    {
        Serial.println("BLE Server initialized successfully!");
    }
    else
    {
        Serial.println("Failed to initialize BLE Server!");
        return;
    }
    // Set callback for received data
    ble_server_manager.setDataReceivedCallback(onDataReceivedOverBle);

    Serial.println("Server is ready and advertising...");

    ir.begin(IR_PIN); // OUT pinini bağladığın GPIO
    ir.onReceive(onIrCommand);
}

void loop()
{
    uart_manager.loop();
    ir.loop();
}
