/** @file
 * Control flow inside cases, and a switch below the top level.
 */

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
