# instrgrind

instrgrind is a valgrind plugin to count the number of times an instruction
is executed during a program run.

## Building

To build instrgrind, first download and unpack valgrind (3.16.0):

    $ wget -qO - https://sourceware.org/pub/valgrind/valgrind-3.16.0.tar.bz2 | tar jxv

Then, enter directory and clone instrgrind github repository.
Apply a patch to add the tool in the compilation chain.

    $ cd valgrind-3.16.0
    $ git clone https://github.com/rimsa/instrgrind.git instrgrind
    $ patch -p1 < instrgrind/instrgrind.patch

Build valgrind with instrgrind:

    $ ./autogen.sh
    $ ./configure
    $ make -j4
    $ sudo make install

## Testing

Compile and use a test program that orders numbers given in the argument's list:

    $ cd instrgrind/tests
    $ gcc test.c
    $ valgrind -q --tool=instrgrind --instrs-outfile=test.out ./a.out 15 4 8 16 42 23
    4 8 15 16 23 42

The output file contains each executed instruction in a line.
Each line has 3 columns, divided by colons: the instruction address, size and execution count.

    $ head test.out
    0x100344050:5:1
    0x100344055:2:1
    0x100344057:4:1
    ...

The instruction at address 0x100344050 has 5 bytes and was executed only once for this run.
