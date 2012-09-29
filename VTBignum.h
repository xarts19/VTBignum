#pragma once

#include <vector>
#include <string>

class VTBignum
{
public:
    VTBignum();
    VTBignum(const VTBignum& other);
    VTBignum& operator=(VTBignum rhs);
    ~VTBignum();

    void swap(VTBignum& first, VTBignum& second);

    // return size in bytes, needed to store the number without a sign
    inline int size() const { return _chunks.size(); }

    // View the n unsigned bytes as an integer in base 256,
    // and return a VTBignum with the same numeric value
    static VTBignum fromByteArray(const unsigned char* bytes, int size, int sign);

    static VTBignum fromInt(int value);
    static VTBignum fromLong(long value);
    static VTBignum fromLongLong(long long value);

    // return the sign, and store the bytes as an integer in base 256 
    // in array of bytes from the least significant to the most significant bytes
    char toByteArray(unsigned char* bytes) const;

    std::string toString(int base = 10) const;

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
    */

    bool operator==(const VTBignum& other) const;
    bool operator!=(const VTBignum& other) const;

    bool operator>(const VTBignum& other) const;
    bool operator<(const VTBignum& other) const;
    bool operator>=(const VTBignum& other) const;
    bool operator<=(const VTBignum& other) const;

    friend VTBignum operator-(const VTBignum &bignum);
    friend bool operator!(const VTBignum &bignum);
 
private:
    static VTBignum complement(const VTBignum& bignum, int size, int base = 256);
    void add_no_sign(const VTBignum& bignum, int base = 256);
    int compare_no_sign(const VTBignum& other) const;
    void mult_by_single_digit(VTBignum& accumulator, unsigned char digit, int position, int base = 256);
    inline void invert() { _sign = !_sign; }
    void normilize();
    static std::string print(const VTBignum&, int base);

private:
    char _sign;      // 0 for +; 1 for -
    std::vector<unsigned char> _chunks;

};