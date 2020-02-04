#pragma once

namespace Clipped
{
    /**
     * @brief reflect reverses the bit order.
     * @param value
     * @return
     */
    template <typename T>
    T reflect(const T value)
    {
        T result = 0;

        for(size_t i = 0; i < sizeof(T) * 8; i++) //For every bit
        {
            if ((value & ((T)1 << i)) != 0)
            {
                result |= (T)((T)1 << ((sizeof(T) * 8 - 1) - i));
            }
        }
        return result;
    }
} //namespace Clipped
