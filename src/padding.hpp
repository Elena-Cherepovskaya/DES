#pragma once

enum class padding_mode
{
    Zeros, ANSI_X923, PKCS7, ISO_10126
};

template<typename T_block>
class padding
{
protected:
    explicit padding(padding_mode padding_mode = padding_mode::Zeros)
        : _padding_mode(padding_mode)
    {}
    
public:
    void set_padding(padding_mode padding_mode)
    {
        _padding_mode = padding_mode;
    }

protected:    
    template <typename T>
    static T get_output_size(T input_size)
    {
        if (input_size % sizeof(T_block) > 0)
            return (input_size / sizeof(T_block) + 1) * sizeof(T_block);
        return input_size;
    }

    T_block get_block(byte_array::const_iterator & it, byte_array::const_iterator const & it_end) const
    {
        T_block res = 0;
        int byte_counter = 0;
        for(size_t i = 0; i < sizeof(T_block); ++i)
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
            res = padding_block(res, sizeof(T_block) - byte_counter);

        return res;
    }

private:
    T_block padding_block(T_block data, int add_byte) const
    {
        switch (_padding_mode)
        {
            case padding_mode::Zeros:
                for (int i = 0; i < add_byte; ++i)
                    data <<= 8;
                break;
    
            case padding_mode::ANSI_X923:
                for (int i = 0; i < add_byte; ++i)
                    data <<= 8;
                data |= add_byte;
                break;
    
            case padding_mode::ISO_10126:
                for (int i = 0; i < add_byte - 1 ; ++i)
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
