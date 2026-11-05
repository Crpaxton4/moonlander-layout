/** @file
 * While and do loops at the top level, each holding one level of if and for.
 */

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
