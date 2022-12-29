"""
usbrelay_py

Python bindings to the usbrelay command line tool
"""

"""
Control usb connected relays with ID 16c0:05df (Van Ooijen Technische Informatica HID device except mice, keyboards, and joysticks)

Module Init
import usbrelay_py

Module Methods:
usbrelay_board_count() returns the number of usbrelay boards found

usbrelay_board_details() returns a set of Python tuples  (SERIAL of Relay Module,Number of RELAYS,Integer State of RELAYS)

usbrelay_board_control('SERIAL',Relay number, Desired State)
eg usbrelay_board_control('ABCDE',2,0)
"""

__version__ = "1.0"
__author__ = "Sean Mollet"
