/** @file
 * Loops two levels deep, the most allowed, with plain statements inside.
 */

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
