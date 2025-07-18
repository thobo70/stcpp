//
// Simple test for -D command line option functionality
//

#ifdef DEBUG
int debug_mode = DEBUG;
#endif

#ifdef VERBOSE  
int verbose_mode = VERBOSE;
#endif

#ifndef DEBUG
int debug_disabled = 1;
#endif

// Regular code that should always be processed
int always_present = 42;
