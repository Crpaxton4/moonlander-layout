/** @file
 * goto at the top level and as a for's one level, and labels, which add no level,
 * beside goto inside an if and inside a case, where no control flow may go.
 */

// Allowed: a goto inside a for, a goto at the top level and a label
int skip_to_end(int value) {
    for (int step = 0; step < value; step++) {
        goto end;
    }
    goto end;
end:
    return value;
}

// Reported: a goto inside an if and a goto inside a case
int retry(int value) {
again:
    if (value > 0) {
        value--;
        goto again;
    }
    switch (value) {
        case 0:
            goto again;
        default:
            break;
    }
    return value;
}
