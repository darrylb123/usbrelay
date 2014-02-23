/* relay.h */

#define ON 0xff
#define OFF 0xfd

int operate_relay(hid_device *handle,unsigned char relay, unsigned char state);

struct  relay {
	char this_serial[10];
	unsigned char relay_num;
	unsigned char state;
	int found;
};
	
