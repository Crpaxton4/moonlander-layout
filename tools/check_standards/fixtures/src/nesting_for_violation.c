/** @file
 * A for loop holding while, do and switch, which belong at the top level only.
 */

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
