#include <SoftwareSerial.h>

SoftwareSerial secondSerial(5, 13);

void setup()  {
  pinMode(5,INPUT);
  pinMode(13,OUTPUT);
  Serial.begin(9600);
  secondSerial.begin(38400);
  Serial.println("Программа начала работу, подайте 3.3в на пин 34 модуля и используйте команду AT для проверки связи (Ответ ОК) и команду AT+NAME для изменения имени модуля");
}

void loop() {
  if (secondSerial.available()) {
    char t = secondSerial.read();
    Serial.print(t);
  }
  if (Serial.available()) {
    char t = Serial.read();
    secondSerial.write(t);
  }
}
