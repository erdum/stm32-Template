#include <Servo.h>
#include <SPI.h>
#include <NRFLite.h>

#define RADIO_TX_ID 1
#define RADIO_ID 0
#define RADIO_CE PC13
#define RADIO_CSN PA4

#define BATTERY_PIN PA0
#define CURRENT_PIN PA1

NRFLite radio;
Servo throttle;
Servo elevator;
Servo ailerons;
Servo rudder;

struct RadioPayload {
  uint8_t throttle;
  uint8_t elevator;
  uint8_t ailerons;
  uint8_t rudder;
} radio_payload;

struct TelemetryPayload {
  uint16_t raw_voltage;
  uint16_t raw_current;
} telemetry_payload;

void setup() {
  Serial.begin(9600);
  radio.init(RADIO_ID, RADIO_CE, RADIO_CSN);

  throttle.attach(PA2);
  elevator.attach(PA3);
  ailerons.attach(PA8);
  rudder.attach(PB0);

  throttle.writeMicroseconds(2000);
  elevator.writeMicroseconds(2000);
  ailerons.writeMicroseconds(2000);
  rudder.writeMicroseconds(2000);

  delay(1000);
  throttle.writeMicroseconds(1000);
}

void loop() {
  telemetry_payload.raw_voltage = analogRead(BATTERY_PIN);
  telemetry_payload.raw_current = analogRead(CURRENT_PIN);

  while (radio.hasData()) {
    radio.readData(&radio_payload);
    throttle.writeMicroseconds(map(radio_payload.throttle, 0, 100, 1000, 2000));
    elevator.writeMicroseconds(map(radio_payload.elevator, 0, 100, 1000, 2000));
    ailerons.writeMicroseconds(map(radio_payload.ailerons, 0, 100, 1000, 2000));
    rudder.writeMicroseconds(map(radio_payload.rudder, 0, 100, 1000, 2000));
    radio.addAckData(&telemetry_payload, sizeof(telemetry_payload));
  }
}
