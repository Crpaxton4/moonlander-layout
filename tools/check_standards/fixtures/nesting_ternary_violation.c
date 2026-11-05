/** @file
 * Conditional expressions inside a conditional expression, an if and a case.
 */

int choose(int value) {
    int chosen = value > 0 ? (value > 1 ? 2 : 1) : 0;
    if (value > chosen ? value : chosen) {
        return value < 0 ? -1 : 1;
    }
    switch (value) {
        case 0:
            return chosen ? 1 : 0;
        default:
            break;
    }
    return chosen;
}
