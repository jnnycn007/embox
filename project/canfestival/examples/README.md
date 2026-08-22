# TestMasterSlave

Object dictionary files (`TestSlave.c`, `TestMaster.c`) are not stored in
the repository — they are generated from `.dcf` files using
`objdictgen.py`.

## Examples

All files required for each example (sources, `.dcf`, headers) are located
in the `examples/` directory of the CANFestival repository — each example
in its own subdirectory, e.g. `examples/TestMasterSlave/`.

## objdictgen

Located in the root of the CANFestival repository: `objdictgen/objdictgen.py`.

## Generating .c files

    python3 objdictgen/objdictgen.py examples/TestMasterSlave/TestSlave.dcf examples/TestMasterSlave/TestSlave.c
    python3 objdictgen/objdictgen.py examples/TestMasterSlave/TestMaster.dcf examples/TestMasterSlave/TestMaster.c

Run these commands from the root of the CANFestival repository.

## Running in Embox

Run only the slave node on the `can0` bus:

    TestMasterSlave -s can0 -M none

Run only the master node on the `can0` bus:

    TestMasterSlave -m can0 -S none

Run both nodes simultaneously (in a single process, on the same bus):

    TestMasterSlave -s can0 -m can0

The `-s`/`-m` flags set the bus for the slave/master respectively, while
`-S none`/`-M none` disable the corresponding side.
