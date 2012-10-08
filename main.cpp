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
#include "VTBignum.h"

#include <stdio.h>
#include <assert.h>

void test_plus(long long a, long long b, long long c)
{
    VTBignum vta = VTBignum::fromLongLong(a);
    VTBignum vtb = VTBignum::fromLongLong(b);
    VTBignum vtab = vta + vtb;
    VTBignum vtc = VTBignum::fromLongLong(c);

    assert(vtab == vtc);
}

void test_minus(long long a, long long b, long long c)
{
    VTBignum vta = VTBignum::fromLongLong(a);
    VTBignum vtb = VTBignum::fromLongLong(b);
    VTBignum vtab = vta - vtb;
    VTBignum vtc = VTBignum::fromLongLong(c);

    assert(vtab == vtc);
}

void test_mult(long long a, long long b, long long c)
{
    VTBignum vta = VTBignum::fromLongLong(a);
    VTBignum vtb = VTBignum::fromLongLong(b);
    VTBignum vtab = vta * vtb;
    VTBignum vtc = VTBignum::fromLongLong(c);

    assert(vtab == vtc);
}

VTBignum factorial(long long value)
{
    VTBignum res = VTBignum::fromInt(1);
    for (long long i = 1; i <= value; ++i)
        res *= VTBignum::fromLongLong(i);
    return res;
}

// test cases
int main(int argc, char** argv)
{
    
    unsigned char bytes1[] = {35, 67, 234, 147};
    unsigned char bytes2[] = {87, 187, 221, 254, 123, 23};

    VTBignum bignum1 = VTBignum::fromByteArray(bytes1, 4, 0);
    VTBignum bignum2 = VTBignum::fromByteArray(bytes2, 6, 0);

    // test 1
    VTBignum bignum_result = bignum1 + bignum2;

    unsigned char* bytes_result = (unsigned char*) malloc(bignum_result.size());
    bignum_result.toByteArray(bytes_result);

    // test 2
    VTBignum bignum_result2 = bignum2 + bignum1;

    unsigned char* bytes_result2 = (unsigned char*) malloc(bignum_result2.size());
    bignum_result2.toByteArray(bytes_result2);

    assert(bignum_result == bignum_result2);

    assert( VTBignum::fromInt(0) == VTBignum() );

    assert( VTBignum::fromInt(-20000) < VTBignum::fromInt(20000) );
    assert( VTBignum::fromInt(20000) < VTBignum::fromInt(-20000) == false );
    assert( VTBignum::fromInt(20000) < VTBignum::fromInt(20000) == false );
    assert( VTBignum::fromInt(21231) <= VTBignum::fromInt(21231) );
    assert( VTBignum::fromInt(3245) <= VTBignum::fromInt(3246) );
    assert( VTBignum::fromInt(1) <= VTBignum::fromInt(1234556) );
    assert( VTBignum::fromInt(45634564) == VTBignum::fromInt(45634564) );

    test_plus(-515, -495, -1010);
    test_plus(515, 495, 1010);
    test_plus(515, -495, 20);
    test_plus(-515, 495, -20);
    test_plus(495, -515, -20);
    test_plus(-495, 515, 20);
    test_plus(-495, 495, 0);
    test_plus(495, -495, 0);
    test_plus(0, 0, 0);
    test_plus(0, 1234, 1234);
    test_plus(123, 0, 123);

    test_minus(515, 495, 20);
    test_minus(495, 515, -20);
    test_minus(515, -495, 1010);
    test_minus(-495, 515, -1010);
    test_minus(-515, -495, -20);
    test_minus(-495, -515, 20);
    test_minus(515, 515, 0);
    test_minus(-515, -515, 0);
    test_minus(0, 0, 0);
    test_minus(0, 1234, -1234);
    test_minus(123, 0, 123);

    test_mult(500, 500, 250000);
    test_mult(500, -500, -250000);
    test_mult(-500, 500, -250000);
    test_mult(-500, -500, 250000);
    test_mult(500, 1, 500);
    test_mult(1, 500, 500);
    test_mult(0, 500, 0);
    test_mult(500, 0, 0);
    test_mult(0, 0, 0);

    long long a = 2147483647;
    a += 1;
    a *= a;
    a *= 2;
    a -= 1;

    VTBignum res1 = VTBignum::fromLongLong(2147483647) + VTBignum::fromInt(1);
    VTBignum res = res1 * res1 * VTBignum::fromInt(2) - VTBignum::fromInt(1);
    long long b = res.toLongLong();

    assert(a == b);
    assert( VTBignum::fromLongLong(-2147483647).toLongLong() == -2147483647 );
    assert( VTBignum::fromLongLong(0).toLongLong() == 0 );

    assert( VTBignum::fromString( "123" ) == VTBignum::fromLongLong(123) );
    assert( VTBignum::fromString( "+12345678" ) == VTBignum::fromLongLong(12345678) );

    VTBignum frstr = VTBignum::fromString( "-12345678" );
    VTBignum frll = VTBignum::fromLongLong(-12345678);
    assert( VTBignum::fromString( "-12345678" ) == VTBignum::fromLongLong(-12345678) );

    // 2^64 == 2^16 * 2^16 * 2^16 * 2^16
    assert( VTBignum::fromString( "18446744073709551616" ) 
        == VTBignum::fromInt(65536) * VTBignum::fromInt(65536) * VTBignum::fromInt(65536) * VTBignum::fromInt(65536) );

    assert( ++VTBignum::fromInt(10000) == VTBignum::fromInt(10001) );
    assert( VTBignum::fromInt(10000)++ == VTBignum::fromInt(10000) );
    assert( --VTBignum::fromInt(10000) == VTBignum::fromInt(9999) );
    assert( VTBignum::fromInt(10000)-- == VTBignum::fromInt(10000) );

    VTBignum pow10 = VTBignum::fromInt(2).pow(10);
    assert( pow10 == VTBignum::fromInt(1024) );

    int d = 100000;
    printf("%d\n", d);
    printf("base 256: %s\n", VTBignum::fromInt(d).toString(256).c_str());
    printf("base  10: %s\n", VTBignum::fromInt(d).toString(10).c_str());
    printf("base  16: %s\n", VTBignum::fromInt(d).toString(16).c_str());
    printf("base  25: %s\n", VTBignum::fromInt(d).toString(35).c_str());
    printf("All good\n");
    
    

    long long fact = 1000;
    VTBignum factbig = factorial(fact);
    printf("Factorial %lld size: %d\n", fact, factbig.size());

    getchar();
    return 0;
}