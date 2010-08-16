/*
    This file is part of APRStrack.

    APRStrack is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
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
	ADMUX = (0<<REFS1)|(1<<REFS0)|(0<<ADLAR)|(0xE<<MUX0);
	ADCSRA = (1<<ADEN)|(0<<ADATE)|(7<<ADPS0);
	ADCSRB = (0<<ADTS0);
}

uint16_t batv_get( void ) {
	ADCSRA |= (1<<ADSC);
	while( ADCSRA & (1<<ADSC) ){ /* Wait for conversion to complete */ }
	return (1024L*110L)/ADC;
}

