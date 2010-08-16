/*
    This file is part of APRStrack.

    Copyright (C) 2010 Max Sikström / SA6BBC

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
#include <avr/sleep.h>
#include <stdio.h>

#include "lib/uart/uart.h"
#include "afsk.h"
#include "ax25.h"
#include "aprs.h"
#include "nmea.h"
#include "config.h"
#include "trigger.h"
#include "batv.h"


double crd_lat;
double crd_long;

volatile uint8_t send=0;

struct ax25_packet_t pkt = {
    {"APZ123",0},
    {CALLSIGN,SSID},
    {
        {"WIDE1 ",0x01},
        {"WIDE2 ",0x02}
    },
    2,
    0x03,
    0xf0,
    "",
    6
};

void Trigger_Event( void ) {
    send = 1;
}

int main(void)
{
    double lat = 0.0, lon = 0.0;
    char buffer[32];
    uint16_t v;

    AFSK_Init();
    uart_init((UART_BAUD_SELECT(9600,F_CPU)));
    Trigger_Init();
    batv_init();

    // Sleep control
    DDRC |= (1<<PORTC0);
    PORTC &= ~(1<<PORTC0);

    // Debug led
    DDRD |= (1<<PORTD3);
    PORTD &= ~(1<<PORTD3);

//    set_sleep_mode( SLEEP_MODE_PWR_DOWN );
//    set_sleep_mode( SLEEP_MODE_IDLE );

    sei();
    sns_nmea_Init();

    send=1;
    for(;;) {
//        sleep_enable();
//            sleep_cpu();
//        sleep_disable();
//        c = uart_getc();
    	sns_nmea_Process();
//        if( !(c&0xFF00) ) {
//            PORTB ^= (1<<PORTB1);
//        }
        if( send ) {

            // Convert to pengi friendly format
            lat = (double)(NMEA_data.lat_deg) + (double)(NMEA_data.lat_min/60.0) + (double)(NMEA_data.lat_sec_h/6000.0);
            lon = (double)(NMEA_data.lon_deg) + (double)(NMEA_data.lon_min/60.0) + (double)(NMEA_data.lon_sec_h/6000.0);

            // end conversion

            v = batv_get();
            sprintf( buffer, "Vcc: %d.%02dV", v/100, v%100 );

            aprs_packet_rewind( &pkt );
            aprs_pack_position( &pkt, lat /*57.68111667*/, lon /*11.98611667*/, '/'<<8|'[' );
            aprs_add_comment(   &pkt, buffer );
            ax25_tx_packet( &pkt );
            sns_nmea_Init();
            send = 0;
        }
    }


    return 0;
}

