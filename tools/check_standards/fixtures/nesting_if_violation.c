/** @file
 * Control flow nested inside an if, its else and an else-if branch.
 */

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
