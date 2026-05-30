#pragma once

#include "data/Singleton.hpp"

namespace nitron
{

template <typename ValueType>
Singleton<ValueType>::AccessToken Singleton<ValueType>::get() {
    static ValueType value;
    static std::mutex mutex;
    return AccessToken(
        std::piecewise_construct,
        std::forward_as_tuple(value),
        std::forward_as_tuple(mutex)
    );
}

}