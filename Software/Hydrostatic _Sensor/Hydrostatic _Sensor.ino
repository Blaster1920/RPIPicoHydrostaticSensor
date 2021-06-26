//Dev: Jacopo Franco, 26/06/2021
//Simple control sketch to actuate a sewage pump from the readings of two pressure sensors.
//The sensors used are the LPS33HW on a custom PCBs covered in epoxy to avoid corrosion.

#include <Adafruit_LPS35HW.h>

Adafruit_LPS35HW sensorLower = Adafruit_LPS35HW();
Adafruit_LPS35HW sensorHigher = Adafruit_LPS35HW();

#define sensorLower_SCK  2
#define sensorLower_MISO 3
#define sensorLower_MOSI 4

#define sensorLower_CS 5
#define sensorHigher_CS 6

int pumpRelay = 7;
int led = 25;

bool fault = false;

float pressureWater = 150.25;
float minimumallowedtemp = 2.0;

void setup() {
  pinMode(pumpRelay, OUTPUT);
  pinMode(led, OUTPUT);
  Serial.begin(115200);
  delay(3000);
  digitalWrite(led, HIGH);
  Serial.println("Hydrostatic Pressure sensor testing");

  if (!sensorLower.begin_SPI(sensorLower_CS)) {     //add for sw spi  (!sensorLower.begin_SPI(sensorLower_CS, sensorLower_SCK, sensorLower_MISO, sensorLower_MOSI)
    fault = true;
    Serial.println("Lower sensor NOT FOUND");
  }
  if (!sensorHigher.begin_SPI(sensorHigher_CS)) {
    fault = true;

    Serial.println("Higher sensor NOT FOUND");
  }
  digitalWrite(led, LOW);
  while (fault) {
    digitalWrite(led, HIGH);
    delay(50);
    digitalWrite(led, LOW);
    delay(50);
    Serial.println("SENSOR FAULT");
  }
}

void loop() {
  digitalWrite(led, HIGH);

  Serial.print("Temp Lower-Higher, Pressure Lower-Higher ");
  Serial.print(sensorLower.readTemperature());
  Serial.print(",");
  Serial.print(sensorHigher.readTemperature());
  Serial.print(",");
  Serial.print(sensorLower.readPressure());
  Serial.print(",");
  Serial.println(sensorHigher.readPressure());

  if (sensorHigher.readPressure() >= pressureWater) {
    if (sensorLower.readTemperature() <= minimumallowedtemp || sensorHigher.readTemperature() <= minimumallowedtemp) {
      sensorLower.setDataRate(LPS35HW_RATE_75_HZ);
      digitalWrite(pumpRelay, HIGH);
      while (sensorLower.readPressure() >= pressureWater) {
        digitalWrite(led, LOW);
        delay(8);
        digitalWrite(led, HIGH);
        delay(8);
      }
      digitalWrite(pumpRelay, LOW);
      sensorLower.setDataRate(LPS35HW_RATE_1_HZ);
      delay(10000);
    }
    else {
      Serial.println("WATER APPROACHING FREEZING TEMPERATURE, PUMP HALTED");
    }
  }
  else {
    Serial.println("WATER LEVEL TOO LOW, PUMPING NOT NEEDED");
  }

  delay(500);
  digitalWrite(led, LOW);
  delay(600);
}
