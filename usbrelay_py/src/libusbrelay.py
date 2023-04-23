
from ctypes import *

try:
    usbrly = CDLL("libusbrelay.so")
except Exception as e:
    print("")
    print("ERROR: libusbrelay.so not found, please install usbrelay package")
    print("")
    raise
#print("Successfully loaded ", usbrly)

# state
usbrly.CMD_ON         = 0xff
usbrly.CMD_OFF        = 0xfd
usbrly.CMD_SET_SERIAL = 0xfa

# module_type
usbrly.DCTTECH        = 1
usbrly.UCREATE        = 2

usbrly.verbose = 0
usbrly.debug   = 0

class relay_board_t(Structure):
    _fields_ = [
      ("serial",      c_char * 6),
      ("relay_count", c_byte),
      ("state",       c_byte),
      ("path",        c_char_p),
      ("module_type", c_int),
    ]
    _pack_ = 1

# int enumerate_relay_boards(const char *product, int verbose, int debug);
usbrly.enumerate_relay_boards.argtypes = c_char_p, c_int, c_int
usbrly.enumerate_relay_boards.restype = c_int

# int operate_relay(const char *path, unsigned char relay, unsigned char state, int debug);
usbrly.operate_relay.argtypes = c_char_p, c_ubyte, c_ubyte, c_int
usbrly.operate_relay.restype = c_int

# int set_serial(const char *path, char *newserial, int debug);
usbrly.set_serial.argtypes = c_char_p, c_char_p, c_int
usbrly.set_serial.restype = c_int

# relay_board *find_board(const char *serial, int debug);
usbrly.find_board.argtypes = c_char_p, c_int
usbrly.find_board.restype = POINTER(relay_board_t)

# int get_relay_board_count(void);
usbrly.get_relay_board_count.argtypes = None
usbrly.get_relay_board_count.restype = c_int

# relay_board* get_relay_boards(void);
usbrly.get_relay_boards.argtypes = None
usbrly.get_relay_boards.restype = POINTER(relay_board_t)

# void shutdown(void);
usbrly.shutdown.argtypes = None
usbrly.shutdown.restype = None

try:
    # const char *libusbrelay_version(void);
    usbrly.libusbrelay_version.argtypes = None
    usbrly.libusbrelay_version.restype = c_char_p
except:
    pass

def init(product=None):
    # pass a NULL pointer for empty string
    product = str.encode(product) if product else None
    return usbrly.enumerate_relay_boards(product, usbrly.verbose, usbrly.debug)

def board_count():
    init()
    return usbrly.get_relay_board_count()

def board_details():
    counts = board_count()
    relay_boards = usbrly.get_relay_boards()
    address = addressof(relay_boards) if relay_boards else 0
    boards = []
    for i in range(0, counts):
        board = POINTER(relay_board_t).from_address(address).contents
        boards.append((board.serial.decode("utf-8"), board.relay_count, board.state,
                       board.path.decode("utf-8"), board.module_type,))
        address = address + sizeof(relay_board_t)
    return boards

def board_control(serial, relay=0, status=0):
    target_state = usbrly.CMD_ON if status else usbrly.CMD_OFF
    return usbrly.operate_relay(str.encode(serial), relay, target_state, usbrly.debug)
