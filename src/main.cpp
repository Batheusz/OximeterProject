#include <Arduino.h>

// Bibliotecas para o display IPS 80x160
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

// Bibliotecas para módulo oxímetro MAX30102
#include <MAX30105.h>
#include <Wire.h>
#include <heartRate.h>

// Definição dos pinos LCD
#define SCK 32
#define MOSI 33
#define MISO 34
#define CS 27
#define RESET 25
#define DC 26 // Pino SPI para alterar entre MOSI e MISO (shield usa uma porta para os dois)
#define BLK 23 // Pino para saturação do transistor da luz de fundo
// Classe do LCD
Adafruit_ST7735 MyLCD = Adafruit_ST7735(CS, DC, MOSI, SCK, RESET);
// Configuração do PWM para o brilho
// Definição dos pinos MAX30102
#define SCL 13
#define SDA 12
#define INT 14

// Pino do botão de trigger
#define BTT 15

void setup() {
  // Iniciando serial
  Serial.begin(115200);

  // Inicializando LCD
  ledcAttachPin(BLK,0);
  ledcSetup(0, 5000, 10);
  MyLCD.initR(INITR_MINI160x80);
  uint16_t time = millis();
  MyLCD.fillScreen(ST7735_RED);
  time = millis() - time;
  Serial.println(time, DEC);
  delay(500);

  MyLCD.fillScreen(ST77XX_RED);
  time = millis() - time;
  Serial.println(time, DEC);
  delay(500);
  MyLCD.fillScreen(ST77XX_GREEN);
  time = millis() - time;
  Serial.println(time, DEC);
  delay(500);
  MyLCD.fillScreen(ST77XX_BLUE);
  time = millis() - time;
  Serial.println(time, DEC);
  delay(500);
  MyLCD.fillScreen(ST77XX_BLACK);
  time = millis() - time;
  Serial.println(time, DEC);
  delay(500);
  MyLCD.fillCircle(40,100,20,ST7735_YELLOW);
  MyLCD.fillCircle(40,100,10,ST7735_GREEN);

}

void loop() {
   for(int i = 0; i <= 1023; i++)
  {
    ledcWrite(0, i);
    delay(30);
    //delayMicroseconds(100);
    Serial.println(i);
  }
}
