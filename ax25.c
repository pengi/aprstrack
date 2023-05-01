/*
    This file is part of APRStrack.

    Copyright (C) 2010 Max Sikström / SA6BBC

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
#include <inttypes.h>
#include <avr/pgmspace.h>
#include <util/crc16.h>
#include "ax25.h"
#include "afsk.h"

#define BITREVERSE( _X ) ( \
        (((_X)>>7) & 0x01) | \
        (((_X)>>5) & 0x02) | \
        (((_X)>>3) & 0x04) | \
        (((_X)>>1) & 0x08) | \
        (((_X)<<1) & 0x10) | \
        (((_X)<<3) & 0x20) | \
        (((_X)<<5) & 0x40) | \
        (((_X)<<7) & 0x80) \
        )


void ax25_putbyte( uint16_t *crc, uint8_t byte );

/*
struct ax25_addr_t {
    uint8_t callsign[6];
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
*/

void ax25_putbyte( uint16_t *crc, uint8_t byte ) {
    *crc = _crc_ccitt_update( *crc, byte );
    AFSK_putbyte( byte );
}


void ax25_tx_packet( struct ax25_packet_t *pkt ) {
    uint8_t i,j;
    uint16_t k;
    uint16_t crc = 0xFFFF;

    // Start packet
    AFSK_tx_start();

    // Destination
    for( i=0; i<6; i++ ) {
        ax25_putbyte( &crc, pkt->dst.callsign[i] << 1 );
    }
    ax25_putbyte( &crc, pkt->dst.ssid << 1 );

    // Source
    for( i=0; i<6; i++ ) {
        ax25_putbyte( &crc, pkt->src.callsign[i] << 1 );
    }
    ax25_putbyte( &crc, pkt->src.ssid << 1 | ((pkt->n_rep==0)?1:0) ); // TODO: repeaters

    for( j=0; j<pkt->n_rep; j++ ) {
        for( i=0; i<6; i++ ) {
            ax25_putbyte( &crc, pkt->rep[j].callsign[i] << 1 );
        }
        ax25_putbyte( &crc, pkt->rep[j].ssid << 1 | ((pkt->n_rep==j+1)?1:0) ); // TODO: repeaters
    }

    // Control field
    ax25_putbyte( &crc, pkt->control );

    // Protocol ID
    ax25_putbyte( &crc, pkt->protocol );

    // Information field
    for( k=0; k<pkt->n_info; k++ )
        ax25_putbyte( &crc, pkt->info[k] );

    // FCS (crc32)
    crc ^= 0xFFFF;
    AFSK_putbyte(  crc     & 0xFF );
    AFSK_putbyte( (crc>>8) & 0xFF );

    // End packet
    AFSK_tx_end();
}
