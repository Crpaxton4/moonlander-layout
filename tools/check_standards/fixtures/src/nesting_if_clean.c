/** @file
 * Flat branches: early returns, an else-if chain and blocks without control flow.
 */

int sign(int value) {
    if (value > 0) {
        return 1;
    } else if (value < 0) {
        return -1;
    } else {
        value = 0;
    }
    return value;
}

int clamp(int value, int limit) {
    if (value > limit) {
        return limit;
    }
    {
        value++;
    }
    return value - 1;
}
