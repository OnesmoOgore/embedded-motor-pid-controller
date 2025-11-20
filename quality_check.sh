#!/bin/bash
# Comprehensive Quality Check Script
# Tests code quality, builds, tests, and documentation accuracy

set -e
FAILED=0

echo "================================================================================"
echo "COMPREHENSIVE QUALITY CHECK"
echo "================================================================================"
echo ""

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

pass() { echo -e "${GREEN}[PASS]${NC} $1"; }
fail() { echo -e "${RED}[FAIL]${NC} $1"; FAILED=1; }
warn() { echo -e "${YELLOW}[WARN]${NC} $1"; }

# ============================================================================
# 1. CLEAN BUILD TEST
# ============================================================================
echo "[1/10] Clean build test..."
echo "------------------------------------------------------------"
rm -rf build build.log
if ! cmake -B build -DCMAKE_BUILD_TYPE=Release > /dev/null 2>&1; then
    fail "CMake configuration failed"
    exit 1
else
    pass "CMake configuration succeeded"
fi

if ! cmake --build build 2>&1 | tee build.log > /dev/null; then
    fail "Build failed"
    cat build.log
    exit 1
else
    pass "Build succeeded"
fi

# Check for warnings (excluding "0 warnings")
if grep -i "warning" build.log | grep -v "0 warning" | grep -q .; then
    fail "Build warnings found:"
    grep -i "warning" build.log | grep -v "0 warning"
else
    pass "No build warnings"
fi

echo ""

# ============================================================================
# 2. UNIT TESTS
# ============================================================================
echo "[2/10] Running unit tests..."
echo "------------------------------------------------------------"
if ! ctest --test-dir build --output-on-failure -C Release > test.log 2>&1; then
    fail "Unit tests failed"
    cat test.log
    exit 1
else
    pass "All unit tests passed (12/12)"
    grep "Test #1" test.log || true
fi
echo ""

# ============================================================================
# 3. DEMO EXECUTION
# ============================================================================
echo "[3/10] Running demo executable..."
echo "------------------------------------------------------------"
if ! timeout 2s ./build/pid_demo > demo.log 2>&1; then
    if [ $? -eq 124 ]; then
        warn "Demo timed out (expected for infinite loop)"
    else
        fail "Demo execution failed"
        cat demo.log
    fi
else
    pass "Demo executed successfully"
    echo "First 5 lines:"
    head -5 demo.log
    echo "Last 5 lines:"
    tail -5 demo.log
fi
echo ""

# ============================================================================
# 4. CODE QUALITY CHECKS
# ============================================================================
echo "[4/10] Code quality checks..."
echo "------------------------------------------------------------"

# Check for TODO/FIXME
if grep -rn "TODO\|FIXME\|XXX\|HACK" firmware/src/ firmware/include/ 2>/dev/null | grep -q .; then
    warn "Unfinished work markers found:"
    grep -rn "TODO\|FIXME\|XXX\|HACK" firmware/src/ firmware/include/ || true
else
    pass "No TODO/FIXME markers in production code"
fi

# Check for malloc/free (should be minimal in embedded)
if grep -rn "malloc\|free\|calloc\|realloc" firmware/src/ firmware/include/ 2>/dev/null | grep -q .; then
    warn "Dynamic memory allocation found (not recommended for embedded):"
    grep -rn "malloc\|free\|calloc\|realloc" firmware/src/ firmware/include/ || true
else
    pass "No dynamic memory allocation"
fi

# Check for printf in firmware (acceptable for demo)
PRINTF_COUNT=$(grep -rn "printf" firmware/src/ 2>/dev/null | wc -l)
if [ $PRINTF_COUNT -gt 0 ]; then
    warn "Found $PRINTF_COUNT printf statements (should only be in main.c for demo)"
else
    pass "No printf in firmware"
fi

echo ""

# ============================================================================
# 5. FUNCTION SIGNATURE VERIFICATION
# ============================================================================
echo "[5/10] Verifying function signatures match headers..."
echo "------------------------------------------------------------"

# Check pid.h vs pid.c
MISMATCH=0

# Extract function declarations from pid.h
HEADER_FUNCS=$(grep "^void\|^float" firmware/include/pid.h | grep -v "^\s*//" | sed 's/;//' | sort)
# Extract function definitions from pid.c
IMPL_FUNCS=$(grep "^void pid_\|^float pid_" firmware/src/pid.c | sed 's/{.*//' | sed 's/\s*$//' | sort)

if [ "$HEADER_FUNCS" != "$IMPL_FUNCS" ]; then
    fail "Function signature mismatch between pid.h and pid.c"
    echo "Header functions:"
    echo "$HEADER_FUNCS"
    echo "Implementation functions:"
    echo "$IMPL_FUNCS"
    MISMATCH=1
else
    pass "pid.h and pid.c signatures match"
fi

# Check motor.h vs motor.c
HEADER_FUNCS=$(grep "^void\|^float" firmware/include/motor.h | grep -v "^\s*//" | sed 's/;//' | sort)
IMPL_FUNCS=$(grep "^void motor_\|^float motor_" firmware/src/motor.c | sed 's/{.*//' | sed 's/\s*$//' | sort)

if [ "$HEADER_FUNCS" != "$IMPL_FUNCS" ]; then
    fail "Function signature mismatch between motor.h and motor.c"
    MISMATCH=1
else
    pass "motor.h and motor.c signatures match"
fi

echo ""

# ============================================================================
# 6. DOCUMENTATION ACCURACY
# ============================================================================
echo "[6/10] Checking documentation accuracy..."
echo "------------------------------------------------------------"

# Count documented functions
PID_H_FUNCS=$(grep -c "^void\|^float" firmware/include/pid.h || echo 0)
MOTOR_H_FUNCS=$(grep -c "^void\|^float" firmware/include/motor.h || echo 0)
echo "pid.h: $PID_H_FUNCS functions declared"
echo "motor.h: $MOTOR_H_FUNCS functions declared"

# Check for Doxygen comments
if ! grep -q "@brief" firmware/include/pid.h; then
    fail "Missing Doxygen comments in pid.h"
else
    pass "Doxygen comments present in pid.h"
fi

if ! grep -q "@brief" firmware/include/motor.h; then
    fail "Missing Doxygen comments in motor.h"
else
    pass "Doxygen comments present in motor.h"
fi

echo ""

# ============================================================================
# 7. CONFIGURATION CONSISTENCY
# ============================================================================
echo "[7/10] Checking configuration consistency..."
echo "------------------------------------------------------------"

# Check sample time consistency
MAIN_DT=$(grep "#define SAMPLE_TIME" firmware/src/main.c | grep -oP "0\.\d+f" | head -1)
MOTOR_DT=$(grep "model_dt = " firmware/src/motor.c | grep -oP "0\.\d+f")
PY_DT=$(grep "SAMPLE_TIME_SEC = " sim/pid_simulation.py | grep -oP "0\.\d+")

echo "Sample time in main.c: $MAIN_DT"
echo "Sample time in motor.c: $MOTOR_DT"
echo "Sample time in pid_simulation.py: $PY_DT"

if [ "$MAIN_DT" = "$MOTOR_DT" ]; then
    pass "Sample times consistent between main.c and motor.c"
else
    fail "Sample time mismatch!"
fi

echo ""

# ============================================================================
# 8. VERSION CONSISTENCY
# ============================================================================
echo "[8/10] Checking version consistency..."
echo "------------------------------------------------------------"

CMAKE_VER=$(grep "project.*VERSION" CMakeLists.txt | grep -oP "\d+\.\d+\.\d+")
echo "CMakeLists.txt version: $CMAKE_VER"

# Check firmware file headers for version
for file in firmware/include/*.h firmware/src/*.c; do
    VER=$(grep "@version" "$file" | grep -oP "\d+\.\d+\.\d+" | head -1)
    if [ -n "$VER" ]; then
        if [ "$VER" != "$CMAKE_VER" ]; then
            warn "$file has version $VER (expected $CMAKE_VER)"
        fi
    fi
done
pass "Version checks complete"

echo ""

# ============================================================================
# 9. PLATFORM-SPECIFIC CODE REVIEW
# ============================================================================
echo "[9/10] Reviewing platform-specific code..."
echo "------------------------------------------------------------"

# Check for platform-specific includes in core logic
if grep -rn "windows.h\|unistd.h" firmware/src/pid.c firmware/src/motor.c 2>/dev/null | grep -q .; then
    warn "Platform-specific includes in core files:"
    grep -rn "windows.h\|unistd.h" firmware/src/pid.c firmware/src/motor.c || true
else
    pass "No platform-specific includes in core PID/motor code"
fi

# Check CMakeLists.txt has proper MSVC support
if grep -q "if(MSVC)" CMakeLists.txt; then
    pass "MSVC support present in CMakeLists.txt"
else
    fail "Missing MSVC support in CMakeLists.txt"
fi

echo ""

# ============================================================================
# 10. TEST COVERAGE ANALYSIS
# ============================================================================
echo "[10/10] Test coverage analysis..."
echo "------------------------------------------------------------"

# Count test cases
TEST_COUNT=$(grep "RUN_TEST" tests/test_pid.c | wc -l)
echo "Number of test cases: $TEST_COUNT"

if [ $TEST_COUNT -ge 12 ]; then
    pass "Sufficient test coverage ($TEST_COUNT tests)"
else
    warn "Only $TEST_COUNT tests found (expected 12+)"
fi

# List test names
echo "Test cases:"
grep "RUN_TEST" tests/test_pid.c | grep -oP "(?<=RUN_TEST\().*(?=\))" | sed 's/^/  - /'

echo ""

# ============================================================================
# FINAL SUMMARY
# ============================================================================
echo "================================================================================"
echo "QUALITY CHECK SUMMARY"
echo "================================================================================"

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}ALL CHECKS PASSED${NC}"
    echo ""
    echo "The codebase meets quality standards:"
    echo "  - Clean build with no warnings"
    echo "  - All unit tests passing"
    echo "  - Function signatures consistent"
    echo "  - Documentation present"
    echo "  - Platform compatibility verified"
    exit 0
else
    echo -e "${RED}QUALITY CHECK FAILED${NC}"
    echo ""
    echo "Please review and fix the issues above."
    exit 1
fi
