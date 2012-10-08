/*

Copyright (c) 2012, Vitaly Turinsky
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the author may not be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#pragma once

#include <vector>
#include <string>

#define MAX_STRING 8192

/*
    Class for handling arbitrary precision integers.

    Stores integer in the array of bytes represented as base 256 number.
*/
class VTBignum
{
public:
    enum Base {Base_10 = 10, Base_16 = 16, Base_256 = 256};

    VTBignum();
    VTBignum(const VTBignum& other);
    VTBignum& operator=(VTBignum rhs);
    ~VTBignum();

    // return size in bytes, needed to store the number without a sign
    inline int size() const { return _chunks.size(); }

    // View the n unsigned bytes as an integer in base 256,
    // and return a VTBignum with the same numeric value
    static VTBignum fromByteArray(const unsigned char* bytes, int size, int sign = 0);

    static VTBignum fromInt(int value);
    static VTBignum fromLongLong(long long value);

    // read number from string (accept Base_10 and Base_16 enums)
    // throws std::runtime_error if encounters unknows characters
    // size is provided if string is not null-terminated
    static VTBignum fromString(const char* char_array, int size = MAX_STRING, Base base = Base_10);

    // return the sign, and store the bytes as an integer in base 256 
    // in array of bytes from the least significant to the most significant bytes
    char toByteArray(unsigned char* bytes) const;

    // returns long long representation of current number if it fits;
    // otherwise throws std::runtime_error
    long long toLongLong() const;

    // for integers with bases from 11 to 16 and 256, string contains hex digits
    // for other integers digits are represented in base 10
    // for integers with bases greater than 16, digits are separated with dot
    std::string toString(int base = Base_10) const;

    VTBignum& operator+=(const VTBignum &rhs);
    const VTBignum operator+(const VTBignum &other) const;

    VTBignum& operator-=(const VTBignum &rhs);
    const VTBignum operator-(const VTBignum &other) const;

    VTBignum& operator*=(const VTBignum &rhs);
    const VTBignum operator*(const VTBignum &other) const;

    // Not implemented
    /*
    VTBignum& operator/=(const VTBignum &rhs);
    const VTBignum operator/(const VTBignum &other) const;
    

    VTBignum pow_modulo(const VTBignum& power, const VTBignum& mod);
    */

    VTBignum& pow(unsigned long long power);

    VTBignum& VTBignum::operator++(); // prefix
    VTBignum VTBignum::operator++(int unused); // postfix
    VTBignum& VTBignum::operator--(); // prefix
    VTBignum VTBignum::operator--(int unused); // postfix

    bool operator==(const VTBignum& other) const;
    bool operator!=(const VTBignum& other) const;

    bool operator>(const VTBignum& other) const;
    bool operator<(const VTBignum& other) const;
    bool operator>=(const VTBignum& other) const;
    bool operator<=(const VTBignum& other) const;

    friend VTBignum operator-(const VTBignum &bignum);
    friend bool operator!(const VTBignum &bignum);
 
private:
    static VTBignum create_empty();

    inline void invert() { _sign = !_sign; }
    void normilize();

    void add_no_sign(const VTBignum& bignum, int base = Base_256);
    int compare_no_sign(const VTBignum& other) const;
    void mult_by_single_digit(VTBignum& accumulator, unsigned char digit, int position, int base = Base_256);
    
    static VTBignum complement(const VTBignum& bignum, int size, int base = Base_256);
    static std::string print(const VTBignum&, int base);

    friend void swap(VTBignum& first, VTBignum& second);

private:
    char _sign;      // 0 for +; 1 for -
    std::vector<unsigned char> _chunks;

};