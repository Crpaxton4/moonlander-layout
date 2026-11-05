/** @file
 * A for loop holding one level of if and for, beside a for loop holding while, do
 * and switch, which belong at the top level only.
 */

// Allowed: an if and a for inside a for
int count_pairs(const int *values, int count) {
    int pairs = 0;
    for (int first = 0; first < count; first++) {
        if (values[first] < 0) {
            continue;
        }
        for (int second = first; second < count; second++) {
            pairs += values[second] == values[first];
        }
    }
    return pairs;
}

// Reported: a while, a do and a switch inside a for
int drain(int *values, int count) {
    for (int index = 0; index < count; index++) {
        while (values[index] > 0) {
            values[index]--;
        }
        do {
            values[index]++;
        } while (values[index] < 0);
        switch (values[index]) {
            case 0:
                break;
            default:
                return index;
        }
    }
    return count;
}
