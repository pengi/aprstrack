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
#include "ax25.h"
#include "aprs.h"

void aprs_packet_rewind( struct ax25_packet_t *pkt ) {
    pkt->n_info = 0;
}

void aprs_pack_position( struct ax25_packet_t *pkt, double lat, double lon, uint16_t sym ) {
    int32_t lati = lat*6000.0;
    int32_t loni = lon*6000.0;
    uint8_t *iptr = pkt->info + pkt->n_info;

    iptr[0] = '!';

    if( lati < 0 ) {
        iptr[8] = 'S';
        lati = -lati;
    } else {
        iptr[8] = 'N';
    }

    iptr[1] = '0' + (lati/60000)%10;
    iptr[2] = '0' + (lati/ 6000)%10;
    iptr[3] = '0' + (lati/ 1000)%6;
    iptr[4] = '0' + (lati/  100)%10;
    iptr[5] = '.';
    iptr[6] = '0' + (lati/   10)%10;
    iptr[7] = '0' + (lati      )%10;

    iptr[9] = (sym>>8)&0xFF;

    if( loni < 0 ) {
        iptr[18] = 'W';
        loni = -loni;
    } else {
        iptr[18] = 'E';
    }

    iptr[10] = '0' + (loni/600000)%10;
    iptr[11] = '0' + (loni/ 60000)%10;
    iptr[12] = '0' + (loni/  6000)%10;
    iptr[13] = '0' + (loni/  1000)%6;
    iptr[14] = '0' + (loni/   100)%10;
    iptr[15] = '.';
    iptr[16] = '0' + (loni/    10)%10;
    iptr[17] = '0' + (loni       )%10;

    iptr[19] = sym&0xFF;

    pkt->n_info+=20;

}

void aprs_add_comment( struct ax25_packet_t *pkt, char *message ) {
    while( *message != '\0' ) {
        pkt->info[ pkt->n_info++ ] = *message++;
    }
}
