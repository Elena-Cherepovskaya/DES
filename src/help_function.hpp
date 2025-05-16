#pragma once

#include <vector>
#include <cassert>
#include <cstdint>
#include <string>

template<typename T>
T reverse(T in)
{
    T res{0};
    for(size_t i = 0; i < sizeof(T); ++i)
    {
        res <<= 8;
        res |= (in & 0xff);
        in >>= 8;
    }
    return res;
}

template<typename T, typename Iterator>
T vector_to_uint(Iterator it, Iterator it_end)
{
    T res = 0;    
    while(it != it_end)
    {
        res <<= 8;
        res |= (*it & 0xff);
        ++it;
    }
    return res;
}

template<typename T>
T vector_to_uint(std::vector<uint8_t> const & array)
{
    return vector_to_uint<T>(array.cbegin(), array.cend());
}

template<typename T>
T string_to_uint(std::string const & str)
{
    return vector_to_uint<T>(str.cbegin(), str.cend());
}

template<typename T>
std::vector<uint8_t> uint_to_vector(T data)
{
    std::vector<uint8_t> res;
    for(size_t i = 0; i < sizeof(T); ++i)
    {
        res.push_back(data & 0xff);
        data >>= 8;
    }
    return res;
}

template<typename T>
void push_as_byte(std::vector<uint8_t> & output, T in_pure)
{
    T in = reverse(in_pure);
    for(size_t i = 0; i < sizeof(T); ++i)
    {
        output.push_back((uint8_t)(in & 0xff));
        in >>= 8;
    }
}

template<typename T>
void set_as_byte(std::vector<uint8_t>::iterator it, T in_pure)
{
    T in = reverse(in_pure);
    for(size_t i = 0; i < sizeof(T); ++i)
    {
        *it = uint8_t(in & 0xff);
        ++it;
        in >>= 8;
    }
}

template <typename T>
constexpr int size_of_bit()
{
    return sizeof(T) * 8;
}

template <typename T>
T shift_left(T x, int n)
{
    return (x << n) | (x >> (size_of_bit<T>() - n));
}

template <typename T>
T shift_right(T x, int n)
{
    return (x >> n) | (x << (size_of_bit<T>() - n));
}
