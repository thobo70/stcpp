/**
 * @file test_basic.c
 * @brief Basic macro functionality tests
 */

// Simple macros
#define PI 3.14159
#define VERSION "1.0"
#define DEBUG 1

// Function macros
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define SQUARE(x) ((x) * (x))

int main() {
    double radius = PI * 2.0;
    char *ver = VERSION;
    int flag = DEBUG;
    
    int max_val = MAX(10, 20);
    int min_val = MIN(5, 3);
    int sq = SQUARE(4);
    
    return 0;
}
