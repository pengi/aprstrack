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
#ifndef SNS_NMEA
#define SNS_NMEA

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
/* system files */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
/* lib files */
//#include <config.h>
//#include <bios.h>
//#include <drivers/can/stdcan.h>
//#include <drivers/timer/timer.h>

void sns_nmea_Init(void);
void sns_nmea_Process(void);
//void sns_nmea_HandleMessage(StdCan_Msg_t *rxMsg);
void sns_nmea_List(uint8_t ModuleSequenceNumber);

struct {
    uint8_t time_h, time_m, time_s;
    uint8_t lat_deg, lon_deg; //Följer inte auml:s enhetsstandard
    uint8_t lat_min, lon_min;
    uint8_t lat_sec_h, lat_sec_l, lon_sec_h, lon_sec_l; //Last bit contains cardinal direction

    uint8_t fixtype; //0=invalid, 1=GPS, 2=DGPS
    uint8_t fixvalid; //From GGA, 0 or 1
    uint8_t usedsat;
} NMEA_data;

#endif // SNS_NMEA
