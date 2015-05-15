/* Имя: osccal.c
 * Автор: Christian Starkjohann
 * Перевод: microsin.ru 
 * Дата создания: 2008-04-10
 * Табуляция: 4
 * Copyright: (c) 2008 by OBJECTIVE DEVELOPMENT Software GmbH
 * Лицензия: GNU GPL v2 (см. License.txt) или проприетарная (CommercialLicense.txt)
 * Ревизия: $Id: osccal.c 553 2008-04-17 19:00:20Z cs $
 */

#include <avr/io.h>
#include "usbdrv.h"
#include <avr/eeprom.h>

 
#ifdef OSC_CAL_OLD
/* ------------------------------------------------------------------------- */
/* -------------------- Калибровка тактового генератора -------------------- */
/* ------------------------------------------------------------------------- */

/* Функция калибрует RC-генератор. Наш опорный источник точного времени - 
 *  сигнал SOF, Start Of Frame (один бит SE0), повторяющийся каждую милисекунду
 *  сразу после состояния USB RESET. Мы сначала делаем двоичный поиск величины
 *  OSCCAL, и затем оптимизируем эту величину поиском соседних значений.
 *  Этот алгоритм может также использоваться для калибровки RC-генератора напрямую
 *  до 12 MHz (не вовлекается ФАПЧ, таким образом эту технику можно использовать 
 *  почти на ВСЕХ AVR), но это слишком вне спецификации величины OSCCAL и необходимой 
 *  точности для тактов на 12 МГц! Используйте RC-генератор, калиброванный на 12 МГц
 *  только для экспериментов!
 */
void    calibrateOscillator(void)
{
	uint8_t		step = 128;
	uint8_t		trialValue = 0, optimumValue;
	int			x, optimumDev, targetValue = (unsigned)(1499 * (double)F_CPU / 10.5e6 + 0.5);

    /* делаем двоичный поиск: */
    do{
        OSCCAL = trialValue + step;
        x = usbMeasureFrameLength();    /* пропорционально текущей реальной частоте */
        if(x < targetValue)             /* частота еще слишком низкая */
            trialValue += step;
        step >>= 1;
    }while(step > 0);
    /* Здесь мы имеем точность +/- 1 от оптимального OSCCAL */
    /*  теперь ищем соседнее значение для оптимальной величины */
    optimumValue = trialValue;
    optimumDev = x; /* это конечно очень далеко от оптимума */
    for(OSCCAL = trialValue - 1; OSCCAL <= trialValue + 1; OSCCAL++){
        x = usbMeasureFrameLength() - targetValue;
        if(x < 0) x = -x;
        if(x < optimumDev)
		{
            optimumDev = x;
            optimumValue = OSCCAL;
        }
    }
    OSCCAL = optimumValue;
}
/*
Замечание: Этот алгоритм калибровки может пробовать 
величины OSCCAL до 192, даже если оптимальная величина далеко меньше 192. 
Таким образом, может быть превышена допустимая тактовая частота CPU в разработках 
с низковольтным питанием! Вы можете заменить этот алгоритм поиска любым другим
алгоритмом по Вашему желанию, если Вы имеете дополнительные ограничения как,
например, максимальная частота CPU.
Для RC-генераторов версий 5.x (у которых диапазон регулировки склеен из 2x128 
шагов, например ATTiny25, ATTiny45, ATTiny85), может быть полезно искать оптимум 
в обеих областях.
*/

#endif /* OSC_CAL_OLD */


/*****************************************************************************/
#define OSC_CAL
#ifdef  OSC_CAL
/* ------------------------------------------------------------------------- */
/* ------------------------ Oscillator Calibration ------------------------- */
/* ------------------------------------------------------------------------- */

/* Calibrate the RC oscillator to 8.25 MHz. The core clock of 16.5 MHz is
 * derived from the 66 MHz peripheral clock by dividing. Our timing reference
 * is the Start Of Frame signal (a single SE0 bit) available immediately after
 * a USB RESET. We first do a binary search for the OSCCAL value and then
 * optimize this value with a neighboorhod search.
 * This algorithm may also be used to calibrate the RC oscillator directly to
 * 12 MHz (no PLL involved, can therefore be used on almost ALL AVRs), but this
 * is wide outside the spec for the OSCCAL value and the required precision for
 * the 12 MHz clock! Use the RC oscillator calibrated to 12 MHz for
 * experimental purposes only!
 */
static void calibrateOscillator(void)
{
uchar       step = 128;
uchar       trialValue = 0, optimumValue;
int         x, optimumDev, targetValue = (unsigned)(1499 * (double)F_CPU / 10.5e6 + 0.5);

    /* do a binary search: */
    do{
        OSCCAL = trialValue + step;
        x = usbMeasureFrameLength();    /* proportional to current real frequency */
        if(x < targetValue)             /* frequency still too low */
            trialValue += step;
        step >>= 1;
    }while(step > 0);
    /* We have a precision of +/- 1 for optimum OSCCAL here */
    /* now do a neighborhood search for optimum value */
    optimumValue = trialValue;
    optimumDev = x; /* this is certainly far away from optimum */
    for(OSCCAL = trialValue - 1; OSCCAL <= trialValue + 1; OSCCAL++){
        x = usbMeasureFrameLength() - targetValue;
        if(x < 0)
            x = -x;
        if(x < optimumDev){
            optimumDev = x;
            optimumValue = OSCCAL;
        }
    }
    OSCCAL = optimumValue;
}
/*
Note: This calibration algorithm may try OSCCAL values of up to 192 even if
the optimum value is far below 192. It may therefore exceed the allowed clock
frequency of the CPU in low voltage designs!
You may replace this search algorithm with any other algorithm you like if
you have additional constraints such as a maximum CPU clock.
For version 5.x RC oscillators (those with a split range of 2x128 steps, e.g.
ATTiny25, ATTiny45, ATTiny85), it may be useful to search for the optimum in
both regions.
*/

void usbEventResetReady(void)
{
    calibrateOscillator();
    eeprom_write_byte(0, OSCCAL);   /* store the calibrated value in EEPROM */
}

#endif /* OSC_CAL */
/*****************************************************************************/
