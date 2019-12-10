Programming Assignment 2
ECE 574

Team Members:
Maclain Marsh
Heriberto De La Cruz
Zach Halvorsen

NetIDs:
mmarsh5
hdlc
halvorsen

Description:
The program parses the file into a netlist object which looks like this:

netlist:
    inputs
        name
        variableType (Int1, Int2, ...)
        objectType (input, output, register, wire)
    outputs
        name
        variableType
        objectType
    registers
        name
        variableType
        objectType
    wires
        name
        variableType
        objectType

    operations
        outputPtr
        inputPtr(s)
        operationType (ADD, SUB, MUL, ...)

Then it converts the netlist object into a synthesizable verilog file by walking
through the netlist object and the verilog module structure. Once the verilog
file has finished being output, the program walks through all of the inputs
to the netlist and all of the reg operations to find the critical path and
displays it for the user.

Responsibilities:
Maclain Marsh - Critical Path
Heriberto De La Cruz - Output to verilog
Zach Halvorsen - Netlist file parsing
