/** @file
 * Conditional expressions where an if may go and outside function bodies, beside
 * conditional expressions inside a conditional expression, an if and a case.
 */

#define LIMIT 3

// Allowed: a nested conditional expression outside function bodies
static const int levels[] = {LIMIT > 2 ? (LIMIT > 4 ? 2 : 1) : 0};

// Allowed: a conditional expression at the top level and inside a for
int pick(int value) {
    int chosen = value > 0 ? value : -value;
    for (int step = 0; step < chosen; step++) {
        chosen -= step > 1 ? 1 : 0;
    }
    return chosen + levels[0];
}

// Reported: conditional expressions inside a conditional expression, an if
// condition, an if body and a case
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
