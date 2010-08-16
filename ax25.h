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
#ifndef AX25_H
#define AX25_H

struct ax25_addr_t {
    char callsign[6];
    uint8_t ssid;
};

struct ax25_packet_t {
    struct ax25_addr_t dst;
    struct ax25_addr_t src;
    struct ax25_addr_t rep[8];
    uint8_t n_rep;

    uint8_t control;
    uint8_t protocol;

    uint8_t info[256];
    uint16_t n_info;
};



void ax25_tx_packet( struct ax25_packet_t *pkt );


#endif
