#pragma once

#include "crypt_type.hpp"

enum class crypt_mode
{
    ECB, CBC, PCBC, CFB, OFB, CTR,
    Random_Delta
};


class I_coder
{
    protected:
        I_coder() = default;

    public:
        virtual ~I_coder() = default;

        virtual void set_crypt_mode(crypt_mode c_mode) = 0;

        virtual byte_array code(byte_array::const_iterator it_begin, byte_array::const_iterator const & it_end) const = 0;

        virtual void code(byte_array::const_iterator it, byte_array::const_iterator const & it_end, byte_array & out) const = 0;

        virtual byte_array decode(byte_array::const_iterator it_begin, byte_array::const_iterator const & it_end) const = 0;

        virtual void decode(byte_array::const_iterator it, byte_array::const_iterator const & it_end, byte_array & out) const = 0;
};

template <typename T_data>
class coder_type : public I_coder
{
public:
    void set_start_vector(T_data const & start_vector)
    {
        _start_vector = start_vector;
    }

protected:
    T_data _start_vector = 0;
};
