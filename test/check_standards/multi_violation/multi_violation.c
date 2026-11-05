// Several findings in one function and across functions, all reported in order

int first(int value) {
    if (value > 0) {
        if (value > 1) {
            return 2;
        }
        while (value > 2) {
            value--;
        }
    }
    return value > 0 ? 1 : 0;
}

int second(int value) {
    switch (value) {
        case 0:
            for (int step = 0; step < 3; step++) {
                value += step;
            }
            break;
        default:
            if (value < 0) {
                return -1;
            }
    }
    return value;
}

int third(int value) {
    for (int outer = 0; outer < value; outer++) {
        for (int inner = 0; inner < outer; inner++) {
            if (inner == outer) {
                return inner;
            }
        }
    }
    return 0;
}
