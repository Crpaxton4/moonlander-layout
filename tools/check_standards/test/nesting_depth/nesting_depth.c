/** @file
 * Control flow two levels deep, the most allowed, beside control flow three levels
 * deep inside loops that allow it one level at a time.
 */

// Allowed: loops two levels deep, with plain statements inside
int sum_grid(int grid[3][3]) {
    int sum = 0;
    for (int row = 0; row < 3; row++) {
        for (int column = 0; column < 3; column++) {
            sum += grid[row][column];
        }
    }
    while (sum > 100) {
        for (int step = 0; step < 10; step++) {
            sum--;
        }
    }
    return sum;
}

// Reported: a for and an if three levels deep
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
