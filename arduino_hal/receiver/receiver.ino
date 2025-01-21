#include <Servo.h>
#include <NRFLite.h>

#define RADIO_TX_ID 1
#define RADIO_ID 0
#define RADIO_CE PC13
#define RADIO_CSN PA4

#define BATTERY_PIN PA0
#define CURRENT_PIN PA1

#define RADIO_FAILSAFE_TIMEOUT_ms 500
#define RADIO_PACKET_TIMEOUT_ms 20

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

unsigned long last_packet_time = 0;

void setup() {
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

  // Process radio data with a timeout
  unsigned long packet_start_time = millis();
  while (radio.hasData()) {
    radio.readData(&radio_payload);
    last_packet_time = millis();

    throttle.writeMicroseconds(
      constrain(map(radio_payload.throttle, 0, 100, 1000, 2000), 1000, 2000)
    );

    elevator.writeMicroseconds(
      constrain(map(radio_payload.elevator, 0, 100, 1000, 2000), 1000, 2000)
    );

    ailerons.writeMicroseconds(
      constrain(map(radio_payload.ailerons, 0, 100, 1000, 2000), 1000, 2000)
    );

    rudder.writeMicroseconds(
      constrain(map(radio_payload.rudder, 0, 100, 1000, 2000), 1000, 2000)
    );

    radio.addAckData(&telemetry_payload, sizeof(telemetry_payload));

    if (millis() - packet_start_time > RADIO_PACKET_TIMEOUT_ms) {
      break;
    }
  }

  // Failsafe: if no data received for more than 500ms, reset servos
  if (millis() - last_packet_time > RADIO_FAILSAFE_TIMEOUT_ms) {
    throttle.writeMicroseconds(1000);
    elevator.writeMicroseconds(1500);
    ailerons.writeMicroseconds(1500);
    rudder.writeMicroseconds(1500);
  }
}
