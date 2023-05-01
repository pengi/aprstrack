/*
    This file is part of APRStrack.

    Copyright (C) 2010 Max Sikström / SA6BBC

    APRStrack is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    APRStrack is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with APRStrack.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <avr/io.h>
#include "batv.h"


void batv_init( void ) {
    /* Measure the 1.1V internal ref signal with the ADC,
     * but use Vref as reference. This makes it possible to calculate
     * Vref.
     */
    ADMUX = (0<<REFS1)|(1<<REFS0)|(0<<ADLAR)|(0xE<<MUX0);

    /* Init ADC without auto trigger */
    ADCSRA = (1<<ADEN)|(0<<ADATE)|(7<<ADPS0);
    ADCSRB = (0<<ADTS0);
}

uint16_t batv_get( void ) {
    /* Start conversion */
    ADCSRA |= (1<<ADSC);

    /* Wait for conversion to complete */
    while( ADCSRA & (1<<ADSC) ) {}

    /* The measured level is 1.1V (110 units), calculate Vref in 0.01V units */
    return (1024L*110L)/ADC;
}

