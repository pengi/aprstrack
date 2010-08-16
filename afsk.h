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
#ifndef AFSK_H
#define AFSK_H

void AFSK_Init( void );
void AFSK_putbyte( uint8_t byte );
void AFSK_putbyte_flag( void );

void AFSK_tx_start( void );
void AFSK_tx_end( void );


void AFSK_wait_finish( void );

#endif
