#include "DEAL_128_round_key.hpp"

#include "help_function.hpp"

DEAL_128_round_key::DEAL_128_round_key(uint128_t key)
{
    generate_round_keys(key);
}

byte_array DEAL_128_round_key::code(DES_coder const & coder, byte_array const & in)
{
    auto it = in.cbegin();
    auto it_end = in.cend();
    return coder.code(it, it_end);
}

void DEAL_128_round_key::generate_round_keys(uint128_t const & key)
{
    DES_coder E;
    E.set_key(0x123456789abcdefull);
    E.set_crypt_mode(crypt_mode::CBC);
    E.set_padding(padding_mode::Zeros);

    uint64_t const key1 = static_cast<uint64_t>(key >> 64);
    uint64_t const key2 = static_cast<uint64_t>(key);

    uint64_t const RK1 = vector_to_uint<uint64_t>(code(E, uint_to_vector(key1)));
    uint64_t const RK2 = vector_to_uint<uint64_t>(code(E, uint_to_vector(key2 ^ RK1)));
    uint64_t const RK3 = vector_to_uint<uint64_t>(code(E, uint_to_vector(key1 ^ 1 ^ RK2)));
    uint64_t const RK4 = vector_to_uint<uint64_t>(code(E, uint_to_vector(key2 ^ 2 ^ RK3)));
    uint64_t const RK5 = vector_to_uint<uint64_t>(code(E, uint_to_vector(key1 ^ 4 ^ RK4)));
    uint64_t const RK6 = vector_to_uint<uint64_t>(code(E, uint_to_vector(key2 ^ 8 ^ RK5)));

    _round_keys.emplace_back(RK1);
    _round_keys.emplace_back(RK2);
    _round_keys.emplace_back(RK3);
    _round_keys.emplace_back(RK4);
    _round_keys.emplace_back(RK5);
    _round_keys.emplace_back(RK6);    
}
