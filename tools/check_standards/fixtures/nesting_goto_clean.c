/** @file
 * goto at the top level and as a for's one level, and labels, which add no level.
 */

int skip_to_end(int value) {
    for (int step = 0; step < value; step++) {
        goto end;
    }
    goto end;
end:
    return value;
}
