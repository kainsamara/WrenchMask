/*
   Wrench Mask 1.6

   В этой версии реализовано управление по Bluetooth Serial порту, также имеются 5 кнопок, 
  есть поддержка кириллицы в бегущей строке, разнообразные статические картинки и паттерны 
  для вывода на матрицу светодиодов, также имеется индикация заряда батареи.
  
   Написана для маски @CYB3R_R0N1N из DC20e6     https://gitlab.com/CYB3R_R0N1N/mask/
  на основе прошивки @N3M351DA из нашего DC7495  https://github.com/n3m351d4/Wrench_mask/
  
   19 Авг 2020 @KainSamara 
*/

#include "font.h"
#include "emoticons.h"
#include "Keys.h"
#include <SoftwareSerial.h>

#define DATA_PIN 2      // В маску пин 2
#define SH_PIN 3        // В маску пин 4
#define ST_PIN 4        // В маску пин 3
#define LED_PIN 12      // Светодиод на клаве (инверсный уровень) если нет места в маске или он незачем, то его можно физически отрезать, отогнув подложку можно увидеть дорожки
#define RX_PIN 5        // указываем пин rx ардуины
#define TX_PIN 13       // и tx соответственно

#define RUSSIAN    // Выбери язык для bluetooth порта
//#define ENGLISH

// указываем номера пинов arduino подключенные к шлейфу клавиатуры: первым +5в общий потом кнопки кроме последнего (он на светодиод)
Keys KB(10, 11, 9, 7, 8, 6);

SoftwareSerial secondSerial(RX_PIN, TX_PIN); // Создадим связь с bluetooth модулем

uint8_t buff[10][3] = {0};    // Массив для LED матрицы
unsigned long checkbattimer;  // Таймер проверки батареи

int timing = 0;     // Сохраняет колличество циклов программы  
int repeats = 0;    // Используется для повторов в анимациях
int j = 0;          // Переменные для вывода анимации картинки
int k = 0;
int mode = 48;       // Переменная для текущего режима работы маски
int prev_mode = mode;
int nowmode = 64;   // Используется в тестовом режиме для пролистывания статичных картинок

// Перемеенные для работы с текстом через bluetooth модуль
bool setuptxt = 0;
bool message = 1;
char defaultxt[] = "   DEDSEC   Join US   ";
char text[70] = {0};

//Переменные для вольтметра
int inputVoltage = 1;           // Пин средней точки делителя напряжения для вольтметра на А1
float outputVoltage = 0.0;
float currentVoltage = 0.0;
float dischargeVoltage = 2.7;   // Напряжение при котором маска начнет сигнализировать о разряде
float AVCC = 4.87;              // 5.0 замените на фактическое напряжение 5в на пине Ардуины, по теории должно точно считать точно, но по факту этим значением можно приблизить показания к действительности
float R1 = 100100.0;            // Точное сопротивление R1 (100K)
float R2 = 9580.0;              // Точное сопротивление R2 (10K)
int vinValue = 0;

void setup() {                  // Понеслась=)
    pinMode(RX_PIN,INPUT);
    pinMode(TX_PIN,OUTPUT);
    pinMode(DATA_PIN, OUTPUT);
    pinMode(SH_PIN, OUTPUT);
    pinMode(ST_PIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
    pinMode(inputVoltage, INPUT);
    secondSerial.begin(9600);
    KB.begin();
    strcpy(text, defaultxt);    // Забиваем бегущую строку текстом по дефолту
    copy(buff,lines);
}

void loop() {
  timing++;          // Считаем число циклов программы
  battroutine();
  btroutine();
  KB.read();
  
  if (KB.justPressed()) {
    uint8_t keyNum = KB.getNum;
    #ifdef RUSSIAN
    secondSerial.print("Нажата кнопка ");
    #endif
    #ifdef ENGLISH
    secondSerial.print("Pressed Button ");
    #endif
    secondSerial.println(keyNum);
    copy(buff,zeros);
    prev_mode = mode;
    switch (keyNum) {
      case 1: mode = 48; break;
      case 2: mode = 52; break;
      case 3: mode = 50; break;
      case 4: mode = 51; break;
      case 5: mode = 62; break;
      default: break;
    }
  }
  else if (KB.isHold()) {
    uint8_t keyNum = KB.getNum;
    #ifdef RUSSIAN
    secondSerial.print("Долго нажата кнопка ");
    #endif
    #ifdef ENGLISH
    secondSerial.print("Long Pressed Button ");
    #endif
    secondSerial.println(keyNum);
    copy(buff,zeros);
    prev_mode = mode;
    switch (keyNum) {
      case 1: mode = 49; break;
      case 2: mode = 53; break;
      case 3: mode = 54; break;
      case 4: mode = 76; break;
      case 5: mode = 73; break;
      default: break;
    }     
  }
  
   switch (mode){                             // В порт слать символы 0-9 для режимов и A-Z и a-z для картинок БЕЗ ПЕРЕВОДА СТРОКИ! (принимается в ASCII и меняет mode) 

    case 33: pattern_lowbatt(); break;        // ! Батарея разряжена

    case 35: setuptext(); break;              // # Начать запись новой бегущей строки
    case 36: finishtext(); break;             // $ Закончить запись
    case 37: defaultext(); break;             // % Сбросить запись на дефолтную
    
    case 48: pattern_main(); break;           // 0
    case 49: showtext(); break;
    case 50: pattern_mimimig(); break;
    case 51: pattern_mimimi(); break;
    case 52: pattern_alert(); break;
    case 53: pattern_question(); break;       // 5
    case 54: pattern_hearts(); break;
    case 55: copy(buff,goat); break;
    case 56: copy(buff,fuck); break;
    case 57: copy(buff,spares); break;        // 9

    case 60: prevmode(); break;               // <
    case 62: nextmode(); break;               // >
    case 63: help(); break;                   // ?
    
    case 65: copy(buff,crosses); break;       // A
    case 66: copy(buff,circles); break;
    //case 67: copy(buff,invaders2); break;   // НЕ ХВАТИЛО ПАМЯТИ =(
    //case 68: copy(buff,invaders3); break;
    
    case 96: copy(buff,test); break;          // Z
    
    case 97: copy(buff,oo); break;            // a
    case 98: copy(buff,ooes); break;
    case 99: copy(buff,equalz); break;
    case 100: copy(buff,browns); break;
    case 101: copy(buff,brownz); break;
    case 102: copy(buff,defis); break;        // f
    case 103: copy(buff,zz); break;           
    case 104: copy(buff,semicolon); break;
    case 105: copy(buff,percent); break;
    case 106: copy(buff,octotorp); break;
    case 107: copy(buff,at); break;           // k
    case 108: copy(buff,bucks); break;        
    case 109: copy(buff,spares); break;
    case 110: copy(buff,lines); break;
    case 111: copy(buff,mig); break;          
    case 112: copy(buff,alert); break;        // p
    case 113: copy(buff,questions); break;
    case 114: copy(buff,invaders); break;
    case 115: copy(buff,pacman); break;       
    case 116: copy(buff,fuck); break;
    case 117: copy(buff,goat); break;         // u
    case 118: copy(buff,heartsmini); break;
    case 119: copy(buff,hearts); break;      
    case 120: copy(buff,fox); break;
    case 121: copy(buff,cyber); break;
    case 122: copy(buff,lowbatt); break;      // z
        
    default: pattern_main(); break;
   }
   draw();
}

void nextmode(){
  nowmode++;
  if (nowmode == 67) {
    nowmode = 96;
    }
  if (nowmode > 122) {
    nowmode = 65;
    }
    mode = nowmode;
}

void prevmode(){
  nowmode--;
  if (nowmode < 65) {
    nowmode = 122;
    }
  if (nowmode == 95) {
    nowmode = 66;
    }
    mode = nowmode;
}

void setuptext(){
  copy(buff,test);
  setuptxt = 1;
  if (message == 1){
    #ifdef RUSSIAN
    secondSerial.println("Отправь текст после 3х пробелов, а после '$'");
    #endif
    #ifdef ENGLISH
    secondSerial.println("Enter new text after 3 spaces and send '$'");
    #endif
  message = 0;
  }
  if (secondSerial.available()) {
    if (secondSerial.read() == 36){
      mode = 36;
    } else {
    memset(text,0,70);
    secondSerial.readBytes(text, 140); // Кириллица приходит по 2 байта, поэтому 140
    secondSerial.println(text);
    }
}
}

void finishtext(){
    setuptxt = 0;
    message = 1;
    #ifdef RUSSIAN
    secondSerial.println("Готово. Отправь '%' для текста по дефолту");
    #endif
    #ifdef ENGLISH
    secondSerial.println("Sucsessful. To default text send '%'");
    #endif
    mode = 49;
}

void defaultext(){
    strcpy(text, defaultxt);
    #ifdef RUSSIAN
    secondSerial.println("Дефолтный текст: OK");
    #endif
    #ifdef ENGLISH
    secondSerial.println("Default text: OK");
    #endif
    mode = 49;
}

void help(){
    #ifdef RUSSIAN
    secondSerial.println("Команды: 0-9 a-z < > #");
    #endif
    #ifdef ENGLISH
    secondSerial.println("Commands: 0-9 a-z < > #");
    #endif
    mode = prev_mode;
}

void battroutine(){
     if (setuptxt == 0 && millis() - checkbattimer > 30000){  // Каждые пол минуты
     checkbattimer = millis();
     vinValue = analogRead(inputVoltage); // Читаем значение с делителя напряжения на А1 и считаем вольтаж батарейки
     outputVoltage = (vinValue * AVCC) / 1024.0;
     currentVoltage = outputVoltage / (R2/(R1+R2)); 
     secondSerial.print(currentVoltage , 2);
     #ifdef RUSSIAN
     secondSerial.println(" В АКБ");
     #endif
     #ifdef ENGLISH
     secondSerial.println(" V Battery");
     #endif
     if (currentVoltage <= dischargeVoltage){
     prev_mode = mode;
     mode = 33;
     }}
}

void btroutine(){
    if (setuptxt == 0 && secondSerial.available()) {
    int incoming = secondSerial.read(); // читаем что пришло с Bluetooth модуля
    copy(buff,zeros);
    prev_mode = mode;
    mode = incoming;
    }
}

void pattern_lowbatt(){
   if (timing > 200){
      copy(buff,lowbatt);
      timing = 0;
      repeats++;
      return;
    }
  if (timing > 100)
  {
  copy(buff,zeros);
  }
  if (repeats == 4){
      repeats = 0;
      mode = prev_mode; 
  }
}

uint8_t shift_flag = 1;

void pattern_main(){
   if (timing > 1100){
      copy(buff,circles);
      timing = 0;
      shift_flag = 1;
      return;
    }
  if (timing > 1050 && shift_flag){
    shift();
    shift_flag = 0;
    return;
  }
  if (timing > 400)
  {
  copy(buff,crosses);
  }
}

void pattern_question(){
  if (timing > 200){
    copy(buff,zeros);
    timing = 0;
    return;
  }

  if (timing > 100){
    copy(buff,questions);
  }
}

void pattern_alert(){
  if (timing > 200){
    copy(buff,zeros);
    timing = 0;
    return;
  }

  if (timing > 100){
    copy(buff,alert);
  }
}

void pattern_hearts(){
   if (timing > 200){
      copy(buff,heartsmini);
      timing = 0;
      return;
    }
  if (timing > 100)
  {
  copy(buff,hearts);  
  }
}

void pattern_mimimi(){
   if (timing > 150){
      copy(buff,lines);
      timing = 0;
      repeats++;
      return;
    }
  if (timing > 100)
  {
  copy(buff,zeros);
  }
  if (repeats == 3){
      repeats = 0;
      mode = prev_mode;  
  }
}

void pattern_mimimig(){
    if (timing > 2){
      copy(buff,mig);
      timing = 0;
  return;
  }
    if (timing > 1){
      mode = prev_mode;  
  }
}

// ---------------------------- А ТУТ НАЧИНАЕТСЯ МАГИЯ ОТ НЕМЕЗИДЫ -----------------

void copy (uint8_t x[10][3] ,uint8_t y[10][3] ){
   for ( int i = 0 ; i < 10 ; i++)
      for (int j = 0 ; j < 3 ; j++)
        x[i][j] = y[i][j];
}

void draw(){
  for ( int i = 0 ; i < 10 ; i++ ){    
    digitalWrite(ST_PIN, LOW);
    shiftOut(DATA_PIN, SH_PIN, LSBFIRST, ~(0b00000001 <<(15-i)) );
    shiftOut(DATA_PIN, SH_PIN, MSBFIRST, ~(0b00000001 << i) );
    //uint8_t let = getFont('a',i);
    for (int j = 2 ; j >= 0  ; j--) {
    //shiftOut(data_pin, sh_pin, LSBFIRST, let );
    shiftOut(DATA_PIN, SH_PIN, LSBFIRST, buff[i][j]);
    }

    digitalWrite(ST_PIN, HIGH);
  }
}

void shift(){
  for (int i = 0 ; i < 10 ; i++){
       buff[i][0] = buff[i][0] << 1;
       buff[i][0] = buff[i][0] | ( buff[i][1] >> 7 );
       buff[i][1] = buff[i][1] << 1;
       buff[i][1] = buff[i][1] | ( buff[i][2] >> 7 );
       buff[i][2] = buff[i][2] << 1;
  }
}

void showtext(){
    uint8_t let;
    if ( k == 7 ){
        if ( text[j+1] != 0){
            j++ ;
            k = 0;
        }
        else {
        j = 0 ;
        }
      } 
    if (timing > 6){
      timing = 0;
      // secondSerial.println(uint8_t(text[j])); // Узнаём код символа 
      if (uint8_t(text[j]) == 208 || uint8_t(text[j]) == 209){ // Избавляемся от стрёмного старшего байта кириллицы UTF-8 ценой буквы ё к сожалению
        j++;
      }
      shift();
        for ( int i = 0 ; i < 8 ; i++ ){
            let = getFont(text[j],i);       
            buff[i][2] |= 0b00000001 & (let >> k) ;            
          }
      
      k++; 
      }
    
} 
