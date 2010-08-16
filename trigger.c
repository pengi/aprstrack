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
#include <avr/interrupt.h>
#include "trigger.h"
#include "config.h"

uint16_t trigger_c;

ISR( TIMER2_COMPB_vect ) {
	if( trigger_c == 0 ) {
		Trigger_Event();
		trigger_c = BEACONTIME * 100;
	} else {
		trigger_c--;
	}
}

void Trigger_Init( void ) {
	TCCR2A = (1<<WGM21) | (1<<WGM20);
	TCCR2B = (1<<WGM22) | (1<<CS22) | (1<<CS21) | (1<<CS20);
	OCR2A = 143;
	OCR2B = 0;
	TIMSK2 = (1<<OCIE2B);
	trigger_c = 0;
}

