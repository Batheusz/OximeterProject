#include <Arduino.h>

// Bibliotecas para o display IPS 80x160
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

// Bibliotecas para módulo oxímetro MAX30102
#include <MAX30105.h>
#include <Wire.h>
#include <heartRate.h>
#include <spo2_algorithm.h>

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
#define PWMCHANNEL 0
#define PWMHZ 5000
#define PWMRESOLUTION 10
#define MAXDUTY 1024 // 2^RESOLUTION

// Definição dos pinos MAX30102
#define SCL 13
#define SDA 12
#define INT 14
// Classe do MAX30102
MAX30105 MyOxi;
TwoWire I2C_Oxi = TwoWire(0);
// Pino do botão de trigger
#define BTT 15

// Declaração de funções
void drawText(char* text, uint8_t x, uint8_t y, uint16_t color, uint8_t size);

void setup() {
  // Iniciando serial
  Serial.begin(115200);

  // Inicializando LCD
  ledcSetup(PWMCHANNEL, PWMHZ, PWMRESOLUTION); // Configura canal, frequência e resolução do PWM
  ledcAttachPin(BLK,PWMCHANNEL); // Define o canal do PWM no controle do backlight
  MyLCD.initR(INITR_MINI160x80_PLUGIN); // Array de cores para o LCD
  MyLCD.setRotation(3); // Atualiza da esquerda para direita
  ledcWrite(PWMCHANNEL, 1023); // Totalmente acesso
  MyLCD.fillScreen(ST7735_BLACK);

  // Inicializando MAX30102
  I2C_Oxi.setPins(SDA,SCL);
  uint8_t Flag_OxiBegin = 0;
  if(MyOxi.begin(I2C_Oxi,I2C_SPEED_FAST) == false)
  {
    drawText("Sensor",0 ,0 , ST7735_WHITE, 2);
    drawText("ausente...",0 ,20 , ST7735_WHITE, 2);
    while(1);
  }
  MyOxi.setup();
}

void loop() {
  Serial.print(" R[");
  Serial.print(MyOxi.getRed());
  Serial.print("] IR[");
  Serial.print(MyOxi.getIR());
  Serial.print("] G[");
  Serial.print(MyOxi.getGreen());
  Serial.print("]");
}

/*!
    @brief Função para escrer o texto desejado  
    @param x Localização x do inicio do texto
    @param y Localização y do inicio do texto
    @param text String com o texto a ser escrito
    @param color Cor a ser usada no texto
    @param size Tamanho da letra a ser usada 1,2 ou 3
    @param wrap Quebra de texto, normalmente ativada
*/
void drawText(char* text, uint8_t x, uint8_t y, uint16_t color, uint8_t size)
{
  MyLCD.setCursor(x,y);
  MyLCD.setTextColor(color);
  MyLCD.setTextWrap(1); // Quebra de linha automática
  MyLCD.setTextSize(size); // Tamanho da fonte
  MyLCD.print(text);
}