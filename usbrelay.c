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
#include "usbrelay.h"

int main(int argc, char *argv[])
{
    struct relay *relays = 0;
    char arg_t[20] = {'\0'};
    int debug = 0;
    char *token;
    const char delimiters[] = "_=";
    int i;
    int size = sizeof(struct relay);
    int exit_code = 0;

    /* allocate the memory for all the relays */
    if (argc > 1)
    {
        relays = malloc(size * (argc + 1)); /* Yeah, I know. Not using the first member */
        relays[0].this_serial[0] = '\0';
    }
    else
        debug = 1;

    /* loop through the command line and grab the relay details */
    for (i = 1; i < argc; i++)
    {
        /* copy the arg and bounds check */
        strncpy(arg_t, argv[i], 19);
        arg_t[19] = '\0';
        token = strtok(arg_t, delimiters);
        if (token != NULL)
        {
            strcpy(relays[i].this_serial, token);
        }
        token = strtok((char *)NULL, delimiters);
        if (token != NULL)
        {
            relays[i].relay_num = atoi(token);
        }
        token = strtok(NULL, delimiters);
        if (relays[i].relay_num == 0)
        { /* command to change the serial - remaining token is the new serial */
            if (token != NULL)
            {
                strcpy(relays[i].new_serial, token);
            }
        }
        else
        {
            if (token != NULL)
            {
                if (atoi(token))
                {
                    relays[i].state = CMD_ON;
                }
                else
                {
                    relays[i].state = CMD_OFF;
                }
            }
        }
        fprintf(stderr, "Orig: %s, Serial: %s, Relay: %d State: %x\n", arg_t, relays[i].this_serial, relays[i].relay_num, relays[i].state);
        relays[i].found = 0;
    }

    //Locate and identify attached relay boards
    enumerate_relay_boards(getenv("USBID"), 1, debug);

    /* loop through the supplied command line and try to match the serial */
    for (i = 1; i < argc; i++)
    {
        fprintf(stderr, "Serial: %s, Relay: %d State: %x \n", relays[i].this_serial, relays[i].relay_num, relays[i].state);
        relay_board *board = find_board(relays[i].this_serial);

        if (board)
        {
            fprintf(stderr, "%d HID Serial: %s ", i, board->serial);
            if (relays[i].relay_num == 0)
            {
                if (!relays[i].new_serial[0])
                {
                    fprintf(stderr, "\n \n New serial can't be empty!\n");
                }
                else
                {
                    set_serial(board->serial, relays[i].new_serial);
                }
            }
            else
            {
                fprintf(stderr, "Serial: %s, Relay: %d State: %x\n", relays[i].this_serial, relays[i].relay_num, relays[i].state);
                if (operate_relay(board->serial, relays[i].relay_num, relays[i].state) < 0)
                    exit_code++;
                relays[i].found = 1;
            }
        }
        fprintf(stderr, "\n");
    }

    shutdown();

    for (i = 1; i < argc; i++)
    {
        fprintf(stderr, "Serial: %s, Relay: %d State: %x ", relays[i].this_serial, relays[i].relay_num, relays[i].state);
        if (relays[i].found)
            fprintf(stderr, "--- Found\n");
        else
        {
            fprintf(stderr, "--- Not Found\n");
            exit_code++;
        }
    }

    if (relays)
        free(relays);
    exit(exit_code);
}
