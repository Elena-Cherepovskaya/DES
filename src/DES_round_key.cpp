#include "DES_round_key.hpp"
#include "help_function.hpp"
#include "P_block.hpp"
#include <array>

DES_round_key::DES_round_key(uint64_t key)
    : _round_keys(number_round)
{
    generate_round_keys(key);
}

void DES_round_key::generate_round_keys(uint64_t const key)
{
    uint64_t key56 = P_block(key, {
        1,  2,  3,  4,  5,  6,  7,//  8,
        9, 10, 11, 12, 13, 14, 15,// 16,
       17, 18, 19, 20, 21, 22, 23,// 24,
       25, 26, 27, 28, 29, 30, 31,// 32,
       33, 34, 35, 36, 37, 38, 39,// 40,
       41, 42, 43, 44, 45, 46, 47,// 48,
       49, 50, 51, 52, 53, 54, 55,// 56,
       57, 58, 59, 60, 61, 62, 63//, 64
   }, firstBit::low, true);

   std::array<int, number_round> const key_shift{1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};

   uint64_t left = (key56 >> 28) & 0xFFFFFFF;
   uint64_t right = key56 & 0xFFFFFFF;

   for(int i = 0; i < number_round; i++)
   {
    //    right <<= key_shift[i]; // Переделать на циклический сдвиг
        right = shift_left(right, key_shift[i]);
    //    left <<= key_shift[i]; // Переделать на циклический сдвиг
        left = shift_left(left, key_shift[i]);
       uint64_t concat = (left << 28) | right;
       _round_keys[i] = P_block(concat,
           {
               14, 17, 11,	24,  1,  5,  3, 28, 15,  6, 21, 10, 23, 19, 12,  4,
               26,  8, 16,  7,	27,	20,	13,	 2,	41,	52,	31,	37,	47,	55,	30,	40,
               51, 45,	33,	48,	44,	49,	39,	56,	34,	53,	46,	42,	50,	36,	29,	32
           },
           firstBit::low, true);
   }
}