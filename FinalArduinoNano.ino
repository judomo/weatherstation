#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);

char TestData;

void setup() {
// Open serial communications and wait for port to open:
Serial.begin(115200);
while (!Serial) {
; // wait for serial port to connect. Needed for native USB port only
}

lcd.init();                     
lcd.backlight();

}


void loop() { // run over and over

if (Serial.available()) {
// wait a bit for the entire message to arrive
delay(100);
// clear the screen
lcd.clear();
// read all the available characters
while (Serial.available() > 0) {
// display each character to the LCD
lcd.write(Serial.read());
}
}
}