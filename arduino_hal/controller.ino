#include <LiquidCrystal.h>
#include <SPI.h>
#include <NRFLite.h>

#define THROTTLE_INPUT A0
#define ELEVATOR_INPUT A1
#define AILERONS_INPUT A2
#define RUDDER_INPUT A3

#define LCD_D7 4
#define LCD_D6 5
#define LCD_D5 6
#define LCD_D4 7
#define LCD_EN 2
#define LCD_RS 3

#define RADIO_RX_ID 0
#define RADIO_ID 1
#define RADIO_CE 10
#define RADIO_CSN 9

#define ADC_CONSTANT 0.000802734

#define display_update_period 100
unsigned long display_last_update_time = 0;

LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
NRFLite radio;

struct RadioPayload {
  uint8_t throttle;
  uint8_t elvator;
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
}

void loop() {
  radio_payload.throttle = map(analogRead(THROTTLE_INPUT), 0, 1023, 100, 0);
  radio_payload.elvator = map(analogRead(ELEVATOR_INPUT), 0, 1023, 100, 0);
  radio_payload.ailerons = map(analogRead(AILERONS_INPUT), 0, 1023, 100, 30);
  radio_payload.rudder = map(analogRead(RUDDER_INPUT), 0, 1023, 100, 0);

  radio.send(RADIO_RX_ID, &radio_payload, sizeof(radio_payload));
  float voltage = 0;
  float current = 0;

  while (radio.hasAckData()) {
    radio.readData(&telemetry_payload);
    voltage = (telemetry_payload.raw_voltage * ADC_CONSTANT) * 11.07;
    current = (2.5 - (telemetry_payload.raw_current * ADC_CONSTANT) * 2.0) / 0.066;
    Serial.println(current);
  }

  // Refresh display
  if ((millis() - display_last_update_time) >= display_update_period) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(radio_payload.throttle);
    lcd.print("%");

    lcd.setCursor(6, 0);
    lcd.print(voltage, 1);
    lcd.print("V");

    if (current > 9) {
      lcd.setCursor(13, 0);
    } else {
      lcd.setCursor(14, 0);
    }

    if (current < 0) current = 0;
    lcd.print(current, 0);
    lcd.print("A");

    display_last_update_time = millis();
  }
}
