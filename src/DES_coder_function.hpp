#pragma once

#include <cstdint>

#include "I_coder_function.hpp"
#include "P_block.hpp"

class DES_function_detail : public I_coder_function<uint64_t, uint64_t>
{
    protected:
        template<typename T_it>    
        uint64_t impl_coder(uint64_t const in,
            T_it const & it_key_begin,
            T_it const & it_key_end)
        {
            auto const chunk = P_block(in,
                {
                    58, 50, 42, 34, 26, 18, 10, 2, 60, 52, 44, 36, 28, 20, 12, 4,        
                    62,	54,	46,	38,	30,	22,	14,	6, 64, 56, 48, 40, 32, 24, 16, 8,
                    57,	49,	41,	33,	25,	17,	 9, 1, 59, 51, 43, 35, 27, 19, 11, 3,
                    61,	53,	45,	37,	29,	21,	13,	5, 63, 55, 47, 39, 31, 23, 15, 7
                },
                firstBit::low, true);
        
            uint64_t left = chunk >> 32;
            uint64_t right = chunk & 0xFFFFFFFFULL;
        
            for(auto it = it_key_begin; it != it_key_end; ++it)
            {
                auto const key = *it;
                auto const tmp = right;
        
                // E (32 бита в 48 бит)
                right = P_block(right,
                    {
                        32,  1,  2,  3,  4,  5,
                         4,  5,  6,  7,  8,  9,
                         8,  9, 10, 11, 12, 13,
                        12, 13, 14,	15,	16,	17,
                        16,	17,	18,	19,	20,	21,
                        20,	21,	22,	23,	24,	25,
                        24,	25,	26,	27,	28,	29,
                        28,	29,	30,	31,	32,	1
                    },
                    firstBit::low, true);
        
                right ^= key;
        
                // Подстановка (48 бит в 32 бита)
                uint64_t new_right = 0;
                for(int i = 0; i < 8; ++i)
                    new_right |= uint64_t(S_block((right >> (8 * i)) & 0xFFU, i)) << (4 * i);
        
                // P
                right = P_block<uint64_t>(new_right,
                    {
                        16,  7, 20, 21, 29, 12, 28, 17,
                         1, 15, 23, 26,  5, 18, 31, 10,
                         2,  8,	24, 14, 32, 27,  3,  9,
                        19, 13, 30,  6, 22, 11,	 4,	25
                    }, firstBit::low, true);

                right ^= left;
                left = tmp;
            }
        
            auto const cipher_chunk = (right << 32) | left;
            auto const res = P_block(cipher_chunk,
                {
                    40, 8, 48, 16, 56, 24, 64, 32, 39, 7, 47, 15, 55, 23, 63, 31,
                    38, 6, 46, 14, 54, 22, 62, 30, 37, 5, 45, 13, 53, 21, 61, 29,
                    36, 4, 44, 12, 52, 20, 60, 28, 35, 3, 43, 11, 51, 19, 59, 27,
                    34,	2, 42, 10, 50, 18, 58, 26, 33, 1, 41,  9, 49, 17, 57, 25
                },
                firstBit::low, true);

            return res;            
        }

        uint8_t S_block(uint8_t in, int block) const;
};

class DES_coder_function final : public DES_function_detail
{
    public:
        uint64_t coder(uint64_t const in, I_round_key<uint64_t> const  & round_key) override;
};

class DES_decoder_function final : public DES_function_detail
{
    public:
        uint64_t coder(uint64_t const in, I_round_key<uint64_t> const & round_key) override;
};
