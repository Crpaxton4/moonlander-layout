/** @file
 * A switch at the top level whose cases hold no control flow, beside control flow
 * inside cases and a switch below the top level.
 */

// Allowed: cases that hold statements, a block and exits only
int weight(int kind) {
    int result = 0;
    switch (kind) {
        case 0:
        case 1:
            result = 1;
            break;
        case 2: {
            int doubled = kind * 2;
            result      = doubled;
            break;
        }
        default:
            return -1;
    }
    return result;
}

// Reported: an if, a for and a switch inside a case, and a switch inside a for
int weigh(int kind, int pressed) {
    switch (kind) {
        case 0:
            if (pressed) {
                return 1;
            }
            return 0;
        case 1: {
            for (int step = 0; step < kind; step++) {
                pressed++;
            }
            break;
        }
        default:
            switch (pressed) {
                default:
                    break;
            }
    }
    for (int step = 0; step < kind; step++) {
        switch (step) {
            default:
                break;
        }
    }
    return pressed;
}
