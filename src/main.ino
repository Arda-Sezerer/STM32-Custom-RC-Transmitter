#include <SPI.h>
#include <Wire.h>
#include <RF24.h>

// --- PIN TANIMLAMALARI ---
#define POT_SOL       PA2
#define POT_SAG       PA3
#define POT_DIKEY1    PA0
#define POT_DIKEY2    PA1

#define JOY_SOL_X     PA4
#define JOY_SOL_Y     PA5
#define JOY_SAG_X     PB1
#define JOY_SAG_Y     PB0

#define SURGU_1       PC13
#define SURGU_2       PA15
#define SURGU_3       PC14
#define SURGU_4       PB5
#define SURGU_5       PB8
#define SURGU_6       PB9

#define BTN_1         PB11
#define BTN_2         PB10
#define BTN_3         PA7
#define BTN_4         PA6
#define BTN_5         PC15

#define LED_MAVI      PB12
#define LED_SARI      PB4
#define LED_YESIL     PA10
#define LED_KIRMIZI   PB3

#define NRF_CE        PA8
#define NRF_CSN       PA9

#define MPU_ADDR      0x68
#define MPU_SCL       PB6
#define MPU_SDA       PB7

SPIClass SPI_2(PB15, PB14, PB13);
RF24 radio(NRF_CE, NRF_CSN);
const byte address[6] = "00001";

bool mpuOk = false;

struct DataPackage {
  uint16_t potSol, potSag, potDikey1, potDikey2;
  uint16_t joySolX, joySolY, joySagX, joySagY;
  uint8_t surguler;
  uint8_t butonlar;
  bool mpuBagli;
  int16_t roll;
  int16_t pitch;
  int16_t yaw;
};

DataPackage data;

bool initMPU6050() {
  Wire.beginTransmission(MPU_ADDR);
  if (Wire.endTransmission() != 0) return false;

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B); // Power Management
  Wire.write(0);    // Uyku modunu kapat
  return (Wire.endTransmission() == 0);
}

void setup() {
  // JTAG kapat, SWD açık bırak (PB3, PB4, PA15 GPIO olur)
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN; 
  AFIO->MAPR = (AFIO->MAPR & ~AFIO_MAPR_SWJ_CFG) | AFIO_MAPR_SWJ_CFG_JTAGDISABLE;

  pinMode(POT_SOL, INPUT_ANALOG);
  pinMode(POT_SAG, INPUT_ANALOG);
  pinMode(POT_DIKEY1, INPUT_ANALOG);
  pinMode(POT_DIKEY2, INPUT_ANALOG);

  pinMode(JOY_SOL_X, INPUT_ANALOG);
  pinMode(JOY_SOL_Y, INPUT_ANALOG);
  pinMode(JOY_SAG_X, INPUT_ANALOG);
  pinMode(JOY_SAG_Y, INPUT_ANALOG);

  pinMode(SURGU_1, INPUT_PULLUP);
  pinMode(SURGU_2, INPUT_PULLUP);
  pinMode(SURGU_3, INPUT_PULLUP);
  pinMode(SURGU_4, INPUT_PULLUP);
  pinMode(SURGU_5, INPUT_PULLUP);
  pinMode(SURGU_6, INPUT_PULLUP);

  pinMode(BTN_1, INPUT_PULLUP);
  pinMode(BTN_2, INPUT_PULLUP);
  pinMode(BTN_3, INPUT_PULLUP);
  pinMode(BTN_4, INPUT_PULLUP);
  pinMode(BTN_5, INPUT_PULLUP);

  pinMode(LED_MAVI, OUTPUT);
  pinMode(LED_SARI, OUTPUT);
  pinMode(LED_YESIL, OUTPUT);
  pinMode(LED_KIRMIZI, OUTPUT);

  // I2C Başlat
  Wire.setSCL(MPU_SCL);
  Wire.setSDA(MPU_SDA);
  Wire.begin();

  mpuOk = initMPU6050();

  // SPI2 Başlat
  SPI_2.begin();
  if (radio.begin(&SPI_2)) {
    digitalWrite(LED_YESIL, HIGH);
  } else {
    digitalWrite(LED_KIRMIZI, HIGH);
  }

  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);
  radio.stopListening();
}

void loop() {
  data.potSol    = analogRead(POT_SOL);
  data.potSag    = analogRead(POT_SAG);
  data.potDikey1 = analogRead(POT_DIKEY1);
  data.potDikey2 = analogRead(POT_DIKEY2);

  data.joySolX   = analogRead(JOY_SOL_X);
  data.joySolY   = analogRead(JOY_SOL_Y);
  data.joySagX   = analogRead(JOY_SAG_X);
  data.joySagY   = analogRead(JOY_SAG_Y);

  data.surguler = 0;
  if (!digitalRead(SURGU_1)) data.surguler |= (1 << 0);
  if (!digitalRead(SURGU_2)) data.surguler |= (1 << 1);
  if (!digitalRead(SURGU_3)) data.surguler |= (1 << 2);
  if (!digitalRead(SURGU_4)) data.surguler |= (1 << 3);
  if (!digitalRead(SURGU_5)) data.surguler |= (1 << 4);
  if (!digitalRead(SURGU_6)) data.surguler |= (1 << 5);

  data.butonlar = 0;
  if (!digitalRead(BTN_1)) data.butonlar |= (1 << 0);
  if (!digitalRead(BTN_2)) data.butonlar |= (1 << 1);
  if (!digitalRead(BTN_3)) data.butonlar |= (1 << 2);
  if (!digitalRead(BTN_4)) data.butonlar |= (1 << 3);
  if (!digitalRead(BTN_5)) data.butonlar |= (1 << 4);

  data.mpuBagli = mpuOk;
  if (mpuOk) {
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x3B);
    if (Wire.endTransmission(false) == 0 && Wire.requestFrom(MPU_ADDR, 14, true) == 14) {
      int16_t Ax = Wire.read() << 8 | Wire.read();
      int16_t Ay = Wire.read() << 8 | Wire.read();
      int16_t Az = Wire.read() << 8 | Wire.read();
      Wire.read(); Wire.read(); // Temp
      Wire.read(); Wire.read(); // Gyro X
      Wire.read(); Wire.read(); // Gyro Y
      int16_t Gz = Wire.read() << 8 | Wire.read();

      float rollVal  = atan2(Ay, Az) * 180.0 / M_PI;
      float pitchVal = atan2(-Ax, sqrt((long)Ay * Ay + (long)Az * Az)) * 180.0 / M_PI;

      data.roll  = (int16_t)rollVal;
      data.pitch = (int16_t)pitchVal;
      data.yaw   = (int16_t)(Gz / 131.0);
    } else {
      data.mpuBagli = false;
    }
  } else {
    data.roll  = 0;
    data.pitch = 0;
    data.yaw   = 0;
  }

  bool sent = radio.write(&data, sizeof(DataPackage));

  if (sent) {
    digitalWrite(LED_MAVI, HIGH);
    digitalWrite(LED_SARI, LOW);
  } else {
    digitalWrite(LED_MAVI, LOW);
    digitalWrite(LED_SARI, HIGH);
  }

  delay(20);
}
