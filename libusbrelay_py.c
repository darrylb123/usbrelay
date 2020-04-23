/*
libusbrelay_py: Control USB HID connected electrical relay modules
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
#include "libusbrelay.h"
#include "Python.h"

//Static function declarations
static PyObject *usbrelay_init(PyObject *self, PyObject *args);
static PyObject *usbrelay_board_count(PyObject *self, PyObject *args);
static PyObject *usbrelay_board_details(PyObject *self, PyObject *args);
static PyObject *usbrelay_board_control(PyObject *self, PyObject *args);

//Required because this won't be used here, that's intentional: Python uses it
// #pragma GCC diagnostic push
// #pragma GCC diagnostic ignored "-Wunused-variable"
// #pragma GCC diagnostic pop

static PyMethodDef UsbRelayMethods[] = {
    {"init", usbrelay_init, METH_VARARGS,
     "Initialize the module and enumerate relay boards"},
    {"board_count", usbrelay_board_count, METH_VARARGS,
     "Return the count of enumerated boards"},
    {"board_details", usbrelay_board_details, METH_VARARGS,
     "Return details on the enumerated boards"},
    {"board_control", usbrelay_board_control, METH_VARARGS,
     "Control a relay on a specific board"},
    {NULL, NULL, 0, NULL} /* Sentinel */
};

static struct PyModuleDef usb_relaymodule = {
    PyModuleDef_HEAD_INIT,
    "usbrelay_py", /* name of module */
    NULL,          /* module documentation, may be NULL */
    -1,            /* size of per-interpreter state of the module,
                or -1 if the module keeps state in global variables. */
    UsbRelayMethods};

PyMODINIT_FUNC PyInit_usbrelay_py(void)
{
    //Attempt to enumerate the relays while we're here
    // int result = -1;
    // result = enumerate_relay_boards(NULL,0,0);

    //Give Python our API
    return PyModule_Create(&usb_relaymodule);
}

// /**
//  * Initialize the usb_relay system with an optional product string
//  * Including enumerating the attached boards
//  */
static PyObject *usbrelay_init(PyObject *self, PyObject *args)
{
    const char *product = NULL;
    int result = -1;

    //If there is no argument, product will remain null
    //Which is fine
    PyArg_ParseTuple(args, "s", &product);
    result = enumerate_relay_boards(product,0,0);

    return Py_BuildValue("i", result);
}

/**
 * Retrieve the count of attached relay boards
 */
static PyObject *usbrelay_board_count(PyObject *self, PyObject *args)
{
    int board_count = 0;
    board_count = get_relay_board_count();

    //Just in case they didn't initialize, we'll try once to get a different result
    if (board_count == 0)
    {
        enumerate_relay_boards(NULL,0,0);
        board_count = get_relay_board_count();
    }

    return Py_BuildValue("i", board_count);
}

/**
 * Retrieve the names of attached boards and their relay counts and statuses
 */
static PyObject *usbrelay_board_details(PyObject *self, PyObject *args)
{
    PyObject *result;
    PyObject **boards;
    int board_count = get_relay_board_count();
    relay_board *relay_boards = get_relay_boards();

    boards = malloc(sizeof(PyObject *) * board_count);

    //Pack up our board information into Python Typles
    for (int i = 0; i < board_count; i++)
    {
        boards[i] = Py_BuildValue("(sii)", relay_boards[i].serial, relay_boards[i].relay_count, relay_boards[i].state);
    }

    //This is not the ideal way to do this, but hand-brewing a va_list is generally considered "bad" and not portable
    switch (board_count)
    {
    default:
        result = Py_BuildValue("()");
        break;
    case 1:
        result = Py_BuildValue("(O)", boards[0]);
        break;
    case 2:
        result = Py_BuildValue("(OO)", boards[0], boards[1]);
        break;
    case 3:
        result = Py_BuildValue("(OOO)", boards[0], boards[1], boards[2]);
        break;
    case 4:
        result = Py_BuildValue("(OOOO)", boards[0], boards[1], boards[2], boards[3]);
        break;
    case 5:
        result = Py_BuildValue("(OOOOO)", boards[0], boards[1], boards[2], boards[3], boards[4]);
        break;
    case 6:
        result = Py_BuildValue("(OOOOOO)", boards[0], boards[1], boards[2], boards[3], boards[4],
                               boards[5]);
        break;
    case 7:
        result = Py_BuildValue("(OOOOOOO)", boards[0], boards[1], boards[2], boards[3], boards[4],
                               boards[5], boards[6]);
        break;
    case 8:
        result = Py_BuildValue("(OOOOOOOO)", boards[0], boards[1], boards[2], boards[3], boards[4],
                               boards[5], boards[6], boards[7]);
        break;
    case 9:
        result = Py_BuildValue("(OOOOOOOOO)", boards[0], boards[1], boards[2], boards[3], boards[4],
                               boards[5], boards[6], boards[7], boards[8]);
        break;
    case 10:
        result = Py_BuildValue("(OOOOOOOOOO)", boards[0], boards[1], boards[2], boards[3], boards[4],
                               boards[5], boards[6], boards[7], boards[8], boards[9]);
        break;
    }

    free(boards);
    return result;
}

/**
 * Set a relay status
 */
static PyObject *usbrelay_board_control(PyObject *self, PyObject *args)
{
    const char *serial;
    int relay;
    int status;
    int result = -1;

    //If there is no argument, or it's ill formed, we just refuse to do anything
    if (PyArg_ParseTuple(args, "sii", &serial, &relay, &status))
    {
        char target_state = CMD_OFF;
        if (status)
        {
            target_state = CMD_ON;
        }

        result = operate_relay(serial, relay, target_state,0);
    }

    return Py_BuildValue("i", result);
}
