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
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "afsk.h"

#define AFSK_DIFF 1

volatile uint8_t afsk_phase = 0;
volatile uint8_t afsk_bitcnt = 0;
volatile uint8_t afsk_curbyte = 0;
volatile uint8_t afsk_outbits = 0;
volatile uint8_t afsk_sendbits = 0;
volatile uint8_t afsk_curflagbit = 0;
volatile uint8_t afsk_nextbyte = 0x00;
volatile uint8_t afsk_nextbyte_status = 0;
volatile uint8_t afsk_tx = 0;
#if AFSK_DIFF
volatile uint8_t lastbit = 0;
#endif

const uint8_t afsk_sint[96] = {
   25,   26,   27,   28,   30,   31,   32,   33,
   35,   36,   37,   38,   39,   40,   40,   41,
   42,   42,   43,   43,   44,   44,   44,   44,
   45,   44,   44,   44,   44,   43,   43,   42,
   42,   41,   40,   40,   39,   38,   37,   36,
   35,   33,   32,   31,   30,   28,   27,   26,
   25,   23,   22,   21,   19,   18,   17,   16,
   14,   13,   12,   11,   10,    9,    9,    8,
    7,    7,    6,    6,    5,    5,    5,    5,
    5,    5,    5,    5,    5,    6,    6,    7,
    7,    8,    9,    9,   10,   11,   12,   13,
   14,   16,   17,   18,   19,   21,   22,   23
};


ISR( TIMER1_OVF_vect ) {
    uint8_t delta;

// Update output
    if( afsk_tx ) {
        delta = (afsk_sendbits&1) ? 6 : 11;
        afsk_phase = (afsk_phase + delta) % 96;
    } else {
        afsk_phase = 0; // No tx? phase = 0 => sin(0)=0
    }
    OCR0B = afsk_sint[afsk_phase]; // Update output

// Update bitbuffer
    if( afsk_bitcnt ) {
        afsk_bitcnt--;
        if( (afsk_bitcnt&0x0F) == 0 ) { // Time for new bit?
            if( !(afsk_curflagbit & 0x02) && ((afsk_outbits&0x1F) == 0x1F) ) {
                // Bitstuff
                afsk_outbits <<= 1;
                afsk_bitcnt += 16; // some more bits this byte...
            } else {
                // Normal operation
                afsk_outbits <<= 1;
                afsk_outbits |= afsk_curbyte&1;
                afsk_curbyte >>= 1;
                // Buffer empty and data on input?
                if( afsk_bitcnt == 0 && (afsk_nextbyte_status&1) ) {
                    afsk_curbyte = afsk_nextbyte;
                    afsk_curflagbit = afsk_nextbyte_status & 0x02;
                    afsk_nextbyte_status = 0;
                    afsk_bitcnt = 8*16;
                }
            }
            afsk_sendbits <<= 1;
            afsk_sendbits |= afsk_outbits & 1;
#if AFSK_DIFF
            afsk_sendbits ^= ((afsk_sendbits>>1)&1)^1;
#endif
        }
    } else if( afsk_nextbyte_status&1 ) {
        afsk_curbyte = afsk_nextbyte;
        afsk_curflagbit = afsk_nextbyte_status & 0x02;
        afsk_nextbyte_status = 0;
        afsk_bitcnt = 8*16;
    }
}

void AFSK_Init( void ) {
    // Init PWM output (timer0)

    TCCR0A  = (0<<COM0A1)
            | (0<<COM0A0)
            | (1<<COM0B1)
            | (0<<COM0B0)
            | (1<<WGM01)
            | (1<<WGM00);

    TCCR0B  = (1<<WGM02)
            | (0<<CS02)
            | (0<<CS01)
            | (1<<CS00);

    OCR0A   = 50;

    OCR0B   = 0;

    // Init timer1

    TCCR1A  = (0<<COM1A1)
            | (0<<COM1A0)
            | (0<<COM1B1)
            | (0<<COM1B0)
            | (1<<WGM11)
            | (1<<WGM10);

    TCCR1B  = (0<<ICNC1)
            | (0<<ICES1)
            | (1<<WGM13)
            | (1<<WGM12)
            | (0<<CS12) // No clock source... enable when using the output
            | (0<<CS11)
            | (0<<CS10);


    OCR1A = (F_CPU/256/19200)-1;

    TIMSK1 |= (1<<TOIE1);

    // Init PTT driver

    DDRD  |= (1<<PORTD6); // PD6 => 0 inactive, 1 active
    PORTD &= ~(1<<PORTD6);
//    DDRD  &= ~(1<<PORTD7); // PD7 => Z inactive, 0 active
//    PORTD &= ~(1<<PORTD7);
}


void AFSK_putbyte( uint8_t byte ) {
    while( afsk_nextbyte_status&1 )
        asm volatile ("nop");
    afsk_nextbyte = byte;
    afsk_nextbyte_status = 1;
}

void AFSK_putbyte_flag( void ) {
    while( afsk_nextbyte_status&1 )
        asm volatile ("nop");
    afsk_nextbyte = 0x7e;
    afsk_nextbyte_status = 3;
}

void AFSK_tx_start( void ) {
    uint8_t i;
    // Enable output
    DDRD   |= (1<<PORTD5);
    // Start the PWM timer
    TCCR0B |= (0<<CS02)
            | (0<<CS01)
            | (1<<CS00);
    // Start the timer
    TCCR1B |= (1<<CS12)
            | (0<<CS11)
            | (0<<CS10);
    // Sync in with a couple of flag bytes
    // Press PTT
    PORTD |= (1<<PORTD6); // PD6 => 0 inactive, 1 active
//    DDRD  |= (1<<PORTD7); // PD7 => Z inactive, 0 active
//    DDRB |= (1<<PORTB0);
    for( i=0; i<10; i++ ) {
        AFSK_putbyte_flag();
    }
    afsk_tx = 1;
    for( i=0; i<140; i++ ) {
        AFSK_putbyte_flag();
    }
}

void AFSK_tx_end( void ) {
    uint8_t i;
    // Sync out...
    for( i=0; i<5; i++ ) {
        AFSK_putbyte_flag();
    }
    AFSK_wait_finish();
    afsk_tx = 0;
    // Release PTT
    PORTD &= ~(1<<PORTD6); // PB0 => 0 inactive, 1 active
//    DDRD  &= ~(1<<PORTD7); // PD7 => Z inactive, 0 active
//    DDRB &= ~(1<<PORTB0);
    // Stop the timer
    TCCR1B &= ~(
                (1<<CS12)
              | (1<<CS11)
              | (1<<CS10)
            );
    // Stop the PWM timer
    TCCR0B &= ~(
                (1<<CS12)
              | (1<<CS11)
              | (1<<CS10)
            );
    // Disable output
    PORTD  &= ~(1<<PORTD5);
    DDRD   &= ~(1<<PORTD5);
}

void AFSK_wait_finish( void ) {
    uint8_t waiting;
    uint8_t sreg;
    do {
        sreg = SREG;
        cli();
        waiting = afsk_nextbyte_status || afsk_bitcnt;
        SREG = sreg;
    } while( waiting );
    afsk_phase=0;
    OCR0B=afsk_sint[afsk_phase];
    afsk_outbits = 0;
    afsk_sendbits = 0;
};
