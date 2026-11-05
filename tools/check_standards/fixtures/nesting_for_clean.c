/** @file
 * A for loop holding one level of if and for.
 */

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
