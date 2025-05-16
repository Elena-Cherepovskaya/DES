#pragma once

#include "crypt_type.hpp"

#include "base_coder.hpp"

#include "DES_round_key.hpp"
#include "DES_coder_function.hpp"

class DES_coder final : public base_coder<uint64_t, uint64_t, DES_round_key, DES_coder_function, DES_decoder_function>
{
};
