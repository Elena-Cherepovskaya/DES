#pragma once

#include <vector>

template <typename T>
class I_round_key
{
    protected:
        virtual ~I_round_key() = default;

    public:
        virtual std::vector<T> const & get_round_keys() const = 0;
};