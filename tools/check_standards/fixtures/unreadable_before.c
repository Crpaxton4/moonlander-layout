/** @file
 * A file checked before one that can't be read.
 */

int before(int value) {
    if (value > 0) {
        if (value > 1) {
            return 2;
        }
    }
    return value;
}
