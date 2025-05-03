#pragma once

#include "crypt_type.hpp"

#include "I_coder_function.hpp"
#include "DES_coder_function.hpp"

class DEAL_128_coder_function : public I_coder_function<uint128_t, DES_round_key>
{
public:
    uint128_t coder(uint128_t in, I_round_key<DES_round_key> const  & round_key) override
    {
        DES_coder_function DES_function;
        uint64_t Xl = (uint64_t)(in >> 64);
        uint64_t Xr = (uint64_t)in;
        auto const & keys = round_key.get_round_keys();
        for(int i = 0; i < 6; ++i)
        {
            auto const prev_Xl = Xl;
            Xl = DES_function.coder(Xl, keys[i]) ^ Xr;
            Xr = prev_Xl;
        }
        return (uint128_t(Xl) << 64) | uint128_t(Xr);
    }
};

class DEAL_128_decoder_function : public I_coder_function<uint128_t, DES_round_key>
{
public:
    uint128_t coder(uint128_t in, I_round_key<DES_round_key> const  & round_key) override
    {
        DES_coder_function DES_function;
        uint64_t Xr = (uint64_t)(in >> 64);
        uint64_t Xl = (uint64_t)in;
        auto const & keys = round_key.get_round_keys();
        for(int i = 5; i >= 0; --i)
        {
            auto const prev_Xl = Xl;
            Xl = DES_function.coder(Xl, keys[i]) ^ Xr;
            Xr = prev_Xl;
        }
        return (uint128_t(Xr) << 64) | uint128_t(Xl);
    }
};
