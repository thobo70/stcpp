/**
 * @file version.h
 * @author Thomas Boos (tboos70@gmail.com)
 * @brief Version information for stcpp
 * @version 0.1
 * @date 2024-08-07
 * 
 * @copyright Copyright (c) 2024
 * 
 * This header provides version information extracted from Git during build time.
 * The version string is automatically generated from Git tags and commit information.
 */

#ifndef VERSION_H
#define VERSION_H

// Version information is injected at compile time via Makefile
#ifndef VERSION_STRING
#define VERSION_STRING "unknown"
#endif

#ifndef BUILD_DATE
#define BUILD_DATE "unknown"
#endif

/**
 * @brief Gets the version string.
 * 
 * Returns the version string that was extracted from Git during build time.
 * Format examples:
 * - "v1.0.0" (clean tagged release)
 * - "v1.0.0-5-g1234567" (5 commits after v1.0.0 tag)
 * - "v1.0.0-5-g1234567-dirty" (uncommitted changes)
 * - "1234567" (no tags, commit hash only)
 * - "unknown" (no Git information available)
 * 
 * @return Constant string containing version information.
 */
const char* get_version_string(void);

/**
 * @brief Gets the build date string.
 * 
 * Returns the date and time when the binary was compiled.
 * 
 * @return Constant string containing build date in UTC format.
 */
const char* get_build_date(void);

/**
 * @brief Prints version information to stdout.
 * 
 * Displays formatted version and build information.
 */
void print_version_info(void);

#endif // VERSION_H
