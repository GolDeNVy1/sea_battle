#include <cstdlib>
#include <ctime>
#include "utils.h"

int getRandomNumber(int min, int max) {
    static bool first = true;
    if (first) {
        srand(static_cast<unsigned int>(time(0)));
        first = false;
    }
    return rand() % (max - min + 1) + min;
}
