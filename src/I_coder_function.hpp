#pragma once

#include "I_round_key.hpp"

template <typename T_data, typename T_key>
class I_coder_function
{
    protected:
        virtual ~I_coder_function() = default;

    public:
        virtual T_data coder(T_data const & in, I_round_key<T_key> const  & round_key) const = 0;
};
