/* control usb relay hid devices*/
/* ./relay SERIAL/relay_no/state SERIAL/relay_no/state ... */
#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <hidapi/hidapi.h>
#include "usbrelay.h"



int main( int argc, char *argv[]) {
   struct relay *relays = 0;
   unsigned char buf[9];// 1 extra byte for the report ID
   char arg_t[20] = {'\0'};
   int debug = 0;
   char *token;
   const char delimiters[] = "_=";
   int i;
   struct hid_device_info *devs, *cur_dev;
   hid_device *handle;
   int size = sizeof(struct relay);
   unsigned short vendor_id  = 0x16c0;
   unsigned short product_id = 0x05df;
   char *vendor, *product;

   /* allocate the memeory for all the relays */
   if (argc > 1) {
      relays = malloc(size*(argc+1)); /* Yeah, I know. Not using the first member */
      relays[0].this_serial[0] = '\0';
   } else debug = 1;

   /* loop through the command line and grab the relay details */
   for (i = 1 ; i < argc; i++ ) {
      /* copy the arg and bounds check */
      strncpy(arg_t,argv[i],19);
      arg_t[19] =  '\0';
      token = strtok(arg_t, delimiters);
      if (token != NULL) {
         strcpy(relays[i].this_serial,token);
      }
      token = strtok((char *)NULL, delimiters);
      if (token != NULL) {
         relays[i].relay_num = atoi(token);
      }
      token = strtok(NULL, delimiters); 
      if (token != NULL) {
         if (atoi(token)) {
             relays[i].state = ON;
         } else {
             relays[i].state = OFF;
         }
      }
      printf("Orig: %s, Serial: %s, Relay: %d State: %x\n",arg_t,relays[i].this_serial,relays[i].relay_num,relays[i].state);
      relays[i].found = 0; 
   }

   product = getenv("USBID");
   if (product != NULL) {
      vendor = strsep(&product, ":");
      if (vendor && *vendor) {
         vendor_id = strtol(vendor, NULL, 16);
      }
      if (product && *product) {
         product_id = strtol(product, NULL, 16);
      }
   }
   devs = hid_enumerate(vendor_id, product_id);

   cur_dev = devs;
   while (cur_dev) {
      fprintf(stderr,"Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
      fprintf(stderr,"\n");
      fprintf(stderr,"  Manufacturer: %ls\n", cur_dev->manufacturer_string);
      fprintf(stderr,"  Product:      %ls\n", cur_dev->product_string);
      fprintf(stderr,"  Release:      %hx\n", cur_dev->release_number);
      fprintf(stderr,"  Interface:    %d\n",  cur_dev->interface_number);

      handle = hid_open_path(cur_dev->path);
      if (!handle) {
         fprintf(stderr,"unable to open device\n");
         return 1;
      }
      buf[0] = 0x01;
      int ret = hid_get_feature_report(handle,buf,sizeof(buf));
      if (ret == -1) {
              perror("hid_get_feature_report");
              exit(1);
      }


      if (debug) {
         if (buf[7] == 0) printf("%s_1=0\n%s_2=0\n",buf,buf );
         else if (buf[7] == 1) printf("%s_1=1\n%s_2=0\n",buf,buf);
         else if (buf[7] == 2) printf("%s_1=0\n%s_2=1\n",buf,buf);
         else if (buf[7] == 3) printf("%s_1=1\n%s_2=1\n",buf,buf);
         /* fprintf(stderr,"Usage: %s %s_1=0  %s\n",argv[0],buf); */
      }

      /* loop through the supplied command line and try to match the serial */
      for (i=1;i<argc;i++) {
         printf("Serial: %s, Relay: %d State: %x \n",relays[i].this_serial,relays[i].relay_num,relays[i].state);
         if (!strcmp(relays[i].this_serial, (const char *) buf)) {
            printf("%d HID Serial: %s ", i, buf);
            printf("Serial: %s, Relay: %d State: %x\n",relays[i].this_serial,relays[i].relay_num,relays[i].state);
            operate_relay(handle,relays[i].relay_num,relays[i].state);
            relays[i].found = 1;
         }
      }
      hid_close(handle);
      printf("\n");
      cur_dev = cur_dev->next;
   }
   hid_free_enumeration(devs);

   /* Free static HIDAPI objects. */
   hid_exit();

   for (i=1;i<argc;i++){
      printf("Serial: %s, Relay: %d State: %x ",relays[i].this_serial,relays[i].relay_num,relays[i].state);
      if (relays[i].found )
         printf("--- Found\n");
      else
         printf("--- Not Found\n");
   }

   if (relays)
      free(relays);
   exit(0);
}

int operate_relay(hid_device *handle,unsigned char relay, unsigned char state){
   unsigned char buf[9];// 1 extra byte for the report ID
   int res;

   buf[0] = 0x0; //report number
   buf[1] = state;
   buf[2] = relay;
   buf[3] = 0x00;
   buf[4] = 0x00;
   buf[5] = 0x00;
   buf[6] = 0x00;
   buf[7] = 0x00;
   buf[8] = 0x00;
   res = hid_write(handle, buf, sizeof(buf));
   if (res < 0) {
      printf("Unable to write()\n");
      printf("Error: %ls\n", hid_error(handle));
   }
   return(res);
}
