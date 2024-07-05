//TEAM 2 Guilman,Schatzyki, Feierman, Mendelovich
#include <U8g2lib.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Preferences.h>

#define DHTPIN 23  
#define BOT1 35
#define BOT2 34

#define MODO1 0
#define MODO2 1
#define INTERMEDIO1 2
#define INTERMEDIO2 3
#define INTERMEDIO3 4

#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
Preferences preferences;

int estado = 0;

float tempActual;
char stringTempActual[5];
int valUmb2=28;
char stringValUmbral[5];

unsigned long tiempo = 0;
int seg = 0;


void setup() {
  Serial.begin(9600);
  dht.begin();
  u8g2.begin();
  preferences.begin("temperatura", false);
  pinMode(BOT1, INPUT_PULLUP);
  pinMode(BOT2, INPUT_PULLUP);
  valUmb2 = preferences.getInt("temperatura", 28);
}

void loop() {
  u8g2.clearBuffer(); 
  millis();
  maquina();
  u8g2.sendBuffer();
  Serial.println(tempActual); Serial.print(estado); Serial.println(tiempo - seg); 
  tiempo = millis();
  tempActual = dht.readTemperature();
    
}

void maquina() {
  switch (estado) {
    case MODO1:
    
      displayTempUmbral();
      if (digitalRead(BOT1) == LOW) {
        seg = tiempo;
        estado = INTERMEDIO1;
      }
      break;

    case INTERMEDIO1:
      displayTempUmbral();
      if (digitalRead(BOT1) == HIGH && tiempo - seg > 5000) {
        estado = MODO2;
      }
      if (digitalRead(BOT1) == HIGH && tiempo - seg < 5000) {
        estado = MODO1;
      }
      break;

    case MODO2:
      displayUmbral();
      if (digitalRead(BOT1) == LOW) {
        estado = INTERMEDIO2;
      }
      if (digitalRead(BOT2) == LOW) {
        seg = tiempo;
        estado = INTERMEDIO3;
      }
      break;

    case INTERMEDIO2:
      displayUmbral();
      if (digitalRead(BOT1) == HIGH) {
        valUmb2=valUmb2+1;
        estado = MODO2;
      }
      break;

    case INTERMEDIO3:
      displayUmbral();
      if (digitalRead(BOT2) == HIGH && tiempo - seg < 5000) {
        valUmb2=valUmb2-1;
        estado = MODO2;
      }
      if (digitalRead(BOT2) == HIGH && tiempo - seg > 5000) {
        preferences.putInt("temperatura", valUmb2                       );
        estado = MODO1;

      }
      break;
  }
}

void displayTempUmbral() {
  u8g2.clearBuffer();

  u8g2.setFont(u8g2_font_ncenB14_tr);
  u8g2.drawStr(0, 15, "Temp: ");
  sprintf(stringTempActual, "%.2f", tempActual);
  u8g2.drawStr(65, 15, stringTempActual);

  u8g2.setFont(u8g2_font_ncenB14_tr);
  u8g2.drawStr(0, 40, "Umbral: ");
  sprintf(stringValUmbral, "%.2d", valUmb2);
  u8g2.drawStr(85, 40, stringValUmbral);

  u8g2.sendBuffer();
}

void displayUmbral() {
  u8g2.clearBuffer();

  u8g2.setFont(u8g2_font_ncenB14_tr);
  u8g2.drawStr(0, 30, "Umbral: ");
  sprintf(stringValUmbral, "%.2d", valUmb2);
  u8g2.drawStr(85, 30, stringValUmbral);

  u8g2.sendBuffer();
}