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

    int d = 100000;
    printf("%d\n", d);
    printf("base 256: %s\n", VTBignum::fromInt(d).toString(256).c_str());
    printf("base  10: %s\n", VTBignum::fromInt(d).toString(10).c_str());
    printf("base  16: %s\n", VTBignum::fromInt(d).toString(16).c_str());
    printf("base  25: %s\n", VTBignum::fromInt(d).toString(35).c_str());
    printf("All good\n");
    getchar();
    return 0;
}