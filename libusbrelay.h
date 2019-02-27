/*
usbrelay: Control USB HID connected electrical relay modules
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

#define ON 0xff
#define OFF 0xfd
#define CMD_SET_SERIAL 0xfa
#define DEBUG 0

#define Serial_Length 5

int enumerate_relay_boards(char *product);
int operate_relay(char *path,unsigned char relay, unsigned char state);
int set_serial(char *path,char *newserial);
char *board_path(char *serial);
int get_relay_board_count();
void shutdown();


typedef struct relay_board {
	//+1 for the \0 string terminator
	char serial[Serial_Length+1];
	unsigned char relay_count;
	unsigned char state;
	char *path;
} relay_board;

#endif
