#include "DEAL_128_coder.hpp"

#include "DEAL_128_coder_function.hpp"

#include "help_function.hpp"

DEAL_128_coder::DEAL_128_coder(uint128_t key, padding_mode padding_mode)
    : coder_detail(padding_mode)
    , _rk(key)
{
}

DEAL_128_coder::DEAL_128_coder(std::string const & key, padding_mode padding_mode)
    : coder_detail(padding_mode)
    , _rk(key)
{    
}

byte_array DEAL_128_coder::code(byte_array const & in)
{
    DEAL_128_coder_function f;

    byte_array res;
    byte_array::const_iterator it = in.cbegin();
    byte_array::const_iterator it_end = in.cend();
    while(it != it_end)
    {
        uint128_t block = get_block(it, it_end);
        auto crypt_block = f.coder(block, _rk);
        push_as_byte(res, crypt_block);
    }
    return res;
}

byte_array DEAL_128_coder::decode(byte_array const & in)
{
    DEAL_128_decoder_function f;
    
    byte_array res;
    byte_array::const_iterator it = in.cbegin();
    byte_array::const_iterator it_end = in.cend();
    while(it != it_end)
    {
        uint128_t block = get_block(it, it_end);
        auto crypt_block = f.coder(block, _rk);
        push_as_byte(res, crypt_block);
    }
    return res;
}
