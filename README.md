Class for handling arbitrary precision integers.
================================================

Integer is stored in the array of bytes represented as base 256 number.
They are signed by default.

Class can be created from:
* 32 and 64 bit ints;
* byte arrays, containing base 256 number;
* character strings, intepreted as base 10 or base 16 numbers

Supported operations:
* addition / substraction (positive and negative numbers)
* multiplication
* comparison

