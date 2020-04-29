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
#include <argp.h>
#include "libusbrelay.h"
#include "usbrelay.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

static void argp_print_version(FILE *stream, struct argp_state *state)
{
	fprintf(stream, "usbrelay %s\n", gitversion);
}

void (*argp_program_version_hook)(FILE *stream, struct argp_state *state) = argp_print_version;

const char *argp_program_bug_address =
	"https://github.com/darrylb123/usbrelay/issues";

/* Program documentation. */
static char doc[] =
	"Control or query USB HID relays."

	/* This part of the documentation comes after the options */
	"\vWithout ACTION, the actual state of all relays is printed to stdout.\n"
	"ACTION can be one of:\n"
	"RELID_N=[0|1] to switch the N-th relay off or on\n"
	"RELID=NEWID to change relay ID\n"
	;

/* A description of the arguments we accept. */
static char args_doc[] = "[ACTION...]";

/* The options we understand. */
static struct argp_option options[] = {
	{"debug",    'd', 0,       0, "Produce debugging output" },
	{"quiet",    'q', 0,       0, "Be quiet" },
	{ 0 }
};

/* Used by ‘main’ to communicate with ‘parse_opt’. */
struct arguments
{
	int debug;
	int verbose;
};

/* Parse a single option. */
static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
	/* Get the ‘input’ argument from ‘argp_parse’, which we
	   know is a pointer to our arguments structure. */
	struct arguments *args = state->input;

	switch (key) {
	case 'd':
		args->debug = 1;
		break;
	case 'q': case 's':
		args->verbose = 0;
		break;

	case ARGP_KEY_NO_ARGS:
		break;

	default:
		return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

/* Our argp parser. */
static struct argp argp = { options, parse_opt, args_doc, doc };



int main(int argc, char *argv[])
{
	struct relay *relays = 0;
	int i;
	int exit_code = 0;
	struct arguments args = {
		.debug = 0,
		.verbose = 1,
	};

       /* Parse our arguments; every option seen by ‘parse_opt’ will
          be reflected in ‘args’. */
       argp_parse (&argp, argc, argv, 0, &optind, &args);

	/* allocate the memory for all the relays */
	if ((argc - optind) >= 1) {
		relays = calloc(argc - optind + 1, sizeof(struct relay));	/* Yeah, I know. Not using the first member */
		/* relays is zero-initialized */
	} 
	/* loop through the command line and grab the relay details */
	for (i = 0 ; i < (argc - optind); i++) {
		char *arg = argv[i + optind];
		struct relay *relay = &relays[i];

		char *underscore = strchr(arg, '_');
		char *equal_sign = strchr(arg, '=');

		if (underscore && equal_sign && underscore > equal_sign) {	/* e.g. ASDFG=QWERT_1 */
			fprintf(stderr, "Invalid relay specification: %s\n",
				argv[i + optind]);
			exit(1);
		}

		size_t size;
		if (underscore)
			size = underscore - arg;
		else if (equal_sign)
			size = equal_sign - arg;
		else
			size = strlen(arg);

		relay->this_serial = malloc(size + 1);
		strncpy(relay->this_serial, arg, size);

		/* Parse relay number */
		if (underscore)
			relay->relay_num = atoi(underscore + 1);

		if (equal_sign) {
			if (relay->relay_num == 0) {	/* command to change the serial - remaining token is the new serial */
				strncpy(relay->new_serial, equal_sign + 1,
					sizeof(relay->new_serial) - 1);
			} else {
				if (atoi(equal_sign + 1)) {
					relays[i].state = CMD_ON;
				} else {
					relays[i].state = CMD_OFF;
				}
			}
		}
		if (args.debug) {
			fprintf(stderr, "Orig: %s, Serial: %s, Relay: %d State: %x\n",
				arg, relay->this_serial, 
				relay->relay_num,
				relay->state);
			relay->found = 0;
		}
	}

	//Locate and identify attached relay boards
	if(args.debug) fprintf(stderr,"Version: %s\n",gitversion);
	enumerate_relay_boards(getenv("USBID"), args.verbose, args.debug);

	/* loop through the supplied command line and try to match the serial */
	for (i = 0; i < (argc - optind); i++) {
		if (args.debug ) {
			fprintf(stderr, "main() arg %d Serial: %s, Relay: %d State: %x \n",
				i,relays[i].this_serial, 
				relays[i].relay_num,
				relays[i].state);
		}
		relay_board *board = find_board(relays[i].this_serial, args.debug );

		if (board) {
			if (args.debug == 2)
				fprintf(stderr, "%d HID Serial: %s ", i, board->serial);
			if (relays[i].relay_num == 0) {
				if (!relays[i].new_serial[0]) {
					fprintf(stderr, "\n \n New serial can't be empty!\n");
				} else {
					fprintf(stderr, "Setting new serial\n");
					set_serial(board->serial,
						   relays[i].new_serial,args.debug);
				}
			} else {
				if (args.debug)
					fprintf(stderr,
						"main() operate: %s, Relay: %d State: %x\n",
						relays[i].this_serial,
						relays[i].relay_num,
						relays[i].state);
				if (operate_relay(relays[i].this_serial, relays[i].relay_num,relays[i].state,args.debug) < 0)
					exit_code++;
				relays[i].found = 1;
			}
		}
	}

	shutdown();

	for (i = 1; i < (argc - optind); i++) {
		if (args.debug)
			fprintf(stderr,"Serial: %s, Relay: %d State: %x ",
				relays[i].this_serial, relays[i].relay_num,
				relays[i].state);
		if (relays[i].found) {
			if (args.debug)
				fprintf(stderr, "--- Found\n");
		} else {
			if (args.debug)
				fprintf(stderr, "--- Not Found\n");
			exit_code++;
		}
	}
	if (relays) {
		for (i = 1; i < (argc - optind) ; i++) {
			free(relays[i].this_serial);
		}
		free(relays);
	}

	exit(exit_code);
}
