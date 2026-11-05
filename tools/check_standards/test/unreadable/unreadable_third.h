#pragma once

/** A header checked after a file that can't be read, missing its docstring. */
static inline int after(int value) {
    while (value > 0) {
        while (value > 1) {
            value--;
        }
    }
    return value;
}
