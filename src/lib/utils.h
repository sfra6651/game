#pragma once

#include <iostream>

template<typename ...Args>
inline void println(Args&&... args) {
    int i = 0;
    ((std::cout << (i++ ? " " : "") << args), ...);
    std::cout << "\n";
}
