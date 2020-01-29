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
#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <hidapi/hidapi.h>
#include "libusbrelay.h"

//Global variables
unsigned short vendor_id = 0x16c0;
unsigned short product_id = 0x05df;
relay_board *relay_boards = 0;
int relay_board_count = 0;
int i, k;

//Private function prototypes
static int get_board_features(relay_board *board, hid_device *handle);

/**
 * Enumerate all possible relay devices in the system
 */
int enumerate_relay_boards(const char *product, int verbose, int debug)
{
   char *vendor;
   int result = 0;
   struct hid_device_info *devs, *cur_dev;
   int num_opened = 0, num_error = 0;

   //If we were given a product code, use it
   if (product != NULL)
   {
      vendor = strsep((char **)&product, ":");
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
   if (relay_board_count > 0)
   {
      relay_boards = calloc(relay_board_count, sizeof(relay_board));

      //Fill the relay structs
      cur_dev = devs;
      for (i = 0; i < relay_board_count; i++)
      {
         //Save the path to this device
         relay_boards[i].path = malloc(strlen(cur_dev->path)+1);
         memcpy(relay_boards[i].path, cur_dev->path, strlen(cur_dev->path) + 1);

         // The product string is USBRelayx where x is number of relays read to the \0 in case there are more than 9
         relay_boards[i].relay_count = atoi((const char *)&cur_dev->product_string[8]);

         //Open it to get more details
         hid_device *handle;
         handle = hid_open_path(cur_dev->path);
         if (handle)
         {
	    num_opened++;
            result = get_board_features(&relay_boards[i], handle);
            hid_close(handle);
         }
         else
         {
	    num_error++;
            perror(cur_dev->path);
            result = -1;
         }

         //Output the device enumeration details if verbose is on
         if (result != -1 && verbose)
         {
            fprintf(stderr, "Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
            fprintf(stderr, "\n");
            fprintf(stderr, "  Manufacturer: %ls\n", cur_dev->manufacturer_string);
            fprintf(stderr, "  Product:      %ls\n", cur_dev->product_string);
            fprintf(stderr, "  Release:      %hx\n", cur_dev->release_number);
            fprintf(stderr, "  Interface:    %d\n", cur_dev->interface_number);

            fprintf(stderr, "  Number of Relays = %d\n", relay_boards[i].relay_count);

            //If verbose and debug are on, output individual relay details
            if (result != -1 && debug)
            {
               for (k = 0; k < relay_boards[i].relay_count; k++)
               {
                  if (relay_boards[i].state & 1 << k)
                  {
                     printf("%s_%d=1\n", relay_boards[i].serial, k + 1);
                  }
                  else
                  {
                     printf("%s_%d=0\n", relay_boards[i].serial, k + 1);
                  }
               }
            }
         }
         cur_dev = cur_dev->next;
      }
   }
   hid_free_enumeration(devs);
   if (num_opened == 0 && num_error > 0)
      fprintf(stderr, "Unable to open any device - Use root, sudo or set the device permissions via udev\n");
   return result;
}

/**
 * Command a relay at a particular /dev path to switch to a given state
 */
int operate_relay(const char *serial, unsigned char relay, unsigned char target_state)
{
   unsigned char buf[9]; // 1 extra byte for the report ID
   int res = -1;
   hid_device *handle;

   relay_board *board = find_board(serial);
   if (board != NULL && relay >0 && relay <= board->relay_count)
   {
      handle = hid_open_path(board->path);

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

      if (res > 0)
      {
         //Update our relay status
         res = get_board_features(board, handle);
      }
      else
      {
         fprintf(stderr, "Unable to write()\n");
         fprintf(stderr, "Error: %ls\n", hid_error(handle));
      }

      hid_close(handle);
   }

   return (res);
}

int set_serial(const char *serial, char *newserial)
{
   unsigned char buf[9]; // 1 extra byte for the report ID
   int res = -1;
   hid_device *handle;

   relay_board *board = find_board(serial);
   if (board != NULL)
   {
      handle = hid_open_path(board->path);

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

      if (res > 0)
      {
         //Update our copy of the serial number
         res = get_board_features(board, handle);
      }
      else
      {
         fprintf(stderr, "Unable to write()\n");
         fprintf(stderr, "Error: %ls\n", hid_error(handle));
      }
      hid_close(handle);
   }

   return (res);
}

/**
 * Find a board path given a relay board serial
 */
relay_board *find_board(const char *serial)
{
   for (i = 0; i < relay_board_count; i++)
   {
      if (strcmp(relay_boards[i].serial, serial) == 0)
      {
         return &relay_boards[i];
      }
   }
   return NULL;
}

/**
 * Return the count of relay boards
 */
int get_relay_board_count()
{
   return relay_board_count;
}

/** 
 * Return the actual relay_board structs
 */
relay_board *get_relay_boards()
{
   return relay_boards;
}

/**
 * Return all allocated resources and perform any other cleanup
 */
void shutdown()
{

   /* Free static HIDAPI objects. */
   hid_exit();

   for (i = 0; i < relay_board_count; i++)
   {
      free(relay_boards[i].path);
   }
   if (relay_board_count > 0)
   {
      free(relay_boards);
   }
}

//Private functions

/**
 * Load the board serial and relay status
 * This intentionally reuses a handle as it's meant to be called by the
 * other functions while they have one already open
 */
static int get_board_features(relay_board *board, hid_device *handle)
{
   unsigned char buf[9] = { 0 };
   //Get the features of the device
   buf[0] = 0x01;
   int ret = hid_get_feature_report(handle, buf, sizeof(buf));
   if (ret == -1)
   {
      perror("hid_get_feature_report\n");
   }

   //Set the serial number (0x0 for termination)
   memset(board->serial, 0x0, sizeof(board->serial));
   memcpy(board->serial, buf, Serial_Length);

   //Byte 7 in the response contains the target_state of the relays
   board->state = buf[7];

   return ret;
}
