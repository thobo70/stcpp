/**
 * @file test_conditionals.c
 * @brief Conditional compilation and macro-based conditions
 */

#define FEATURE_A 1
#define FEATURE_B 0
#define VERSION_MAJOR 2
#define VERSION_MINOR 1

// Recursive macro for conditional
#define ENABLED FEATURE_A
#define CONFIG ENABLED

int main() {
    // Test basic conditionals
    #if FEATURE_A
    int feature_a_code = 1;
    #endif
    
    #if FEATURE_B
    int feature_b_code = 1;
    #else
    int no_feature_b = 0;
    #endif
    
    // Test complex conditions
    #if (VERSION_MAJOR > 1) && (VERSION_MINOR >= 0)
    int version_check = 1;
    #endif
    
    // Test recursive macro in conditional
    #if CONFIG
    int config_enabled = 1;
    #endif
    
    // Test defined() operator
    #if defined(FEATURE_A)
    int feature_a_defined = 1;
    #endif
    
    #ifdef FEATURE_A
    int feature_a_ifdef = 1;
    #endif
    
    #ifndef UNDEFINED_FEATURE
    int undefined_feature_test = 1;
    #endif
    
    return 0;
}
