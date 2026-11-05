/** @file
 * Control flow three levels deep inside loops that allow it one level at a time.
 */

int count_positive(int cube[3][3][3]) {
    int count = 0;
    for (int layer = 0; layer < 3; layer++) {
        for (int row = 0; row < 3; row++) {
            for (int column = 0; column < 3; column++) {
                count += cube[layer][row][column] > 0;
            }
            if (cube[layer][row][0] < 0) {
                return -1;
            }
        }
    }
    return count;
}
