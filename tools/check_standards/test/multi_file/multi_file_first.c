/** @file
 * The first of two files with findings: one in a function and one more after it.
 */

int first(int value) {
    if (value > 0) {
        if (value > 1) {
            return 2;
        }
    }
    return value;
}

int again(int value) {
    for (int step = 0; step < value; step++) {
        while (step > 1) {
            step--;
        }
    }
    return value;
}
