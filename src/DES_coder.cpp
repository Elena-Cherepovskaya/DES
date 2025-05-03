#include "DES_coder.hpp"

#include "DES_coder_function.hpp"
#include "help_function.hpp"

#include <cassert>
#include <iostream>

DES_coder::DES_coder(uint64_t key, crypp_mode crtyp, padding_mode padding, uint64_t start_vector, uint64_t delta_counter)
    : coder_detail(padding)
    , _round_key(key)
    , _crypt_mode(crtyp)
    , _start_vector(start_vector)
    , _delta_counter(delta_counter)
{    
}

byte_array DES_coder::code(byte_array const & in)
{
    DES_coder_function f;
    std::vector<uint64_t> res;

    reset_chaining();
    auto it = in.cbegin();
    auto it_end = in.cend();
    while(it != it_end)
    {
        uint64_t data = get_block(it, it_end);
        data = begin_crypt_code(data);
        auto crypt_data = f.coder(data, _round_key);
        crypt_data = end_crypt_code(crypt_data);

        res.push_back(crypt_data);
    }

    return stream_convert(res);
}


byte_array DES_coder::decode(byte_array const & in)
{
    DES_coder_function f_coder;
    DES_decoder_function f_decoder;
    byte_array res;

    auto it = in.cbegin();
    reset_chaining();
    auto it_end = in.cend();

    while(it != it_end)
    {
        uint64_t data = get_block(it, it_end);

        uint64_t decrypt_data = 0;
        data = begin_crypt_decode(data);
        if (_crypt_mode == crypp_mode::CFB ||
            _crypt_mode == crypp_mode::OFB ||
            _crypt_mode == crypp_mode::CTR ||
            _crypt_mode == crypp_mode::Random_Delta)
            decrypt_data = f_coder.coder(data, _round_key);
        else
            decrypt_data = f_decoder.coder(data, _round_key);
        decrypt_data = end_crypt_decode(decrypt_data);

        push_as_byte(res, decrypt_data);
    }

    return res;
}

void DES_coder::reset_chaining()
{
    _vector = _start_vector;
}


uint64_t DES_coder::begin_crypt_code(uint64_t data)
{
    switch (_crypt_mode)
    {
        case crypp_mode::CBC:
            data ^= _vector;
            break;

        case crypp_mode::PCBC:
            _tmp_vector = data;
            data ^= _vector;
            break;

        case crypp_mode::CFB:
        case crypp_mode::OFB:
        case crypp_mode::CTR:
        case crypp_mode::Random_Delta:
            _tmp_vector = data;
            data = _vector;
            break;

    }
    return data;
}

uint64_t DES_coder::end_crypt_code(uint64_t data)
{
    switch (_crypt_mode)
    {
        case crypp_mode::ECB:
            break;

        case crypp_mode::CBC:
            _vector = data;
            break;

        case crypp_mode::PCBC:
            _vector = data ^ _tmp_vector;
            break;

        case crypp_mode::CFB:
            data ^= _tmp_vector;
            _vector = data;
            break;

        case crypp_mode::OFB:
            _vector = data;
            data ^= _tmp_vector;
            break;

        case crypp_mode::CTR:
            data ^= _tmp_vector;
            ++_vector;
            break;

        case crypp_mode::Random_Delta:
            data ^= _tmp_vector;
            _vector += _delta_counter;
            break;
    }
    return data;
}

uint64_t DES_coder::begin_crypt_decode(uint64_t data)
{
    switch (_crypt_mode)
    {
        case crypp_mode::ECB:
            break;

        case crypp_mode::CBC:
            _tmp_vector = data;
            break;

        case crypp_mode::PCBC:
            _tmp_vector = data;
            break;

        case crypp_mode::CFB:
        case crypp_mode::OFB:
        case crypp_mode::CTR:
        case crypp_mode::Random_Delta:
            _tmp_vector = data;
            data = _vector;
            break;
    }
    return data;
}

uint64_t DES_coder::end_crypt_decode(uint64_t data)
{
    switch (_crypt_mode)
    {
        case crypp_mode::ECB:
            break;
            
        case crypp_mode::CBC:
            data ^= _vector;
            _vector = _tmp_vector;
            break;

        case crypp_mode::PCBC:
            data ^= _vector;
            _vector = _tmp_vector ^ data;
        break;

        case crypp_mode::CFB:
            _vector = _tmp_vector;
            data ^= _tmp_vector;
            break;

        case crypp_mode::OFB:
            _vector = data;
            data ^= _tmp_vector;
            break;

        case crypp_mode::CTR:
            data ^= _tmp_vector;
            ++_vector;
            break;

        case crypp_mode::Random_Delta:
            data ^= _tmp_vector;
            _vector += _delta_counter;
            break;
    }
    return data;
}
