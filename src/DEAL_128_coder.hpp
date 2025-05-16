#pragma once

#include "crypt_type.hpp"

#include "base_coder.hpp"

#include "DEAL_128_round_key.hpp"
#include "DEAL_128_coder_function.hpp"

class DEAL_128_coder final : public base_coder<uint128_t, uint128_t, DEAL_128_round_key, DEAL_128_coder_function, DEAL_128_decoder_function>
{
};
