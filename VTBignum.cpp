#include "VTBignum.h"

#include <sstream>
#include <iomanip>
#include <assert.h>
#include <math.h>
#include <exception>

VTBignum::VTBignum(): _sign(0), _chunks()
{}

VTBignum::VTBignum(const VTBignum& other): _sign(other._sign), _chunks(other._chunks)
{}

VTBignum& VTBignum::operator=(VTBignum rhs)
{
    swap(*this, rhs);
    return *this;
}

VTBignum::~VTBignum(void)
{}

VTBignum VTBignum::fromByteArray(const unsigned char* bytes, int size, int sign)
{
    VTBignum bignum;
    bignum._sign = ( sign == 0 ? 0 : 1 );
    bignum._chunks.reserve(size);

    for (int i = 0; i < size; ++i)
    {
        bignum._chunks.push_back(bytes[i]);
    }

    return bignum;
}

VTBignum VTBignum::fromInt(int value)
{
    return fromLongLong(value);
}

VTBignum VTBignum::fromLongLong(long long value)
{
    VTBignum bignum;
    bignum._chunks.clear();
    bignum._sign = (value < 0 ? 1 : 0);
    long long overflow = (value < 0 ? -value : value);

    while (overflow > 0)
    {
        bignum._chunks.push_back(overflow % 256);
        overflow = overflow / 256;
    }

    return bignum;
}

VTBignum VTBignum::fromString(const char* char_array, int size, Base base)
{
    assert(base == Base_10 || base == Base_16);
    VTBignum bignum;

    int i = 0;
    char sign = 0;
    if (char_array[i] == '-')
    {
        sign = 1;
        ++i;
    }
    else if (char_array[i] == '+')
    {
        sign = 0;
        ++i;
    }

    for ( /* none */; char_array[i] != '\0' && i < size; ++i)
    {
        if ( char_array[i] < '0' || char_array[i] > (base == Base_10 ? '9' : 'f') )
            throw std::runtime_error("Wrong character in number");

        bignum = (bignum * fromInt(base)) + fromInt(char_array[i] - '0');
    }

    bignum._sign = sign;
    return bignum;
}

char VTBignum::toByteArray(unsigned char* bytes) const
{
    for (int i = 0; i < size(); ++i)
    {
        bytes[i] = _chunks[i];
    }
    return _sign;
}

long long VTBignum::toLongLong() const
{
    if (size() > sizeof(long long))
        throw std::runtime_error("Number is too big for long long");
    else if (size() == sizeof(long long) && _chunks[size()-1] & 0x80) // one bit is needed for sign in signed long long
        throw std::runtime_error("Number is too big for long long");

    long long result = 0;
    std::vector<unsigned char>::const_reverse_iterator pdigit;
    for (pdigit = _chunks.rbegin(); pdigit != _chunks.rend(); ++pdigit)
    {
        result *= 256;
        result += *pdigit;
    }

    if (_sign == 1)
        result *= -1;

    return result;
}

std::string VTBignum::toString(int base) const
{
    assert(base > 1 && base <= 256);

    if (base == 256)
        return print(*this, base);

    VTBignum dest;

    // size needed to hold this number in given base
    while ( dest.size() < ceil( size() * log(static_cast<double>(256))/log(static_cast<double>(base)) ) )
        dest._chunks.push_back(0);

    std::vector<unsigned char>::const_reverse_iterator base256_digit;
    for (base256_digit = _chunks.rbegin(); base256_digit != _chunks.rend(); ++base256_digit)
    {
        int overflow = *base256_digit;
        int i = 0;

        for (i = 0; i < dest.size(); ++i)
        {
            int acc = (dest._chunks[i] * 256) + overflow;
            dest._chunks[i] = acc % base;
            overflow = acc / base;
        }

        while (overflow > 0)
        {
            dest._chunks[i] = overflow % base;
            overflow = overflow / base;
            ++i;
        }
    }

    dest.normilize();

    return print(dest, base);
}

VTBignum& VTBignum::operator+=(const VTBignum &rhs)
{
    // use base's complement addition for adding negative to positive
    if (_sign != rhs._sign)
    {
        int this_is_bigger = compare_no_sign(rhs);
        int len = ( this_is_bigger > 0 ? size() : rhs.size() );
        
        if (this_is_bigger == 0) return this->operator=( fromInt(0) );
        
        if (_sign == 1)      // this < 0
        {
            if (this_is_bigger > 0)     
                _sign = 1;      // result < 0  ( -6 + 3 )
            else
                _sign = 0;       // result > 0  ( -3 + 6 )

            this->operator=( complement( *this, len, 256 ) );
            add_no_sign(rhs);
        }
        else /* _sign == 0 */     // this >= 0
        {
            if (this_is_bigger > 0)     
                _sign = 0;      // result < 0  ( 6 + -3 )
            else
                _sign = 1;       // result > 0  ( 3 + -6 )

            add_no_sign( complement( rhs, len, 256 ) );
        }

        // add 1 as per algorithm specification
        add_no_sign(fromInt(1));

        // if overflow was considered, drop it
        if (size() > len)
            _chunks.pop_back();

        // convert back to regular form
        if (_sign == 1)
        {
            this->operator=(complement(*this, len, 256));
            add_no_sign(fromInt(1));
        }

        normilize();

    }
    else
    {
        add_no_sign(rhs);
    }

    return *this;
}



const VTBignum VTBignum::operator+(const VTBignum &other) const
{
    return VTBignum(*this) += other;
}

VTBignum& VTBignum::operator-=(const VTBignum &rhs)
{
    return this->operator+=(-rhs);
}

const VTBignum VTBignum::operator-(const VTBignum &other) const
{
    return VTBignum(*this) -= other;
}

VTBignum& VTBignum::operator*=(const VTBignum &rhs)
{
    if (this->operator==(fromInt(0)) || rhs == fromInt(0)) return this->operator=(fromInt(0));

    VTBignum accumulator;
    /*
        + (0) ^ + (0)   ->   + (0)
        + (0) ^ - (1)   ->   - (1)
        - (1) ^ + (0)   ->   - (1)
        - (1) ^ - (1)   ->   + (0)
    */
    accumulator._sign = (_sign == 1) ^ (rhs._sign == 1);

    for (int i = 0; i < rhs.size(); ++i)
        mult_by_single_digit(accumulator, rhs._chunks[i], i);

    this->operator=(accumulator);
    return *this;
}

const VTBignum VTBignum::operator*(const VTBignum &other) const
{
    return VTBignum(*this) *= other;
}

bool VTBignum::operator==(const VTBignum& other) const
{
    return ( this->_sign == other._sign && this->_chunks == other._chunks );
}

bool VTBignum::operator!=(const VTBignum& other) const
{
    return !( this->operator==(other) );
}

bool VTBignum::operator>(const VTBignum& other) const
{
    if ( _sign != other._sign ) return ( _sign == 0 );

    bool bigger = (compare_no_sign(other) > 0);
    return ( _sign == 0 ? bigger : !bigger );
}

bool VTBignum::operator<(const VTBignum& other) const
{
    if ( _sign != other._sign ) return ( _sign == 1 );

    bool smaller = (compare_no_sign(other) < 0);
    return ( _sign == 0 ? smaller : !smaller );
}

bool VTBignum::operator>=(const VTBignum& other) const
{
    return !( this->operator<(other) );
}

bool VTBignum::operator<=(const VTBignum& other) const
{
    return !( this->operator>(other) );
}

VTBignum operator-(const VTBignum &bignum)
{
    VTBignum result(bignum);
    result._sign = ! result._sign;
    return result;
}

bool operator!(const VTBignum &bignum)
{
    return bignum == VTBignum::fromLongLong(0);
}

// PRIVATE FUNCTIONS


void VTBignum::add_no_sign(const VTBignum& rhs, int base)
{
    assert(base > 1 && base <=256);

    // increase this if it is shorter
    while (size() < rhs.size())
        _chunks.push_back(0);

    int overflow = 0;

    // add digits
    for (int i = 0; i < rhs.size(); ++i)
    {
        int sum = _chunks[i] + rhs._chunks[i] + overflow;
        _chunks[i] = sum % base;
        overflow = sum / base;
    }

    // propagate overflow
    for (int i = rhs.size(); i < size(); ++i)
    {
        if (overflow == 0)
            break;

        int sum = _chunks[i] + overflow;
        _chunks[i] = sum % base;
        overflow = sum / base;
    }

    if (overflow > 0)
        _chunks.push_back(overflow);
}

int VTBignum::compare_no_sign(const VTBignum& other) const
{
    // this is longer then other
    if ( size() != other.size() ) return size() - other.size();

    // start comparing from most significant digits
    assert(size() == other.size());

    for (int i = size() - 1; i >= 0; --i)
    {
        int diff = _chunks[i] - other._chunks[i];
        if (diff != 0)
            return diff;
    }

    // they are equal
    return 0;
}

void VTBignum::mult_by_single_digit(VTBignum& accumulator, unsigned char digit, int position, int base)
{
    assert(base > 1 && base <=256);
    assert(position >= 0);

    if (digit == 0)
        return;

    int overflow = 0;

    for (int i = 0; i < size(); ++i)
    {
        //increase accumulator to needed size
        while (accumulator.size() < position + i + 1)
            accumulator._chunks.push_back(0);

        int acc = (_chunks[i] * digit) + overflow + accumulator._chunks[position+i];
        accumulator._chunks[position+i] = acc % base;
        overflow = acc / base;
    }

    if (overflow > 0)
    {
        while (accumulator.size() < position + size() + 1)
            accumulator._chunks.push_back(0);

        accumulator._chunks[position + size()] = overflow;
    }
}

VTBignum VTBignum::complement(const VTBignum& bignum, int size, int base)
{
    assert(size >= bignum.size());
    assert(base > 1 && base <= 256);

    VTBignum new_bignum;

    // preserve original sign
    new_bignum._sign = bignum._sign;

    std::vector<unsigned char>::const_iterator pdigit;
    for (pdigit = bignum._chunks.begin(); pdigit != bignum._chunks.end(); ++pdigit)
        new_bignum._chunks.push_back( base - 1 - *pdigit );

    // fill missing size with max digit in current base
    // (just like when shifting right negative binary two's complement numbers)
    while ( new_bignum.size() < size )
        new_bignum._chunks.push_back( base - 1 );

    return new_bignum;
}

void VTBignum::normilize()
{
    while (_chunks[size()-1] == 0)
        _chunks.pop_back();
}

std::string VTBignum::print(const VTBignum& source, int base)
{
    assert(base > 1 && base <= 256);

    int width;
    if (base <= 16)
        width = 1;
    else
        width = 2;

    std::stringstream ss;
    std::vector<unsigned char>::const_reverse_iterator digit;
    for (digit = source._chunks.rbegin(); digit != source._chunks.rend(); ++digit)
    {
        ss << ( width == 1 || base == 256 ? std::hex : std::dec ) << std::setw(width) << static_cast<int>(*digit) << ( width > 1 ? "," : "" );
    }

    std::string res = ss.str();

    if (width > 1)
        res.erase(res.size()-1);

    return res;
}

void swap(VTBignum& first, VTBignum& second)
{
    std::swap(first._sign, second._sign); 
    std::swap(first._chunks, second._chunks);
}