#include <Arduino.h>
#include <Wire.h>

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Wire.begin();
  // Wire.setClock(100000); // 100kHzに設定
  Serial.println("Finish I2C setup");
}

void scanI2CAddresses()
{
  Serial.println("Starting I2C address scan");
  for (uint8_t address = 1; address < 127; address++)
  {
    Wire.beginTransmission(address);
    uint8_t error = Wire.endTransmission();
    if (error == 0)
    {
      Serial.print("Found address: 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
    delay(2);
  }
  Serial.println("I2C address scan finished");
}

void loop()
{
  // シリアルから'scan'コマンドを受け取ったらスキャンモード
  if (Serial.available())
  {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    Serial.println(cmd);
    if (cmd == "scan")
    {
      scanI2CAddresses();
      return;
    }
  }

  // 固定アドレス0x08からデータを読み取り
  for (uint8_t address = 1; address <= 30; address++)
  {
    Wire.beginTransmission(address);
    uint8_t error = Wire.endTransmission();

    if (error != 0)
    {
      return;
    }

    // データを要求
    Wire.requestFrom(0x08, (uint8_t)2);
    if (Wire.available() >= 2)
    {
      int8_t sensorValue_MSB = Wire.read();
      int8_t sensorValue_LSB = Wire.read();
      int sensorValue = (sensorValue_MSB << 8) | sensorValue_LSB;
      Serial.print("Address: 0x");
      Serial.print(address, HEX);
      Serial.print(" Value: ");
      Serial.println(sensorValue);
    }
  }

  delay(1); // 1スキャンごとに待つ
}