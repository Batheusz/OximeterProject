#include <Arduino.h>
#include <time.h>

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
#define CS 26
#define RESET 25
#define DC 22 // Pino SPI para alterar entre MOSI e MISO (shield usa uma porta para os dois)
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
#define SDA 14
#define INT 27
// Classe do MAX30102
MAX30105 MyOxi;
TwoWire I2C_Oxi = TwoWire(0);
// Configuração do MAX30102
#define BRILHO 0x1F // Brilho para aproximadamente 8 cm
#define MODE 2
#define PULSEWIDTH 411 // Microsegundos
#define SAMPLERATE 400 // Leituras por segundo
#define SAMPLEAVAREGE 16
#define ADCRANGE 8192
#define READTIME 15 // Segundos
#define DELAYREAD 3 // Segundos
#define RATESIZE 100

// Pino do botão de trigger
#define BTT 15

// Declaração de variáveis
time_t timestamp;
float temp = 0.0;
uint32_t red[RATESIZE];
uint32_t infrared[RATESIZE];
int32_t spo2;
int8_t validsSpo2;
int32_t heartRate;
int8_t validHeartRate;

// Declaração de funções
void drawText(char* text, uint8_t x, uint8_t y, uint16_t color, uint8_t size);
uint8_t oxiRead();
void bttTrigg();

void setup() {
  // Iniciando serial
  Serial.begin(115200);

  // Definindo interrupt
  // attachInterrupt(BTT, bttTrigg, HIGH);

  // Inicializando LCD
  ledcSetup(PWMCHANNEL, PWMHZ, PWMRESOLUTION); // Configura canal, frequência e resolução do PWM
  ledcAttachPin(BLK,PWMCHANNEL); // Define o canal do PWM no controle do backlight
  MyLCD.initR(INITR_MINI160x80_PLUGIN); // Array de cores para o LCD
  MyLCD.setRotation(3); // Atualiza da esquerda para direita
  ledcWrite(PWMCHANNEL, 1023); // Totalmente acesso
  MyLCD.fillScreen(ST7735_BLACK);

  // Inicializando MAX30102
  I2C_Oxi.setPins(SDA,SCL);
  if(MyOxi.begin(I2C_Oxi,I2C_SPEED_FAST) == false)
  {
    drawText("Sensor",3 ,10 , ST7735_WHITE, 2);
    drawText("ausente...",3 ,30 , ST7735_WHITE, 2);
    while(1);
  }
  else
  {
    drawText("Sensor",3 ,10 , ST7735_WHITE, 2);
    drawText("inicializado!",3 ,30 , ST7735_WHITE, 2);
  }
  MyOxi.setup(BRILHO,SAMPLEAVAREGE, MODE, SAMPLERATE, PULSEWIDTH, ADCRANGE);
  MyOxi.enableDIETEMPRDY();
  vTaskDelay(500);
  MyLCD.fillScreen(ST7735_BLACK);
  drawText("Pressione o",3 ,10 , ST7735_WHITE, 2);
  drawText("botao",3 ,30 , ST7735_WHITE, 2);
  vTaskDelay(500);

  oxiRead();
}

void loop() {
}
/**************************************************************************/
/*!
  @brief ISR para setar a flag de leitura
*/
/**************************************************************************/
void bttTrigg()
{
  oxiRead();
}
/**************************************************************************/
/*!
    @brief Função para escrer o texto desejado  
    @param x Localização x do inicio do texto
    @param y Localização y do inicio do texto
    @param text String com o texto a ser escrito
    @param color Cor a ser usada no texto
    @param size Tamanho da letra a ser usada 1,2 ou 3
*/
/**************************************************************************/
void drawText(char* text, uint8_t x, uint8_t y, uint16_t color, uint8_t size)
{
  MyLCD.setCursor(x,y);
  MyLCD.setTextColor(color);
  MyLCD.setTextWrap(1); // Quebra de linha automática
  MyLCD.setTextSize(size); // Tamanho da fonte
  MyLCD.print(text);
}
/**************************************************************************/
/*!
  @brief Função para executar todas as leituras do oximetro
*/
/**************************************************************************/
uint8_t oxiRead()
{
  // portDISABLE_INTERRUPTS();
  MyLCD.fillScreen(ST7735_BLACK);
  drawText("Posicione o",3 ,10 , ST7735_WHITE, 2);
  drawText("dedo",3 ,30 , ST7735_WHITE, 2);
  vTaskDelay(1000*DELAYREAD);

  uint32_t i=0;
  while(timestamp<=READTIME)
  {
    timestamp = time(NULL);
    for (int i = 0; i < RATESIZE; i++)
    {
      red[i] = MyOxi.getRed();
      infrared[i] = MyOxi.getIR();
      MyOxi.nextSample();
      timestamp = time(NULL);
      if (timestamp == 15)
      {
        break;
      }
    }

    // Calculando SpO2
    maxim_heart_rate_and_oxygen_saturation(infrared,RATESIZE,red,&spo2,&validsSpo2,&heartRate,&validHeartRate);
    Serial.print("SpO2: ");
    Serial.print(validsSpo2);
    Serial.print(" | HR: ");
    Serial.println(validHeartRate);
    Serial.print("SpO2: ");
    Serial.print(validsSpo2);
    Serial.print(" | HR: ");
    Serial.println(validHeartRate);
  }
  temp = MyOxi.readTemperature();
  // portENABLE_INTERRUPTS();
  return 1;
}