/*
libusbrelay: Control USB HID connected electrical relay modules
Copyright (C) 2014  Darryl Bond
Library version
Copyright (C) 2019  Sean Mollet

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
/* relay.h */

#ifndef __USBRELAY_H
#define __USBRELAY_H

//Configuration
#define CMD_ON 0xff
#define CMD_OFF 0xfd
#define CMD_SET_SERIAL 0xfa

#define Serial_Length 5

//Struct for relay boards
typedef struct relay_board {
	//+1 for the \0 string terminator
	char serial[Serial_Length+1];
	unsigned char relay_count;
	unsigned char state;
	char *path;
} relay_board;

//Public methods
int enumerate_relay_boards(const char *product, int verbose, int debug);
int operate_relay(const char *path,unsigned char relay, unsigned char state);
int set_serial(const char *path,char *newserial);
relay_board *find_board(const char *serial);
int get_relay_board_count(void);
relay_board* get_relay_boards(void);
void shutdown(void);




#endif
