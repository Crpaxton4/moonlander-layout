/** @file
 * Preprocessor conditionals add no level: the if inside is a for's one level, the
 * second if is still inside an if.
 */

int tune(int value) {
    for (int step = 0; step < value; step++) {
#if defined(FAST)
        if (step > 1) {
            break;
        }
#endif
    }
    if (value > 0) {
#ifdef SLOW
        if (value > 1) {
            value--;
        }
#endif
    }
    return value;
}
