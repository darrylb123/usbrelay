/*
usbrelay: Control USB HID connected electrical relay modules
Copyright (C) 2014  Darryl Bond

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
#include "usbrelay.h"

unsigned short vendor_id = 0x16c0;
unsigned short product_id = 0x05df;
relay_board *relay_boards = 0;
int relay_board_count = 0;

int enumerate_relay_boards(char *product)
{
   char *vendor;
   unsigned char buf[9];
   int result = 0;
   struct hid_device_info *devs, *cur_dev;

   //If we were given a product code, use it
   if (product != NULL)
   {
      vendor = strsep(&product, ":");
      if (vendor && *vendor)
      {
         vendor_id = strtol(vendor, NULL, 16);
      }
      if (product && *product)
      {
         product_id = strtol(product, NULL, 16);
      }
   }

   //Enumerate USB devices with the given IDs
   devs = hid_enumerate(vendor_id, product_id);

   //Count the number of returned devices
   cur_dev = devs;

   for (relay_board_count = 0; cur_dev != NULL; relay_board_count++)
   {
      cur_dev = cur_dev->next;
   }

   //Allocate a buffer for the relays
   relay_boards = malloc((relay_board_count) * sizeof(relay_board));

   //Fill the relay structs
   cur_dev = devs;
   for (int i = 0; i < relay_board_count; i++)
   {

      //Save the path to this device
      relay_boards[i].path = malloc(strlen(cur_dev->path));
      memcpy(relay_boards[i].path, cur_dev->path, strlen(cur_dev->path) + 1);

      // The product string is USBRelayx where x is number of relays read to the \0 in case there are more than 9
      relay_boards[i].relay_count = atoi((const char *)&cur_dev->product_string[8]);

      //Open it to get more details
      hid_device *handle;
      handle = hid_open_path(cur_dev->path);
      if (!handle)
      {
         perror("unable to open device\n");
         result = -1;
         break;
      }

      //Get the features of the device
      buf[0] = 0x01;
      int ret = hid_get_feature_report(handle, buf, sizeof(buf));
      if (ret == -1)
      {
         perror("hid_get_feature_report\n");
         result = -1;
         break;
      }

      //Set the serial number (0x0 for termination)
      memset(relay_boards[i].serial, 0x0, sizeof(relay_boards[i].serial));
      memcpy(relay_boards[i].serial, buf, Serial_Length);

      //Byte 7 in the response contains the target_state of the relays
      relay_boards[i].state = buf[7];

#if DEBUG == 1
      fprintf(stderr, "Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
      fprintf(stderr, "\n");
      fprintf(stderr, "  Manufacturer: %ls\n", cur_dev->manufacturer_string);
      fprintf(stderr, "  Product:      %ls\n", cur_dev->product_string);
      fprintf(stderr, "  Release:      %hx\n", cur_dev->release_number);
      fprintf(stderr, "  Interface:    %d\n", cur_dev->interface_number);

      // The product string is USBRelayx where x is number of relays read to the \0 in case there are more than 9
      fprintf(stderr, "  Number of Relays = %d\n", relay_boards[i].relay_count);

      //Print the individual relay status
      for (int j = 0; j < relay_boards[i].relay_count; j++)
      {
         if (relay_boards[i].state & 1 << j)
         {
            printf("%s_%d=1\n", relay_boards[i].serial, j + 1);
         }
         else
         {
            printf("%s_%d=0\n", relay_boards[i].serial, j + 1);
         }
      }
#endif
      hid_close(handle);
      cur_dev = cur_dev->next;
   }

   hid_free_enumeration(devs);
   return result;
}

int main(int argc, char *argv[])
{
   char arg_t[20] = {'\0'};
   char *token;
   const char delimiters[] = "_=";
   int i;
   int exit_code=0;

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

   /* Free static HIDAPI objects. */
   hid_exit();

   for (int i = 0; i < relay_board_count; i++)
   {
      free(relay_boards[i].path);
   }
   if (relay_board_count > 0)
   {
      free(relay_boards);
   }

   exit(exit_code);
}

int operate_relay(char *path, unsigned char relay, unsigned char target_state)
{
   unsigned char buf[9]; // 1 extra byte for the report ID
   int res;
   hid_device *handle;

   handle = hid_open_path(path);

   if (handle)
   {
      buf[0] = 0x0; //report number
      buf[1] = target_state;
      buf[2] = relay;
      buf[3] = 0x00;
      buf[4] = 0x00;
      buf[5] = 0x00;
      buf[6] = 0x00;
      buf[7] = 0x00;
      buf[8] = 0x00;
      res = hid_write(handle, buf, sizeof(buf));
   }
   else
   {
      res = -1;
   }

   if (res < 0)
   {
      fprintf(stderr, "Unable to write()\n");
      fprintf(stderr, "Error: %ls\n", hid_error(handle));
   }

   hid_close(handle);
   return (res);
}

int set_serial(char *path, char *newserial)
{
   unsigned char buf[9]; // 1 extra byte for the report ID
   int res;
   hid_device *handle;

   handle = hid_open_path(path);

   if (handle)
   {
      buf[0] = 0x0; //report number
      buf[1] = CMD_SET_SERIAL;
      buf[2] = newserial[0];
      buf[3] = newserial[1];
      buf[4] = newserial[2];
      buf[5] = newserial[3];
      buf[6] = newserial[4];
      buf[7] = 0x00;
      buf[8] = 0x00;
      res = hid_write(handle, buf, sizeof(buf));
   }
   else
   {
      res = -1;
   }

   if (res < 0)
   {
      fprintf(stderr, "Unable to write()\n");
      fprintf(stderr, "Error: %ls\n", hid_error(handle));
   }

   hid_close(handle);
   return (res);
}

/**
 * Find a board path given a relay board serial
 */
char *board_path(char *serial)
{
   for (int i = 0; i < relay_board_count; i++)
   {
      if (strcmp(relay_boards[i].serial, serial) == 0)
      {
         return relay_boards[i].path;
      }
   }
   return NULL;
}
