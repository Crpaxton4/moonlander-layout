/** @file
 * While and do loops nested in loops, where only if and for may go.
 */

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
