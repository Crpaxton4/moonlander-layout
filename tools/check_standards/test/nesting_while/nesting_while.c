/** @file
 * While and do loops at the top level, each holding one level of if and for, beside
 * while and do loops nested in loops, where only if and for may go.
 */

// Allowed: an if and a for inside a while, and an if inside a do
int settle(int value) {
    while (value > 0) {
        if (value % 2 == 0) {
            value /= 2;
        }
        for (int step = 0; step < 3; step++) {
            value--;
        }
    }
    do {
        if (value < 0) {
            value++;
        }
    } while (value < 0);
    return value;
}

// Reported: a while and a do inside a while, and a while inside a do
int spin(int value) {
    while (value > 0) {
        while (value > 1) {
            value--;
        }
        do {
            value--;
        } while (value > 2);
    }
    do {
        while (value < 0) {
            value++;
        }
    } while (value < 0);
    return value;
}
