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

relay_board *relay_boards = 0;
int relay_board_count = 0;
int i, k;

//Private function prototypes
static int get_board_features(relay_board *board, hid_device *handle);
int known_relay(struct hid_device_info *thisdev);

/**
 * Enumerate all possible relay devices in the system
 */
int enumerate_relay_boards(const char *product,int verbose, int debug)
{

   int result = 0,relay = 0;
   struct hid_device_info *devs, *cur_dev;
   int num_opened = 0, num_error = 0;

 
   //Enumerate all HID USB devices 
   devs = hid_enumerate(0, 0);

   //Count the number of returned devices
   cur_dev = devs;

   while(cur_dev != NULL)
   {
      // Check if the HID device is a known relay else jump over it
      if ( ! known_relay(cur_dev) && cur_dev != NULL ) 
         cur_dev = cur_dev->next;
      else
         relay_board_count++;
      cur_dev = cur_dev->next;
   }
   fprintf(stderr,"Found %d devices\n", relay_board_count);

   //Allocate a buffer for the relays
   if (relay_board_count > 0)
   {
      relay_boards = calloc(relay_board_count, sizeof(relay_board));

      //Fill the relay structs
      cur_dev = devs;
      while (cur_dev != NULL) {
         // skip unknown HID devices
         relay_boards[relay].module_type = known_relay(cur_dev);
         if( relay_boards[relay].module_type ) 
         {
            
            //Save the path to this device
            relay_boards[relay].path = malloc(strlen(cur_dev->path)+1);
            memcpy(relay_boards[relay].path, cur_dev->path, strlen(cur_dev->path) + 1);

            // The product string is USBRelayx where x is number of relays read to the \0 in case there are more than 9
            relay_boards[relay].relay_count = atoi((const char *)&cur_dev->product_string[8]);
            // Ucreatefun relays do not have any information returned from the HID report
            // The USB serial is also fixed so this is copied to the module serial so that something can make the module unique
            if (relay_boards[relay].module_type == UCREATE)
            {
            relay_boards[relay].relay_count = 9; //No way of finding number of relays for these boards
            memset(relay_boards[relay].serial, 0x0, sizeof(relay_boards[relay].serial));
            wcstombs(relay_boards[relay].serial, cur_dev->serial_number , Serial_Length);
            // memcpy(relay_boards[relay].serial, cur_dev->serial_number, Serial_Length);
            }

            //Open it to get more details
            hid_device *handle;
            handle = hid_open_path(cur_dev->path);
            if (handle)
            {
	            num_opened++;
               result = get_board_features(&relay_boards[relay], handle);
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
               fprintf(stderr, "Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %s\n", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, relay_boards[relay].serial);
               fprintf(stderr, "  Manufacturer: %ls\n  Product:      %ls\n  Release:      %hx\n  Interface:    %d\n  Number of Relays = %d\n  Module_type = %d\n", 
                  cur_dev->manufacturer_string,cur_dev->product_string,cur_dev->release_number,cur_dev->interface_number,relay_boards[relay].relay_count,relay_boards[relay].module_type);


               //If verbose and debug are on, output individual relay details
               if (result != -1 && debug) 
               {
                  
                  for (k = 0; k < relay_boards[relay].relay_count; k++){
                     if ( relay_boards[relay].module_type == DCTTECH ) // No point printing state of Ucreatefun relays, not available
                     {
                        if (relay_boards[relay].state & 1 << k)
                        {
                           printf("%s_%d=1\n", relay_boards[relay].serial, k + 1);
                        }else{
                           printf("%s_%d=0\n", relay_boards[relay].serial, k + 1);
                        }
                     }
                  }
               }
            }
            relay++;
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
         if ( board->module_type == DCTTECH ){
            buf[0] = 0x0; //report number
            buf[1] = target_state;
            buf[2] = relay;
            buf[3] = 0x00;
            buf[4] = 0x00;
            buf[5] = 0x00;
            buf[6] = 0x00;
            buf[7] = 0x00;
            buf[8] = 0x00;
         } 
         if (board->module_type == UCREATE ) 
         {
            
            unsigned char ucreate;
            if (target_state == 0xff ) ucreate = 0xF0;
            else ucreate = 0x00;
            ucreate += relay;
            
            printf("target %x ucreate %x f0 %x\n\n",target_state,ucreate,0xF0);
            buf[0] = 0; //report number
            buf[1] = ucreate;
            buf[2] = 0x00;
            buf[3] = 0x00;
            buf[4] = 0x00;
            buf[5] = 0x00;
            buf[6] = 0x00;
            buf[7] = 0x00;
            buf[8] = 0x00;
         }
         res = hid_write(handle, buf, sizeof(buf));
      }
      else
      {
         res = -1;
      }

      if (res > 0  )
      {
         if (board->module_type == DCTTECH)
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
   char *respath = NULL;
   int isdevice = 0;
   if(strncmp(serial,"/dev/",5)== 0 )
   {
       respath = realpath(serial,NULL);
       
   }
   
   for (i = 0; i < relay_board_count; i++)
   {
      if (respath != NULL)
      {
         if (strcmp(relay_boards[i].path, respath)  == 0) isdevice = 1;
      }
      
      if ((strcmp(relay_boards[i].serial, serial) == 0)|| isdevice )
      {
         if (respath) free(respath);
         return &relay_boards[i];
      }
   }
   if (respath) free(respath);
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
   if ( board->module_type == DCTTECH ) {
      //Set the serial number (0x0 for termination)
      memset(board->serial, 0x0, sizeof(board->serial));
      memcpy(board->serial, buf, Serial_Length);

      //Byte 7 in the response contains the target_state of the relays
      board->state = buf[7];
      } else if(board->module_type == UCREATE ){
         return ret;
      }

   return ret;
}

// Function to check if the product is known and return the type
int known_relay(struct hid_device_info *thisdev) 
{
   char product[20];
   if ( thisdev == NULL ) return 0;
   sprintf(product,"%ls",thisdev->product_string);
   //       fprintf(stderr,"%s\n",product );
   if ( !strncmp( product,"USBRelay",8) ) return DCTTECH ;
   if ( !strncmp( product,"HIDRelay",8) ) return UCREATE;
   return 0;
}
