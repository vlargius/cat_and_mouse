#pragma once

template<int LastValue, int Count = 0>
struct bits_required {
    enum {value = bits_required<(LastValue >> 1), Count + 1>::value};
};

template<int Count>
struct bits_required<0, Count>
{
 enum {value = Count};
};
