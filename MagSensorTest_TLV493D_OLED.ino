/*********************************************************************
  Displays magnetic field measurements on OLED

  Magnetic field sensor: TLV493D
  OLED: 128x32 graphic display
  OLED Driver: SSD1306 (I2C)

  Trevor Bruns
  September 2018
*********************************************************************/

#include <SPI.h>
#include "TLV493D.h"
#include "Wire.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

TLV493D magSensor;
const int magSensor_pwr_pin = 15;
const int i2c_sda = 18;
double maxB = 15; // [mT] max expected B field value

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

const int pixPosUnitX = 66;
const int pixOffsetX = 23;
const int pixVert = 8;
const int pixLoopTimeX = 100;
const int pixLoopTimeY = 24;
int16_t  X1, Y1;
int16_t  X2, Y2;
int16_t  X3, Y3;
int16_t  X4, Y4;
uint16_t w, h;
int16_t barX = pixPosUnitX + 14;
int16_t barY = 3;
uint16_t barW = 128 - barX;
uint16_t barH = 3;

void setup()   {
  // start serial

  Serial.begin(115200);
  delay(1000);
  
  // Initialize OLED

  display.begin(SSD1306_SWITCHCAPVCC, 0X3C);  // initialize with the I2C addr 0X3D (for the 128x64)
  display.setRotation(180);
  display.clearDisplay();
  display.display();

  display.setTextSize(2);
  display.setCursor(0, 0);
  display.setTextColor(WHITE);
  display.println("Starting");
  display.print("Sensor");
  display.display();


  // Initialize mag sensor

  pinMode(magSensor_pwr_pin, OUTPUT);
  pinMode(i2c_sda, OUTPUT);
  digitalWrite(magSensor_pwr_pin, LOW);
  digitalWrite(i2c_sda, LOW);
  delay(500);
  digitalWrite(magSensor_pwr_pin, HIGH);
  digitalWrite(i2c_sda, LOW); //0x1F
  delay(500);
  Wire.begin();
  Serial.print("Initializing sensor 1: 0x");
  Serial.println(magSensor.init(LOW), HEX);


  // setup static text (so we don't waste time refreshing it over and over)

  display.clearDisplay();
  display.display();

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.setTextColor(WHITE);

  display.print("Bx");
  display.setCursor(16,0);
  display.print("=");
  display.setCursor(pixPosUnitX, 0);
  display.print("mT");

  display.setCursor(0, pixVert);
  display.print("By");
  display.setCursor(16,pixVert);
  display.print("=");
  display.setCursor(pixPosUnitX, pixVert);
  display.println("mT");

  display.setCursor(0, pixVert * 2);
  display.print("Bz");
  display.setCursor(16,pixVert*2);
  display.print("=");
  display.setCursor(pixPosUnitX, pixVert * 2);
  display.println("mT");
  display.display();

  display.drawFastVLine(0, pixVert*3, 8, WHITE);
  display.drawFastVLine(2, pixVert*3, 8, WHITE);
  display.setCursor(4, pixVert * 3);
  display.print("B");
  display.drawFastVLine(10, pixVert*3, 8, WHITE);
  display.drawFastVLine(12, pixVert*3, 8, WHITE);
  display.setCursor(16, pixVert*3);
  display.print("=");
  display.setCursor(pixPosUnitX, pixVert * 3);
  display.println("mT");
  display.display();

  char valString[] = "-123.45";
  display.getTextBounds(valString, pixOffsetX, 0, &X1, &Y1, &w, &h);
  display.getTextBounds(valString, pixOffsetX, pixVert, &X2, &Y2, &w, &h);
  display.getTextBounds(valString, pixOffsetX, pixVert * 2, &X3, &Y3, &w, &h);
  display.getTextBounds(valString, pixOffsetX, pixVert * 3, &X4, &Y4, &w, &h);
}

//-----------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------//


void loop() {

  // update magnetic field sensor

  if (!magSensor.update()) {
    //    Serial.print(magSensor.m_dBx);
    //    Serial.print(",");//\t");
    //    Serial.print(magSensor.m_dBy);
    //    Serial.print(",");//\t");
    //    Serial.println(magSensor.m_dBz);
    //    Serial.print(", ");
    //    Serial.println(magSensor.m_dMag_2);
    //    delay(25);

    double Bmag = sqrt(magSensor.m_dMag_2);
    
    int loopStart = millis();

    // clear areas for new values

    display.fillRect(X1, Y1, w, h, BLACK);
    display.fillRect(X2, Y2, w, h, BLACK);
    display.fillRect(X3, Y3, w, h, BLACK);
    display.fillRect(X4, Y4, w, h, BLACK);
    display.fillRect(barX, barY, barW, barH, BLACK);
    display.fillRect(barX, barY + pixVert, barW, barH, BLACK);
    display.fillRect(barX, barY + pixVert * 2, barW, barH, BLACK);
    display.fillRect(barX, barY + pixVert * 3, barW, barH, BLACK);

    // display new values

    display.setTextColor(WHITE);
    if (magSensor.m_dBx < 0) {
      display.setCursor(pixOffsetX, 0);}
    else {
      display.setCursor(pixOffsetX + 6, 0);}
    display.print(magSensor.m_dBx, 2);
    display.fillRect(barX, barY, int(abs(barW * magSensor.m_dBx / maxB)), barH, WHITE);

    if (magSensor.m_dBy < 0) {
      display.setCursor(pixOffsetX, pixVert);}
    else {
      display.setCursor(pixOffsetX + 6, pixVert);}
    display.print(magSensor.m_dBy, 2);
    display.fillRect(barX, barY + pixVert, int(abs(barW * magSensor.m_dBy / maxB)), barH, WHITE);

    if (magSensor.m_dBz < 0) {
      display.setCursor(pixOffsetX, pixVert * 2);}
    else {
      display.setCursor(pixOffsetX + 6, pixVert * 2);}
    display.print(magSensor.m_dBz, 2);
    display.fillRect(barX, barY + pixVert * 2, int(abs(barW * magSensor.m_dBz / maxB)), barH, WHITE);

    display.setCursor(pixOffsetX + 6, pixVert * 3);
    display.print(Bmag, 2);
    display.fillRect(barX, barY + pixVert * 3, int(abs(barW * Bmag / maxB)), barH, WHITE);

    display.display();

    // report display refresh rate
      Serial.println(millis() - loopStart);
  }
}
