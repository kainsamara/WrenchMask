#include <Arduino.h>

#define ON_PRESS        1 // нажатие кнопки
#define ON_PRESS_LONG   2 // кнопка удерживается нажатой	2000 мс
#define ON_RELEASE      3 // отпускание кнопки

class Keys
{
public:
    Keys(uint8_t = 255, uint8_t = 255, uint8_t = 255, uint8_t = 255, uint8_t = 255, uint8_t = 255);
    // инициализация клавиатуры
    void begin(uint32_t timeHold = 1000);
    void read();
    bool justPressed() const; // определение нажатой кнопки
    bool justReleased() const; // определение отжатой кнопки
    bool isHold() const; // определение зажатие кнопки
    uint8_t	getNum = 0; // цифровое обозначение кнопки
private:
    uint8_t	_pinsKB[6]; // номера выводов к которым подключена клавиатура
    uint32_t _timeHold; // время длительного зажатия кнопки
    uint8_t _state; // состояние системы
    uint8_t	_numberKeyNow;  // номер сейчас нажатой кнопки
    uint8_t	_numberKeyWas = 255; // номер ранее нажатой кнопки
    uint32_t _msNumberKeyState; // время нажатия кнопки
    uint8_t _numberKeyNowLong; // длинное нажатие
    uint8_t	_massItem; // номер подмассива для вывода информации
    uint8_t	_massNumberKey[1][5] = {0x1, 0x2, 0x3, 0x4, 0x5}; // массив цифровых обозначений номера кнопки для переменной getNum
    void _findPressKeyNow(); // определение номера кнопки нажатой в данный момент времени
};
