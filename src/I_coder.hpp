#pragma once

#include "crypt_type.hpp"
#include <cstddef>

enum class crypp_mode
{
    ECB, CBC, PCBC, CFB, OFB, CTR,
    Random_Delta
};

enum class padding_mode
{
    Zeros, ANSI_X923, PKCS7, ISO_10126
};

class I_coder
{
    public:
        virtual ~I_coder() = default;

    public:
        virtual byte_array code(byte_array const & in) = 0;
        virtual byte_array decode(byte_array const & in) = 0;

    protected:
        I_coder() = default;
};

template<typename T_block>
class coder_detail : public I_coder
{
protected:
    coder_detail(padding_mode padding_mode) : _padding_mode(padding_mode)
    {        
    }
    
    T_block get_block(byte_array::const_iterator & it, byte_array::const_iterator const & it_end)
    {
        T_block res = 0;
        int byte_counter = 0;
        for(auto i = 0; i < sizeof(T_block); ++i)
        {
            if(it != it_end)
            {
                res <<= 8;
                res |= (*it);
                ++byte_counter;
                ++it;
            }
        }
        if (byte_counter != sizeof(T_block))
            res = padding(res, sizeof(T_block) - byte_counter);
        return res;
    }


    T_block padding(T_block data, int add_byte) const
    {
        switch (_padding_mode)
        {
            case padding_mode::Zeros:
                for (int i = 0; i < add_byte; ++i)
                    data <<= 8;
                break;
    
            case padding_mode::ANSI_X923:
                for (int i = 0; i < add_byte - 1; ++i)
                    data <<= 8;
                data <<= 8;
                data |= add_byte;
                break;
    
            case padding_mode::ISO_10126:
                for (int i = 0; i < add_byte - 1; ++i)
                {
                    data <<= 8;
                    data |= rand() % 256;
                }
                data <<= 8;
                data |= add_byte;
                break;
            
            case padding_mode::PKCS7:
            for (int i = 0; i < add_byte; ++i)
                {
                    data <<= 8;
                    data |= add_byte;
                }
                break;
            
        }
        return data;
    }
    
    
    padding_mode  _padding_mode;
};
