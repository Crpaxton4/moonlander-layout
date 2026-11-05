/** @file
 * Flat branches: early returns, an else-if chain and blocks without control flow,
 * beside control flow nested inside an if, its else and an else-if branch.
 */

// Allowed: an else-if chain and blocks without control flow
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

// Allowed: an early return and a block without control flow
int clamp(int value, int limit) {
    if (value > limit) {
        return limit;
    }
    {
        value++;
    }
    return value - 1;
}

// Reported: an if and a for inside an if, an if inside an else and a while inside
// an else-if branch
int nested(int value) {
    if (value > 0) {
        if (value > 1) {
            return 2;
        }
        for (int step = 0; step < value; step++) {
            value--;
        }
    } else {
        if (value < 0) {
            return -1;
        }
    }
    if (value == 0) {
        return 0;
    } else if (value < 0) {
        while (value < 0) {
            value++;
        }
    }
    return value;
}
