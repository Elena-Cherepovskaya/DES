#pragma once

#include <vector>

enum class firstBit
{
    low,
    high
};

template<typename T>
T P_block(T const & in, std::vector<int> const & index_array, firstBit fb, bool firstIndexIsOne)
{
    T res{0};
    T bit_dst_mask{1};
    for(auto index : index_array)
    {
        if (firstIndexIsOne)
            index -= 1;

        if (index >= 0 && index < sizeof(T) * 8)
        {
            T const bit_src_mask = (fb == firstBit::low) ? (T{1} << index) : (T{1} << (sizeof(T) * 8 - index - 1));
            if ((in & bit_src_mask) == bit_src_mask)
                res |= bit_dst_mask;
        }
        
        bit_dst_mask <<= 1;
    }

    return res;
}
