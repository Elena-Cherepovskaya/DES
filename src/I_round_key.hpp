#pragma once

#include <vector>

template <typename T>
class I_round_key
{
    public:
        virtual ~I_round_key() = default;

    public:
        virtual std::vector<T> get_round_keys() const = 0;
};