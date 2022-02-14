// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU6050.h"
#include "LiquidCrystal.h"

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 accelgyro;
//MPU6050 accelgyro(0x69); // <-- use for AD0 high

//int16_t ax, ay, az;
int16_t gx, gy, gz;
#define lowest_point 20
#define highest_point 180
#define LED_PIN 13
bool blinkState = false;

int down = 0;
int rep = 0;

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
    // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif
    lcd.begin(16, 2);
    lcd.print("babsoluts");
    
    // initialize serial communication
    // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
    // it's really up to you depending on your project)
    Serial.begin(38400);

    // initialize device
    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

    // configure Arduino LED pin for output
    pinMode(LED_PIN, OUTPUT);

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("grattis");

    while (gx < 30) {
       accelgyro.getAcceleration(&gx, &gy, &gz);
    }
}

void loop() {
    // read raw accel/gyro measurements from device
    accelgyro.getAcceleration(&gx, &gy, &gz);

    gx = map(gx, -16384, 16383, -360, 360);
    gy = map(gy, -16384, 16383, -360, 360);
    gz = map(gz, -16384, 16383, -360, 360);
    
    // these methods (and a few others) are also available
    //accelgyro.getAcceleration(&ax, &ay, &az);
    //accelgyro.getRotation(&gx, &gy, &gz);

    // display tab-separated accel/gyro x/y/z values
    Serial.print("gyro:\t");
    //Serial.print(ax); Serial.print("\t");
    //Serial.print(ay); Serial.print("\t");
    //Serial.print(az); Serial.print("\t");
    Serial.print(gx); Serial.print("\t");
    Serial.print(gy); Serial.print("\t");
    Serial.println(gz);

    // blink LED to indicate activity
    blinkState = !blinkState;
    digitalWrite(LED_PIN, blinkState);

    if (down == 0) {
      lcd.setCursor(0,0);
      lcd.print("upp med armen   ");
      if (gx > highest_point) {
        rep++;
        down = 1;
      }
    }
    if (down == 1) {
      lcd.setCursor(0,0);
      lcd.print("ner med armen");
      if (gx < lowest_point) {
        down = 0;
      }
    }
    lcd.setCursor(0,1);
    lcd.print(rep);
    lcd.print("                ");
}
