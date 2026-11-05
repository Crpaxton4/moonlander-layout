/** @file
 * Conditional expressions where an if may go, and outside function bodies.
 */

#define LIMIT 3

static const int levels[] = {LIMIT > 2 ? (LIMIT > 4 ? 2 : 1) : 0};

int pick(int value) {
    int chosen = value > 0 ? value : -value;
    for (int step = 0; step < chosen; step++) {
        chosen -= step > 1 ? 1 : 0;
    }
    return chosen + levels[0];
}
