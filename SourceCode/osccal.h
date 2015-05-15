/* Имя: osccal.h
 * Автор: Christian Starkjohann
 * Перевод: microsin.ru 
 * Дата создания: 2008-04-10
 * Табуляция: 4
 * Copyright: (c) 2008 by OBJECTIVE DEVELOPMENT Software GmbH
 * Лицензия: GNU GPL v2 (см. License.txt) или проприетарная (CommercialLicense.txt)
 * Ревизия: $Id: osccal.h 553 2008-04-17 19:00:20Z cs $
 */

/*
Основное описание:
Этот модуль содержит функцию, которая калибрует внутренний RC-генератор AVR,
чтобы CPU работал на частоте F_CPU (F_CPU является макропределением, которое 
должно быть задано при компиляции модуля. Лучший выбор - передача этого макро
через командную строку компилятора). Эталон времени - частота фрейма USB 
1 кГц, доступная немедленно после состояния USB RESET. Отсчет времени производится
путем подсчета циклов CPU, приэтом все прерывания должны быть запрещены, пока работает
калибровочная функция. Для измерений времени низкого уровня вызывается функция 
usbMeasureFrameLength(). Эта функция должна быть разрешена в usbconfig.h 
путем задания USB_CFG_HAVE_MEASURE_FRAME_LENGTH в 1.

Используемый алгоритм:
calibrateOscillator() сначала делает двоичный поиск в регистре OSCCAL для 
наилучшего совпадения с частотой тактового генератора. Затем она делает поиск
следующей соседней (частоты?) для нахождения наименьшей девиации тактовой 
частоты. Это гарантирует наилучшее соответствие между соседними величинами,
но для генераторов версии 5 (у которых прерывающаяся взаимосвязь между
OSCCAL и частотой) лучшее соответствие может быть доступно в другом диапазоне
OSCCAL.

Ограничения:
Этот алгоритм калибровки может пробовать величины OSCCAL до 192, даже если
оптимальная величина далеко меньше 192. Таким образом, может быть превышена 
допустимая тактовая частота CPU в разработках с низковольтным питанием!
Точность зависит от OSCCAL простив частотной зависимости генератора.
Типичная точность для ATMega168 (вычисленная от OSCCAL против F_RC диаграмма
в даташите) должна быть в диапазоне 0.4%. Только версия 16.5 МГц AVR-USB 
(которая использует ФАПЧ в приемнике данных) может допускать такую девиацию!
Все другие частотные модули требуют точности 0.3%.
*/

#ifndef __OSCCAL_H_INCLUDED__
#define __OSCCAL_H_INCLUDED__

void calibrateOscillator(void);
/* Эта функция калибрует генератор RC, чтобы CPU работал на частоте F_CPU.
 * Она ДОЛЖНА быть вызвана немедленно после окончания состояния USB RESET!
 * Запретите все прерывания во время вызова! Рекомендуется сохранить 
 * результирующую величину в EEPROM так, чтобы хорошее предполагаемое 
 * значение было доступно после следующего сброса.
 */

void    usbEventResetReady(void);

#endif /* __OSCCAL_H_INCLUDED__ */
