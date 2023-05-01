/*
    This file is part of APRStrack.

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
#include "nmea.h"
#include "lib/uart/serial.h"
#include "lib/uart/uart.h"

void nmea_send_can(uint8_t timer);

uint8_t nmea_buffer[80];
uint8_t nmea_comindex[25];
uint8_t nmea_buffer_index, nmea_buflen, nmea_comlen;

uint8_t a2d (uint8_t chr) { // Convert ascii to decimal
    return (chr-48);
}

void nmea_clear_comindex(void) {
    uint8_t i;

    for (i=0;i<25;i++) {
        nmea_comindex[i]=0;
    }
}


void nmea_parse_GGA(void) {
    if (nmea_buffer[nmea_comindex[1]+1] != ',')
    {
    	PORTD ^= (1<<PORTD3);
//        uint8_t i;

        //Get time in UTC
        NMEA_data.time_h = 10*a2d(nmea_buffer[6]) + a2d(nmea_buffer[7]);
        NMEA_data.time_m = 10*a2d(nmea_buffer[8]) + a2d(nmea_buffer[9]);
        NMEA_data.time_s = 10*a2d(nmea_buffer[10]) + a2d(nmea_buffer[11]);

        //Get fixtype
        NMEA_data.fixvalid=a2d(nmea_buffer[nmea_comindex[5]+1]);

        //Get number of satellites used in fix
        NMEA_data.usedsat = 10*a2d(nmea_buffer[nmea_comindex[6]+1]) + a2d(nmea_buffer[nmea_comindex[6]+2]);

        // $GPGGA,174049.0,5741.285,N,01158.695,E,1,06,1.96,00065,M,040,M,,*53
        // Get latitude
        if (nmea_comindex[2]-nmea_comindex[1] == 9 /*10*/) {

            NMEA_data.lat_deg = 10*a2d(nmea_buffer[nmea_comindex[1]+1])
            + a2d(nmea_buffer[nmea_comindex[1]+2]);

            NMEA_data.lat_min = 10*a2d(nmea_buffer[nmea_comindex[1]+3])
            + a2d(nmea_buffer[nmea_comindex[1]+4]);

            NMEA_data.lat_sec_h = 10*a2d(nmea_buffer[nmea_comindex[1]+6]);
            NMEA_data.lat_sec_h += a2d(nmea_buffer[nmea_comindex[1]+7]);
            NMEA_data.lat_sec_l = 10*a2d(nmea_buffer[nmea_comindex[1]+8]);
            //NMEA_data.lat_sec_l += a2d(nmea_buffer[nmea_comindex[1]+9]);
            NMEA_data.lat_sec_l = NMEA_data.lat_sec_l << 1;

            //Last bit contains cardinal direction, N=0, S=1
            if (nmea_buffer[nmea_comindex[2]+1] == 'N') {
                NMEA_data.lat_sec_l &=0b11111110;
            } else {
                NMEA_data.lat_sec_l |=1;
            }
        }


        // Get longitude
        if (nmea_comindex[4]-nmea_comindex[3] == 10 /*11*/) {
            NMEA_data.lon_deg = 100*a2d(nmea_buffer[nmea_comindex[3]+1])
            + 10*a2d(nmea_buffer[nmea_comindex[3]+2])
            + a2d(nmea_buffer[nmea_comindex[3]+3]);

            NMEA_data.lon_min = 10*a2d(nmea_buffer[nmea_comindex[3]+4])
            + a2d(nmea_buffer[nmea_comindex[3]+5]);

            NMEA_data.lon_sec_h = 10*a2d(nmea_buffer[nmea_comindex[3]+7]);
            NMEA_data.lon_sec_h += a2d(nmea_buffer[nmea_comindex[3]+8]);
            NMEA_data.lon_sec_l = 10*a2d(nmea_buffer[nmea_comindex[3]+9]);
            //NMEA_data.lon_sec_l += a2d(nmea_buffer[nmea_comindex[3]+10]);
            NMEA_data.lon_sec_l = NMEA_data.lon_sec_l << 1;

            //Last bit contains cardinal direction, E=0, W=1
            if (nmea_buffer[nmea_comindex[4]+1] == 'E') {
                NMEA_data.lon_sec_l &=0b11111110;
            } else {
                NMEA_data.lon_sec_l |=1;
            }


        }


    }
}

void nmea_parse_GSA(void) {
    //TODO
}

void nmea_parse_GSV(void) {
    //TODO
}

void nmea_parse_RMC(void) {
    //TODO
}

void nmea_make_index(void) {

    nmea_clear_comindex();

    uint8_t i;
    uint8_t j=0;

    for (i=0;i<nmea_buflen;i++) {
        if (nmea_buffer[i] == ',') {
            nmea_comindex[j] = i;
            j++;
        }
    }
    nmea_comlen = j;
}

void nmea_parse(void) {
    // Supported messages: GSA, GSV, GGA, RMC
    nmea_make_index();

    if (
        nmea_buffer[2] == 'G' &&
        nmea_buffer[3] == 'S' &&
        nmea_buffer[4] == 'A')
    {
        nmea_parse_GSA();
    } else if (
        nmea_buffer[2] == 'G' &&
        nmea_buffer[3] == 'S' &&
        nmea_buffer[4] == 'V') {
        nmea_parse_GSV();
    } else if (
        nmea_buffer[2] == 'G' &&
        nmea_buffer[3] == 'G' &&
        nmea_buffer[4] == 'A') {
        nmea_parse_GGA();
    } else if (
        nmea_buffer[2] == 'R' &&
        nmea_buffer[3] == 'M' &&
        nmea_buffer[4] == 'C') {
        nmea_parse_RMC();
    }

}

void sns_nmea_Init(void)
{
	uint8_t i;

	uint8_t tsip_command[] = {0x10, 0xBC, 0x00, 0x07, 0x07, 0x03, 0x00, 0x00, 0x00, 0x02, 0x04, 0x00, 0x10, 0x03}; //switch to nmea and 9600 baud
	for(i = 0; i<14; i++){
		uart_putc(tsip_command[i]);
	}

}

void sns_nmea_Process(void)
{
    uint16_t rx_uart;
    uint8_t rx_char;

    rx_uart = uart_getc();

    if (rx_uart != UART_NO_DATA) {
        rx_char = (uint8_t) rx_uart & 0x00ff;

        if (rx_char == 0x24) {
            nmea_buffer_index=0;
        } else if (rx_char == '\n') {
            nmea_buflen = nmea_buffer_index+1;
            nmea_parse();
        } else {
            nmea_buffer[nmea_buffer_index] = rx_char;
            nmea_buffer_index++;
        }
    }
}
