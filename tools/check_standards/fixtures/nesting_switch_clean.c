/** @file
 * A switch at the top level whose cases hold no control flow.
 */

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
