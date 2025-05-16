#pragma once

#include "I_coder.hpp"
#include "I_round_key.hpp"
#include "padding.hpp"
#include "help_function.hpp"

#include <memory>
#include <future>
#include <iostream>

template <typename T_key, typename T_data, typename T_round_key, typename T_coder_function, typename T_decoder_function>
class base_coder : public coder_type<T_data>, public padding<T_data>
{
public:
    base_coder() = default;

    void set_crypt_mode(crypt_mode c_mode) override
    {
        _crypt_mode = c_mode;
    }

    void set_delta_counter(T_data delta_counter)
    {
         _delta_counter = delta_counter;
    }

    void set_key(T_key const && key)
    {
         _round_key = std::make_unique<T_round_key>(key);
    }
    
    void set_key(std::string const & key)
    {
         _round_key = std::make_unique<T_round_key>(string_to_uint<T_key>(key));
    }
    
    void set_number_thread(size_t number_thread)
    {
         _number_thread = number_thread;
    }

private:
    T_data code_chain(byte_array::const_iterator it, byte_array::const_iterator const & it_end,
        byte_array::iterator it_output, std::shared_ptr<std::future<T_data>> f_vector, T_data && counter) const
    {
        bool first = true;
        T_data _tmp_vector = 0;
        T_data _vector; // Будет инициализирован на первом блоке
        while(it != it_end)
        {
            T_data data = this->get_block(it, it_end);
            if (first)
            {
                data = begin_crypt_code(std::move(data), f_vector, _tmp_vector,  counter);
                first = false;
            }
            else
                data = begin_crypt_code(std::move(data), _vector, _tmp_vector, counter);

            auto crypt_data = f_coder.coder(data, *_round_key);
            crypt_data = end_crypt_code(std::move(crypt_data), _vector, _tmp_vector);

            set_as_byte(it_output, crypt_data);
            std::advance(it_output, sizeof(T_data));
        }
        return _vector;
    }

public:
    void code(byte_array::const_iterator it, byte_array::const_iterator const & it_end, byte_array & out) const override
    {
        out.resize(this->get_output_size(std::distance(it, it_end)));
        byte_array::iterator it_output = out.begin();

        std::future<T_data> f_begin = std::async(std::launch::deferred, []()
        { 
            return T_data(0);
        });
        std::shared_ptr<std::future<T_data>> f = std::make_shared<std::future<T_data>>(std::move(f_begin));

        auto size = std::distance(it, it_end);
        long step = ((size / _number_thread) / sizeof(T_data) + 1) * sizeof(T_data);
        T_data counter = this->_start_vector;
        while(it != it_end)
        {
            if (std::distance(it, it_end) > step) // length = it_end - it;
            {
                auto it_next = it;
                std::advance(it_next, step); // it_next += step;
                auto next_f = std::async(std::launch::async, [&, it, it_next, it_output, f, counter] ()
                {
                    T_data cc = counter;
                    return code_chain(it, it_next, it_output, f, std::move(cc));
                });
                f = std::make_shared<std::future<T_data>>(std::move(next_f));
                std::advance(it_output, step);
                it = it_next;
            }
            else
            {
                auto last_f = std::async(std::launch::async, [&, it, it_end, it_output, f, counter] ()
                {
                    T_data cc = counter;
                    return code_chain(it, it_end, it_output, f, std::move(cc));
                });
                it = it_end;
            }
            counter += (step / sizeof(T_data)) * _delta_counter;
        }
    }

    byte_array code(byte_array::const_iterator it, byte_array::const_iterator const & it_end) const override
    {
        byte_array res;

        T_data prev_vector = 0;
        T_data tmp_vector = 0;
        T_data counter = this->_start_vector;
        while(it != it_end)
        {
            T_data data = this->get_block(it, it_end);
            data = begin_crypt_code(std::move(data), prev_vector, tmp_vector, counter);
            auto crypt_data = f_coder.coder(data, *_round_key);
            crypt_data = end_crypt_code(std::move(crypt_data), prev_vector, tmp_vector);
            push_as_byte(res, crypt_data);
        }
        return res;
    }

private:
    T_data decode_chain(byte_array::const_iterator it, byte_array::const_iterator const & it_end,
        byte_array::iterator it_output, 
        T_data const & start_vector,
        std::shared_ptr<std::future<T_data>> f) const
    {
        bool first = true;

        T_data tmp_vector = start_vector;
        T_data prev_vector = 0;  // Будет инициализирован на первом блоке
        T_data decrypt_data = 0;
        T_data counter = start_vector;
        while(it != it_end)
        {              
            T_data data = this->get_block(it, it_end);
            if (first)
                data = begin_crypt_decode(std::move(data), f, tmp_vector, counter);
            else
                data = begin_crypt_decode(std::move(data), prev_vector, tmp_vector, counter);

            if (_crypt_mode == crypt_mode::CFB ||
                _crypt_mode == crypt_mode::OFB ||
                _crypt_mode == crypt_mode::CTR ||
                _crypt_mode == crypt_mode::Random_Delta)
                decrypt_data = f_coder.coder(data, *_round_key);
            else
                decrypt_data = f_decoder.coder(data, *_round_key);

            if (first)
            {
                decrypt_data = end_crypt_decode(std::move(decrypt_data), f, prev_vector, tmp_vector, counter);
                first = false;
            }
            else
                decrypt_data = end_crypt_decode(std::move(decrypt_data), prev_vector, tmp_vector, counter);
            
            set_as_byte(it_output, decrypt_data);
            std::advance(it_output, sizeof(T_data));
        }
        
        return prev_vector;            
    }

public:
    void decode(byte_array::const_iterator it, byte_array::const_iterator const & it_end, byte_array & out) const override
    {
        out.resize(std::distance(it, it_end));
        byte_array::iterator it_output = out.begin();

        auto f_begin = std::async(std::launch::deferred, [&]()
        { 
            return T_data(0);
        });
        std::shared_ptr<std::future<T_data>> f = std::make_shared<std::future<T_data>>(std::move(f_begin));

        auto size = std::distance(it, it_end);
        long step = ((size / _number_thread) / sizeof(T_data) + 1) * sizeof(T_data);

        T_data counter = this->_start_vector;
        while(it != it_end)
        {
            if (std::distance(it, it_end) > step)
            {
                auto it_next = it;
                std::advance(it_next, step);
                auto next_f = std::async(std::launch::async, [&, it, it_next, it_output, counter, f] ()
                {
                    return decode_chain(it, it_next, it_output, counter, f);
                });
                f = std::make_shared<std::future<T_data>>(std::move(next_f));
                std::advance(it_output, step);
                it = it_next;
                counter += (step / sizeof(T_data)) * _delta_counter;
            }
            else
            {
                auto next_f = std::async(std::launch::async, [&, it, it_end, it_output, counter, f] ()
                {
                    return decode_chain(it, it_end, it_output, counter, f);
                });
                f = std::make_shared<std::future<T_data>>(std::move(next_f));
                it = it_end;
                counter += (step / sizeof(T_data)) * _delta_counter;
            }
        }        
    }

    byte_array decode(byte_array::const_iterator it, byte_array::const_iterator const & it_end) const override
    {
        byte_array res;
    
        T_data tmp_vector;
        T_data prev_vector = 0;
        T_data counter = this->_start_vector;
        while(it != it_end)
        {              
            T_data data = this->get_block(it, it_end);
            data = begin_crypt_decode(std::move(data), prev_vector, tmp_vector, counter);

            T_data decrypt_data = 0;                        
            if (_crypt_mode == crypt_mode::CFB ||
                _crypt_mode == crypt_mode::OFB ||
                _crypt_mode == crypt_mode::CTR ||
                _crypt_mode == crypt_mode::Random_Delta)
                decrypt_data = f_coder.coder(data, *_round_key);
            else
                decrypt_data = f_decoder.coder(data, *_round_key);

            decrypt_data = end_crypt_decode(std::move(decrypt_data), prev_vector, tmp_vector, counter);
    
            push_as_byte(res, decrypt_data);
        }
        
        return res;            
    }

private:
    T_data begin_crypt_code(T_data && data,
        std::shared_ptr<std::future<T_data>> const & prev_data, T_data & tmp_data, T_data & counter) const
    {
        switch (_crypt_mode)
        {
            case crypt_mode::CBC:
                data ^= prev_data->get();
                break;

            case crypt_mode::PCBC:
                tmp_data = data;
                data ^= prev_data->get();
                break;

            case crypt_mode::CFB:
            case crypt_mode::OFB:
                tmp_data = data;
                data = prev_data->get();
                break;

            case crypt_mode::CTR:
                tmp_data = data;
                data = counter;
                counter++;                
                break;

            case crypt_mode::Random_Delta:
                tmp_data = data;
                data = counter;
                counter += _delta_counter;
                break;

            default:
                break;
        }
        return data;
    }

    T_data begin_crypt_code(T_data && data,
        T_data const & prev_data, T_data & tmp_data, T_data & counter) const
    {
        switch (_crypt_mode)
        {
            case crypt_mode::CBC:
                data ^= prev_data;
                break;
    
            case crypt_mode::PCBC:
                tmp_data = data;
                data ^= prev_data;
                break;
    
            case crypt_mode::CFB:
            case crypt_mode::OFB:
                tmp_data = data;
                data = prev_data;
                break;

            case crypt_mode::CTR:
                tmp_data = data;
                data = counter;
                counter++;                
                break;

            case crypt_mode::Random_Delta:
                tmp_data = data;
                data = counter;
                counter += _delta_counter;
                break;
    
            default:
                break;
        }
        return data;
    }

    T_data end_crypt_code(T_data && data, T_data & prev_data, T_data & tmp_data) const
    {
        switch (_crypt_mode)
        {
            case crypt_mode::ECB:
                break;
    
            case crypt_mode::CBC:
                prev_data = data;
                break;
    
            case crypt_mode::PCBC:
                prev_data = data ^ tmp_data;
                break;
    
            case crypt_mode::CFB:
                data ^= tmp_data;
                prev_data = data;
                break;
    
            case crypt_mode::OFB:
                prev_data = data;
                data ^= tmp_data;
                break;
    
            case crypt_mode::CTR:
                prev_data = data;
                data ^= tmp_data;
                break;
    
            case crypt_mode::Random_Delta:
                data ^= tmp_data;
                break;
    
            default:
                break;
        }
        return data;            
    }


    T_data begin_crypt_decode(T_data && data,
        std::shared_ptr<std::future<T_data>> const & prev_data, T_data & tmp_data,
        T_data const & counter) const
    {
        switch (_crypt_mode)
        {
            case crypt_mode::ECB:
                break;
    
            case crypt_mode::CBC:
                tmp_data = data;
                break;
    
            case crypt_mode::PCBC:
                tmp_data = data;
                break;
    
            case crypt_mode::CFB:
            case crypt_mode::OFB:
                tmp_data = data;
                data = prev_data->get();
                break;

            case crypt_mode::CTR:
            case crypt_mode::Random_Delta:
                tmp_data = data;
                data = counter;
                break;
    
            default:
                break;
        }
        return data;
    }

    T_data begin_crypt_decode(T_data && data,
        T_data const & prev_data, T_data & tmp_data,
        T_data const & counter) const
    {
        switch (_crypt_mode)
        {
            case crypt_mode::ECB:
                break;
    
            case crypt_mode::CBC:
                tmp_data = data;
                break;
    
            case crypt_mode::PCBC:
                tmp_data = data;
                break;
    
            case crypt_mode::CFB:
            case crypt_mode::OFB:
                tmp_data = data;
                data = prev_data;
                break;

            case crypt_mode::CTR:
            case crypt_mode::Random_Delta:
                tmp_data = data;
                data = counter;
                break;
    
            default:
                break;
        }
        return data;
    }

    T_data end_crypt_decode(T_data && data, 
        std::shared_ptr<std::future<T_data>> const & in_vector,
        T_data & out_vector, T_data const & tmp_vector,
        T_data & counter) const
    {
        switch (_crypt_mode)
        {
            case crypt_mode::ECB:
                break;
                
            case crypt_mode::CBC:
                data ^= in_vector->get();
                out_vector = tmp_vector;
                break;
    
            case crypt_mode::PCBC:
                data ^= in_vector->get();
                out_vector = tmp_vector ^ data;
            break;
    
            case crypt_mode::CFB:
                out_vector = tmp_vector;
                data ^= tmp_vector;
                break;
    
            case crypt_mode::OFB:
                out_vector = data;
                data ^= tmp_vector;
                break;
    
            case crypt_mode::CTR:
                data ^= tmp_vector;
                ++counter;
                out_vector = counter;
                break;
    
            case crypt_mode::Random_Delta:
                data ^= tmp_vector;
                counter += _delta_counter;
                out_vector = counter;
                break;
            
            default:
                break;
        }
        return data;
    }

    T_data end_crypt_decode(T_data && data, T_data & _vector, T_data const & _tmp_vector, T_data & counter) const
    {
        switch (_crypt_mode)
        {
            case crypt_mode::ECB:
                break;
                
            case crypt_mode::CBC:
                data ^= _vector;
                _vector = _tmp_vector;
                break;
    
            case crypt_mode::PCBC:
                data ^= _vector;
                _vector = _tmp_vector ^ data;
            break;
    
            case crypt_mode::CFB:
                _vector = _tmp_vector;
                data ^= _tmp_vector;
                break;
    
            case crypt_mode::OFB:
                _vector = data;
                data ^= _tmp_vector;
                break;
    
            case crypt_mode::CTR:
                data ^= _tmp_vector;
                ++counter;
                _vector = counter;
                break;
    
            case crypt_mode::Random_Delta:
                data ^= _tmp_vector;
                counter += _delta_counter;
                _vector = counter;
                break;
            
            default:
                break;
        }
        return data;
    }    

    int _number_thread = 8;
    std::unique_ptr<T_round_key> _round_key;
    T_coder_function   f_coder;
    T_decoder_function f_decoder;
    T_data             _delta_counter = 1;

    crypt_mode _crypt_mode = crypt_mode::ECB;
};
