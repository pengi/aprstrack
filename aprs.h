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
#ifndef APRS_H
#define APRS_H

#include "ax25.h"

void aprs_packet_rewind( struct ax25_packet_t *pkt );
void aprs_pack_position( struct ax25_packet_t *pkt, double lat, double lon, uint16_t sym );
void aprs_add_comment( struct ax25_packet_t *pkt, char *message );


#endif
