# STCPP Test Suite

## 🎯 Unity Test Framework (ACTIVE)

The main test suite is now located in `unity_tests/` and provides comprehensive testing using the Unity testing framework.

### Running Tests
```bash
cd unity_tests/
make test-comprehensive  # Run all tests
make help               # See all available options
```

### Test Coverage
The Unity framework provides **complete coverage** of all STCPP functionality:
- ✅ **26 test files** fully covered with enhanced testing
- ✅ **All shell script validations** integrated
- ✅ **Professional-grade error reporting**
- ✅ **Memory safety validation**
- ✅ **Coverage analysis**

### Features Tested
- Basic macro expansion
- Function-like macros  
- Recursive macro expansion
- Token pasting (`##` operator)
- Stringification (`#` operator)
- Conditional compilation (`#if`, `#ifdef`, `#ifndef`)
- Built-in macros (`__LINE__`, `__FILE__`)
- Include functionality
- Command line options (`-D`, `-U`, `-I`)
- Line directives
- Edge cases and error conditions

## 📦 Archived Legacy Tests

Old test files have been moved to `archive_old_tests/` for reference:
- All `test_*.c` and `test_*.h` files
- Shell scripts (`run_tests.sh`, `test_*.sh`)
- Legacy test infrastructure

These are **no longer used** but preserved for historical reference.

## 🔧 Current Status

The Unity framework has identified several bugs in the STCPP implementation that need to be fixed:
1. Stringification operator issues
2. Token pasting problems
3. `defined()` operator not working
4. `__LINE__` macro off-by-one errors
5. Command line option parsing bugs
6. Quote handling failures
7. `-U` option not functioning

See `unity_tests/UNITY_TEST_RESULTS.md` for detailed issue reports.

## 🚀 Benefits of Unity Framework

- **Enhanced Testing**: Found bugs that manual tests missed
- **Professional Quality**: Industry-standard testing framework
- **Better Debugging**: Detailed failure information
- **Automated Validation**: Integrated with build system
- **Comprehensive Coverage**: No functionality left untested

---

**To run tests**: `cd unity_tests && make test-comprehensive`  
**For help**: `cd unity_tests && make help`
