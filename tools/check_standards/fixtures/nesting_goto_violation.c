/** @file
 * goto inside an if and inside a case, where no control flow may go.
 */

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
