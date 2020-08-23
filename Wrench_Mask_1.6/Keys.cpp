#include "Keys.h"

Keys::Keys(uint8_t pin1KB, uint8_t pin2KB, uint8_t pin3KB, uint8_t pin4KB, uint8_t pin5KB, uint8_t pin6KB) {
    _pinsKB[0] = pin1KB;
    _pinsKB[1] = pin2KB;
    _pinsKB[2] = pin3KB;
    _pinsKB[3] = pin4KB;
    _pinsKB[4] = pin5KB;
    _pinsKB[5] = pin6KB; 
}

// инициализация клавиатуры
void Keys::begin(uint32_t timeHold) {
    _timeHold = timeHold;
    pinMode(_pinsKB[0], OUTPUT);        // устанавливаем выводы в соответствующие им состояния
    digitalWrite(_pinsKB[0], HIGH);     // переводим вывод в режим передачи и устанавливаем состояние «1»
    pinMode(_pinsKB[1], INPUT_PULLUP);  // переводим выводы в режим приёма с подтягивающим резистором
    pinMode(_pinsKB[2], INPUT_PULLUP);
    pinMode(_pinsKB[3], INPUT_PULLUP);
    pinMode(_pinsKB[4], INPUT_PULLUP);
    pinMode(_pinsKB[5], INPUT_PULLUP);
}

void Keys::read() {                     // считывание состояние кнопки
    _state = false;
    //определяем номер кнопки нажатой в данный момент времени
    _findPressKeyNow();
    if (_numberKeyNow != 255 && _numberKeyNow != _numberKeyWas) {
        // определяем номер кнопки нажатой в данный момент времени
        getNum =  _massNumberKey[_massItem][_numberKeyNow];
        _numberKeyNowLong = _numberKeyNow;
        _msNumberKeyState = millis();
        _state = ON_PRESS;
    }
    if (_numberKeyNow == 255 && _numberKeyNow != _numberKeyWas && _numberKeyWas != 255 && _numberKeyNowLong != 255) {
        // определяем номер зажатой кнопки в данный момент времени
        getNum = _massNumberKey[_massItem][_numberKeyWas];
        _state = ON_RELEASE;
    }
    if (_numberKeyNow != 255 && _numberKeyNowLong != 255 && millis() - _msNumberKeyState > _timeHold) {
        // определяем номер кнопки отжатой в данный момент времени
        getNum = _massNumberKey[_massItem][_numberKeyNow];
        _numberKeyNowLong = 255;
        _msNumberKeyState = millis();
        _state = ON_PRESS_LONG;
        }
    _numberKeyWas = _numberKeyNow;      // запоминаем нажатую кнопку
}
bool Keys::justPressed() const {        // определение нажимаемой кнопки
    return _state == ON_PRESS ? 1 : 0;
}
bool Keys::justReleased() const {       // определение отжимаемой кнопки
    return _state == ON_RELEASE ? 1 : 0;
}
bool Keys::isHold() const {             // определение отжимаемой кнопки
    return _state == ON_PRESS_LONG  ? 1 : 0;
}
// определяем номер кнопки нажатой в данный момент времени
void Keys::_findPressKeyNow() {
    _numberKeyNow = 255;
    //определяем номер кнопки нажатой в данный момент времени
    // устанавливаем состояние вывода 0 «LOW»
    digitalWrite(_pinsKB[0], LOW); 
    if (!digitalRead(_pinsKB[1])) {     // проверяем состояние вывода 1
      _numberKeyNow = 1;
    }
    if (!digitalRead(_pinsKB[2])) {     // проверяем состояние вывода 2
      _numberKeyNow = 0;
    }
    if (!digitalRead(_pinsKB[3])) {     //  проверяем состояние вывода 3
      _numberKeyNow = 3;
    }
    if (!digitalRead(_pinsKB[4])) {     //  проверяем состояние вывода 4
      _numberKeyNow = 2;
    }
    if (!digitalRead(_pinsKB[5])) {     //  проверяем состояние вывода 5
      _numberKeyNow = 4;
    }
    digitalWrite(_pinsKB[0], HIGH);     // устанавливаем состояние вывода 0 «HIGH»
}
