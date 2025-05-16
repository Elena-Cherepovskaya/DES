#pragma once

#include "crypt_type.hpp"
#include "I_coder.hpp"

class stream_coder
{
    const int _block_size;
    byte_array _buffer_in;

public:
    stream_coder(int block_size = 16384)
         : _block_size(block_size)
    {
    }

    void code(I_coder * coder, std::istream & input, std::ostream & output)
    {
        byte_array buffer_out;
        while(true)
        {
            _buffer_in.resize(_block_size);            
            input.read(reinterpret_cast<char *>(&_buffer_in[0]), _buffer_in.size());
            auto size = input.gcount(); // Сколько реально прочитано
            if (size > 0)
            {
                _buffer_in.resize(size);
                coder->code(_buffer_in.cbegin(), _buffer_in.cend(), buffer_out);
                output.write(reinterpret_cast<char *>(&buffer_out[0]), buffer_out.size());
            }
            else
                break;
        }
    }

    void decode(I_coder * coder, std::istream & input, std::ostream & output)
    {
        byte_array buffer_out;
        while(true)
        {
            _buffer_in.resize(_block_size);            
            input.read(reinterpret_cast<char *>(&_buffer_in[0]), _buffer_in.size());
            auto size = input.gcount();
            if (size > 0)
            {
                _buffer_in.resize(size);
                coder->decode(_buffer_in.cbegin(), _buffer_in.cend(), buffer_out);
                output.write(reinterpret_cast<char *>(&buffer_out[0]), buffer_out.size());
            }
            else
                break;
        }
    }
};
