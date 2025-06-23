#include <Arduino.h>
#include <Wire.h>

// エラーメッセージを表示する関数
void printI2CError(uint8_t error, uint8_t address)
{
  Serial.print("I2C Error at address 0x");
  if (address < 16)
    Serial.print("0");
  Serial.print(address, HEX);
  Serial.print(": ");

  switch (error)
  {
  case 0:
    Serial.println("Success");
    break;
  case 1:
    Serial.println("Data too long to fit in transmit buffer");
    break;
  case 2:
    Serial.println("Received NACK on transmit of address");
    break;
  case 3:
    Serial.println("Received NACK on transmit of data");
    break;
  case 4:
    Serial.println("Other error");
    break;
  case 5:
    Serial.println("Timeout");
    break;
  default:
    Serial.print("Unknown error code: ");
    Serial.println(error);
    break;
  }
}

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

  for (uint8_t address = 1; address <= 127; address++)
  {
    delay(1); // 1スキャンごとに待つ
    Wire.beginTransmission(address);
    uint8_t error = Wire.endTransmission();

    if (error != 0)
    {
      // エラーが発生した場合に詳細なメッセージを表示
      // printI2CError(error, address);
      continue; // 次のアドレスに進む
    }

    // データを要求
    Wire.requestFrom(address, (uint8_t)2);
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
}