#include <Arduino.h>

#include "config.h"
#include "buzzer/BuzzerManager.h"
#include "level-sensor/SensorController.h"
#include "protocol/protocol.h"
#include "relay/RelayController.h"
#include "i2c/I2CManager.h"
#include "uart/UartManager.h"
#include "json/JsonCodec.h"


RelayController relay_controller(RELAY_PINS, MAX_RELAYS);
SensorController sensor_controller(MAX_SENSORS);
UartManager uart_manager(Serial1);
BuzzerManager buzzer_manager(BUZZER_PIN);

void readAllStatus(Response& response)
{
    for (uint8_t i = 0; i < MAX_SENSORS; i++)
    {
        response.levels[i] = sensor_controller.readLevel(i);
    }

    for (int i = 0; i < MAX_RELAYS; ++i)
    {
        response.relays[i] = relay_controller.getState(i);
    }
}

void onPacketReceived(String data)
{
    Serial.print("Command : ");
    Serial.println(data);
    buzzer_manager.beep(2000, 200);
    const Command command = JsonCodec::instance().deserializeCommand(data);
    switch (command.type)
    {
    case CommandType::ALL_RELAYS_OFF:
        relay_controller.allOff();
        break;
    case CommandType::RELAY_TOGGLE:
        relay_controller.toggle(command.parameter.item_number);
        break;
    case CommandType::RELAY_OFF:
        relay_controller.turnOff(command.parameter.item_number);
        break;
    case CommandType::RELAY_ON:
        relay_controller.turnOn(command.parameter.item_number);
        break;
    case CommandType::ALL_STATUS:
        Serial.println("All Status command received");
        break;
    case CommandType::LEVEL_STATUS:
        Serial.println("Level status command received");
        break;
    case CommandType::UNKNOWN:
        Serial.println("Unknown command received");
        break;
    default: ;
        Serial.println("Unhandled command received");
    }
    Response response;

    readAllStatus(response);

    const String serialized_response = JsonCodec::instance().serializeResponse(response);
    uart_manager.sendData(serialized_response);
}


void setup()
{
    delay(100);
    Serial.begin(115200);
    delay(100);
    sensor_controller.addSensor(0, SENSOR_PINS[0], SENSOR1_MIN_RESISTANCE, SENSOR1_MAX_RESISTANCE, REFERENCE_RESISTOR,
                                ADC_RESOLUTION, SUPPLY_VOLTAGE);
    sensor_controller.addSensor(1, SENSOR_PINS[1], SENSOR2_MIN_RESISTANCE, SENSOR2_MAX_RESISTANCE, REFERENCE_RESISTOR,
                                ADC_RESOLUTION, SUPPLY_VOLTAGE);
    sensor_controller.addSensor(2, SENSOR_PINS[2], SENSOR3_MIN_RESISTANCE, SENSOR3_MAX_RESISTANCE, REFERENCE_RESISTOR,
                                ADC_RESOLUTION, SUPPLY_VOLTAGE);

    delay(100);
    buzzer_manager.begin();
    Serial.println("Arduino Buzzer Started");
    delay(100);

    relay_controller.begin();
    Serial.println("Arduino Relay Controller Started");

    delay(100);

    sensor_controller.begin();
    Serial.println("Level Sensor Controller Started");

    delay(100);

    uart_manager.start(9600);
    uart_manager.onDataReceived(onPacketReceived);
    Serial.println("Arduino UART Node Started");
    buzzer_manager.beep(1000, 100);
}

void loop()
{
    uart_manager.loop();
}
