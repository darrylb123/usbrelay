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
#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <hidapi/hidapi.h>
#include "libusbrelay.h"

int main(int argc, char *argv[])
{
    char arg_t[20] = {'\0'};
    char *token;
    const char delimiters[] = "_=";
    int i;
    int exit_code = 0;

    //Locate and identify attached relay boards
    enumerate_relay_boards(getenv("USBID"));

    /* loop through the command line and process the relay details given */
    for (i = 1; i < argc; i++)
    {
        /* copy the arg and bounds check */
        //+1 for the \0 string terminator
        char target_serial[Serial_Length + 1];
        int target_relay_num = 0;
        int target_state = 0;

        strncpy(arg_t, argv[i], 19);
        arg_t[19] = '\0';
        token = strtok(arg_t, delimiters);
        if (token != NULL)
        {
            strcpy(target_serial, token);
        }
        token = strtok((char *)NULL, delimiters);
        if (token != NULL)
        {
            target_relay_num = atoi(token);
        }
        token = strtok(NULL, delimiters);
        if (target_relay_num == 0)
        { /* command to change the serial - remaining token is the new serial */
            if (token != NULL)
            {
                char *path = board_path(target_serial);
                if (path != NULL)
                {
                    set_serial(path, token);
                }
            }
        }
        else
        {
            if (token != NULL)
            {

                if (atoi(token))
                {
                    target_state = ON;
                }
                else
                {
                    target_state = OFF;
                }
                char *path = board_path(target_serial);
                if (path != NULL)
                {
                    if (operate_relay(path, target_relay_num, target_state) < 0)
                    {
                        exit_code++;
                    }
                }
            }
        }
#if DEBUG == 1
        fprintf(stderr, "Serial: %s, Relay: %d State: %x ", target_serial, target_relay_num, target_state);
#endif
    }
    shutdown();

    exit(exit_code);
}
