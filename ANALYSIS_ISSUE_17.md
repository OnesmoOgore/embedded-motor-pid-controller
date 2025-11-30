# Comprehensive Analysis: Issue #17 - Code Coverage Reporting with gcov/lcov

**Date:** 2025-11-30
**Issue:** https://github.com/OnesmoOgore/embedded-motor-pid-controller/issues/17
**Status:** ✅ RECOMMENDED FOR IMPLEMENTATION

---

## Executive Summary

**Verdict: This issue is well-conceived, adds significant value, and should be implemented.**

The request to add code coverage reporting with gcov/lcov is:
- ✅ **Technically sound** - Aligns with project's existing infrastructure
- ✅ **Valuable** - Provides measurable quality metrics for an embedded systems project
- ✅ **Safe** - Can be implemented without breaking existing functionality
- ✅ **Already planned** - Referenced in existing documentation as a future enhancement
- ✅ **Well-scoped** - Clear requirements and success criteria

---

## 1. Issue Analysis

### 1.1 What is Being Requested

The issue requests a complete code coverage reporting system with four key components:

1. **CMake Integration**: Add `-DENABLE_COVERAGE=ON` flag to enable coverage instrumentation
2. **Report Generation**: Use gcov/lcov to generate HTML coverage reports
3. **CI/CD Integration**: Automated coverage runs in GitHub Actions with artifact uploads
4. **Documentation**: Coverage badges and README updates

**Target Metrics:**
- Line coverage ≥ 85%
- Branch coverage ≥ 80%

### 1.2 Current Project State

**Strong Foundation:**
- ✅ 12 comprehensive unit tests already in place (`tests/test_pid.c`)
- ✅ CMake-based build system with modular architecture
- ✅ 4 active GitHub Actions workflows (CI, quality gate, CodeQL, docs deployment)
- ✅ Quality gate script (`quality_check.sh`) with 10 validation checks
- ✅ Cross-platform support (Linux, macOS, Windows)
- ✅ Professional documentation (README, build guide, architecture, Doxygen)

**Existing Coverage References:**
- Documentation already claims "~90%+ of critical paths" coverage (docs/architecture.md:370)
- README shows "coverage-coming-soon" badge (README.md:4)
- Architecture doc lists "Code coverage reporting (gcov/lcov integration)" as planned (docs/architecture.md:633)
- README lists "Code coverage reporting" as future enhancement (README.md:172)

**Key Observation:** The team already recognized the need for coverage reporting. This issue formalizes the implementation.

---

## 2. Value Proposition

### 2.1 Why This Adds Value

**For Embedded Systems Development:**
1. **Safety-Critical Nature**: Embedded motor control is often used in robotics, automation, and automotive applications where failures have real-world consequences
2. **Objective Quality Metrics**: Provides quantifiable evidence of test effectiveness
3. **Gap Identification**: Reveals untested code paths that may contain latent bugs
4. **Regression Prevention**: CI integration prevents coverage from decreasing over time
5. **Professional Appearance**: Coverage badges signal project maturity to users and contributors

**For This Specific Project:**
1. **Validation of Existing Claims**: The docs claim "~90%+ coverage" - this would prove it
2. **Continuous Improvement**: Identifies which of the 12 tests are most valuable
3. **Anti-Windup Verification**: Can prove edge cases like integrator clamping are tested
4. **Derivative-on-Measurement**: Can verify this critical feature (prevents derivative kick) is fully exercised

### 2.2 Alignment with Project Goals

**From CHANGELOG.md and Documentation:**
- Project emphasizes "production-ready" quality (version 1.0.0 released)
- Strict compiler flags (`-Wall -Wextra -Werror`) show commitment to quality
- Comprehensive quality gate (10 checks) demonstrates thoroughness
- Doxygen documentation shows professional standards

**Coverage reporting is a natural next step in this quality-focused trajectory.**

---

## 3. Risk Assessment

### 3.1 Potential Issues

| Risk | Severity | Mitigation |
|------|----------|------------|
| **Build Performance** | Low | Coverage only enabled with `-DENABLE_COVERAGE=ON` (off by default) |
| **Windows Incompatibility** | Medium | Already acknowledged in issue; Linux/macOS only, documented in README |
| **CI/CD Overhead** | Low | Coverage job runs in parallel with tests; ~1 minute estimated |
| **False Confidence** | Medium | Document that 100% coverage ≠ bug-free code; focus on critical paths |
| **Unity Framework Pollution** | Medium | Exclude `tests/Unity/` from coverage (third-party code) |
| **Demo Code Inflation** | Low | Exclude `main.c` from coverage (not library code) |
| **Complexity Creep** | Low | Implementation is straightforward; gcov/lcov are mature tools |

### 3.2 What Could Go Wrong

**Scenario 1: Coverage Metrics Too Low**
- *Risk*: Initial coverage might be lower than the claimed "~90%+"
- *Impact*: Embarrassment, but reveals truth about test quality
- *Mitigation*: Accept actual metrics as baseline; improve from there

**Scenario 2: CI Failures on Coverage Runs**
- *Risk*: Coverage jobs fail due to environment issues
- *Impact*: Delays merging pull requests
- *Mitigation*: Make coverage job non-blocking initially (allow_failure: true)

**Scenario 3: Conflicts with Existing Build Flags**
- *Risk*: Coverage flags (`--coverage`, `-fprofile-arcs`) conflict with optimization flags
- *Impact*: Build failures or incorrect coverage data
- *Mitigation*: Use separate build type or override flags in coverage mode

---

## 4. Functional Requirements Analysis

### 4.1 Stated Requirements (from Issue)

| Requirement | Feasibility | Notes |
|-------------|-------------|-------|
| CMake flag `-DENABLE_COVERAGE=ON` | ✅ Straightforward | Use `option()` command in CMakeLists.txt |
| Compiler flags `--coverage` | ✅ Supported | GCC/Clang on Linux/macOS |
| Custom CMake target for lcov | ✅ Standard practice | `add_custom_target(coverage ...)` |
| HTML reports in `coverage_html/` | ✅ Standard lcov output | Generated by `genhtml` |
| CI workflow with artifact upload | ✅ Easy integration | Similar to existing `simulate` job artifacts |
| Coverage badges in README | ✅ Multiple options | shields.io, coveralls.io, or codecov.io |
| Line coverage ≥ 85% | ⚠️ Needs verification | Depends on actual coverage; may need adjustment |
| Branch coverage ≥ 80% | ⚠️ Needs verification | Same as above |
| Build overhead < 1 minute | ✅ Reasonable | Coverage adds ~10-30 seconds |

### 4.2 Implied/Missing Requirements

**Should Be Added:**

1. **Exclusion Patterns**:
   - Exclude `tests/Unity/*` (third-party framework)
   - Exclude `firmware/src/main.c` (demo code, not library)
   - Focus coverage on library code: `pid.c`, `motor.c`

2. **Local Development Workflow**:
   - Document how developers can run coverage locally
   - Add instructions to `docs/build.md`
   - Provide convenient `make coverage` or `cmake --target coverage` command

3. **Integration with quality_check.sh**:
   - Step 10 currently does basic test count analysis
   - Could be enhanced to parse coverage report and fail if below threshold

4. **Baseline Documentation**:
   - Record initial coverage metrics for future comparison
   - Add to CHANGELOG.md when implemented

5. **Coverage Trends**:
   - Store historical coverage data (optional)
   - Compare PR coverage against main branch (optional)

### 4.3 Recommended Scope Adjustments

**Minimum Viable Product (MVP):**
- ✅ CMake flag and compiler configuration
- ✅ Custom target to generate coverage report
- ✅ HTML report generation
- ✅ Local development instructions
- ✅ Basic CI integration

**Nice-to-Have (Phase 2):**
- ⏳ Coverage badges (requires third-party service integration)
- ⏳ Threshold enforcement (fail build if coverage drops)
- ⏳ Differential coverage (compare PRs to main)
- ⏳ Integration with quality_check.sh

**Recommendation: Implement MVP first, assess results, then add Phase 2 features.**

---

## 5. Technical Implementation Feasibility

### 5.1 CMake Integration

**Current Build System:**
- Clean modular design with two STATIC libraries (`pid_controller`, `motor_model`)
- Existing options: `BUILD_TESTS`, `BUILD_DEMO`
- Platform-specific compiler flags already handled (MSVC vs GCC/Clang)

**Required Changes:**
```cmake
# Add option
option(ENABLE_COVERAGE "Enable code coverage reporting" OFF)

# Add compiler flags when enabled (GCC/Clang only)
if(ENABLE_COVERAGE AND NOT MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

# Custom target
add_custom_target(coverage
    COMMAND lcov --capture --directory . --output-file coverage.info
    COMMAND lcov --remove coverage.info '/usr/*' '*/tests/Unity/*' --output-file coverage.info
    COMMAND genhtml coverage.info --output-directory coverage_html
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS test_pid
    COMMENT "Generating code coverage report..."
)
```

**Complexity: LOW** - Approximately 20 lines of CMake code

### 5.2 CI/CD Integration

**Current CI Structure:**
- `ci.yml`: Build and test on Ubuntu + Windows
- `quality-gate.yml`: Runs quality_check.sh on Ubuntu

**Proposed Addition:**
```yaml
coverage:
  name: Code Coverage
  runs-on: ubuntu-latest
  steps:
    - checkout
    - install CMake
    - clone Unity framework
    - install lcov (apt-get)
    - configure with -DENABLE_COVERAGE=ON
    - build
    - run tests
    - generate coverage report (cmake --target coverage)
    - upload HTML artifacts
    - (optional) upload to codecov.io
```

**Complexity: LOW** - Similar to existing `test` job, ~30 lines of YAML

### 5.3 Compatibility Assessment

| Platform | gcov Support | lcov Support | Recommendation |
|----------|--------------|--------------|----------------|
| Ubuntu (GCC) | ✅ Native | ✅ apt-get install | Primary target |
| Ubuntu (Clang) | ✅ llvm-cov | ✅ Compatible | Secondary target |
| macOS (Clang) | ✅ llvm-cov | ✅ brew install | Supported |
| Windows (MSVC) | ❌ No | ❌ No | Document as unsupported |
| Windows (MinGW) | ⚠️ Partial | ⚠️ Partial | Not recommended |

**Conclusion:** Focus on Linux/macOS. Add clear documentation that coverage requires GCC or Clang.

---

## 6. Testing & Validation Strategy

### 6.1 Implementation Testing Loop

**Phase 1: Local Development**
1. Enable coverage on local machine (Ubuntu recommended)
2. Build with `-DENABLE_COVERAGE=ON`
3. Run tests and generate initial coverage report
4. Review baseline metrics (line %, branch %, function %)
5. Identify any untested functions or branches
6. Verify exclusions work (Unity framework not included)

**Phase 2: CI Integration**
1. Add coverage job to GitHub Actions
2. Verify job completes successfully
3. Check artifact upload works
4. Review HTML report from CI artifacts
5. Ensure coverage job doesn't block other workflows

**Phase 3: Documentation & Validation**
1. Update README with coverage badge (or "coverage: X%" text)
2. Update docs/build.md with local coverage instructions
3. Update CHANGELOG.md with coverage feature addition
4. Run full quality gate to ensure no regressions
5. Test on clean environment (fresh clone)

### 6.2 Success Criteria Validation

From the issue, success requires:

| Criterion | Validation Method |
|-----------|-------------------|
| ✅ CMake compiles with coverage flags | Run `cmake -B build -DENABLE_COVERAGE=ON && cmake --build build` |
| ✅ HTML report generation | Check `build/coverage_html/index.html` exists and opens in browser |
| ✅ CI integration | GitHub Actions workflow completes without errors |
| ✅ Artifact upload | Download artifacts from GitHub Actions and inspect HTML |
| ✅ README documentation | Visual inspection of README.md |
| ✅ Coverage badge | Badge displays in README (or text shows coverage %) |
| ⚠️ Line coverage ≥ 85% | May need adjustment based on actual results |
| ⚠️ Branch coverage ≥ 80% | May need adjustment based on actual results |
| ✅ Build overhead < 1 minute | Measure CI job duration |

**Adjustment for Realism:**
- Initial coverage may not meet 85%/80% thresholds
- Recommendation: Record actual baseline, then work toward targets iteratively
- Alternative: Set thresholds to "non-blocking" initially

### 6.3 Regression Testing

**Ensure No Breakage:**
1. Run all existing tests with coverage disabled (default behavior)
2. Verify demo still compiles and runs
3. Run quality_check.sh in full
4. Check all 4 existing GitHub Actions workflows still pass
5. Test on both Ubuntu and Windows (coverage only on Ubuntu)
6. Verify Python simulation still works

**Rollback Plan:**
- Coverage is opt-in (`-DENABLE_COVERAGE=ON`), so default builds unaffected
- If CI coverage job fails, mark as `allow_failure: true` until fixed
- Can always disable coverage target if it causes issues

---

## 7. Recommended Enhancements to Issue

The issue is well-written, but could be improved with these additions:

### 7.1 Clarifications Needed

1. **Exclusion Patterns**:
   - "Exclude `tests/Unity/` from coverage reports (third-party code)"
   - "Exclude `firmware/src/main.c` from coverage (demo code, not library)"
   - "Focus coverage metrics on library code: `pid.c`, `motor.c`"

2. **Threshold Flexibility**:
   - "Initial coverage baseline will be measured and documented"
   - "85%/80% targets are aspirational; actual thresholds will be set based on baseline"
   - "Consider non-blocking coverage checks initially to avoid CI disruptions"

3. **Local Development**:
   - "Add instructions to `docs/build.md` for running coverage locally"
   - "Provide convenient `cmake --build build --target coverage` command"

4. **Badge Implementation**:
   - Specify which service to use (codecov.io, coveralls.io, or manual shields.io badge)
   - Or: "Start with text-based coverage % in README, badges optional in Phase 2"

### 7.2 Additional Considerations

**Performance:**
- Coverage-instrumented builds should NOT be deployed to production
- Add warning in CMake output when coverage is enabled
- Consider separate build type: `-DCMAKE_BUILD_TYPE=Coverage`

**Reporting:**
- Generate both summary (console output) and detailed (HTML) reports
- Include function coverage in addition to line/branch coverage
- Consider JSON output for programmatic parsing

**Maintenance:**
- Add `coverage_html/` and `*.gcda`, `*.gcno` to `.gitignore`
- Document that coverage data is local-only, not committed

---

## 8. Master Implementation Plan

### 8.1 Phased Approach

**Phase 1: Core Implementation (2-3 hours)**
1. Update `.gitignore` to exclude coverage artifacts
2. Modify `CMakeLists.txt`:
   - Add `ENABLE_COVERAGE` option
   - Add compiler/linker flags when enabled
   - Create `coverage` custom target with lcov commands
   - Add exclusion patterns for Unity and main.c
3. Test locally:
   - Build with coverage enabled
   - Run tests
   - Generate HTML report
   - Verify exclusions work
   - Record baseline metrics

**Phase 2: CI/CD Integration (1 hour)**
4. Create `.github/workflows/coverage.yml`:
   - Configure Ubuntu runner
   - Install lcov
   - Clone Unity framework
   - Build with coverage enabled
   - Run tests
   - Generate coverage report
   - Upload HTML artifacts
5. Test CI workflow:
   - Push to branch
   - Verify workflow runs successfully
   - Download and inspect artifacts

**Phase 3: Documentation (30 minutes)**
6. Update `README.md`:
   - Add coverage section
   - Update "coverage-coming-soon" badge to actual coverage % (or badge URL)
   - Document Windows limitation
7. Update `docs/build.md`:
   - Add "Building with Coverage" section
   - Provide step-by-step local coverage instructions
8. Update `CHANGELOG.md`:
   - Add entry for coverage feature
   - Record baseline coverage metrics
9. Update `docs/architecture.md`:
   - Update "Testing & Quality" section
   - Move coverage from "planned" to "implemented"

**Phase 4: Testing & Validation (30 minutes)**
10. Run full quality gate: `./quality_check.sh`
11. Test clean build: `cmake -B build && cmake --build build`
12. Test coverage build: `cmake -B build_cov -DENABLE_COVERAGE=ON && cmake --build build_cov --target coverage`
13. Verify all CI workflows pass
14. Review coverage report for any surprises

**Total Estimated Time: 4-5 hours** (conservative estimate; issue says 2-4 hours)

### 8.2 Step-by-Step Implementation Checklist

**Pre-Implementation:**
- [ ] Verify local environment has lcov installed (`sudo apt-get install lcov`)
- [ ] Ensure Unity framework is cloned (`git clone https://github.com/ThrowTheSwitch/Unity.git tests/Unity`)
- [ ] Create feature branch: `git checkout -b add-code-coverage`

**Step 1: Update .gitignore**
- [ ] Add `*.gcda`, `*.gcno`, `*.gcov`
- [ ] Add `coverage_html/`
- [ ] Add `coverage.info`

**Step 2: Modify CMakeLists.txt**
- [ ] Add `option(ENABLE_COVERAGE "Enable code coverage" OFF)`
- [ ] Add compiler flags section (after existing compiler flags)
- [ ] Add linker flags for coverage
- [ ] Create `coverage` custom target
- [ ] Add lcov commands with proper exclusions
- [ ] Test: `cmake -B build_cov -DENABLE_COVERAGE=ON`

**Step 3: Local Testing**
- [ ] Build: `cmake --build build_cov`
- [ ] Run tests: `cd build_cov && ctest`
- [ ] Generate coverage: `cmake --build build_cov --target coverage`
- [ ] Open HTML: `firefox build_cov/coverage_html/index.html`
- [ ] Record baseline metrics (line %, branch %, function %)
- [ ] Verify Unity excluded
- [ ] Verify main.c excluded (or decide if it should be)

**Step 4: Create CI Workflow**
- [ ] Create `.github/workflows/coverage.yml`
- [ ] Add job definition with ubuntu-latest
- [ ] Install dependencies (cmake, lcov)
- [ ] Clone Unity framework
- [ ] Configure with coverage enabled
- [ ] Build and run tests
- [ ] Generate coverage report
- [ ] Upload artifacts

**Step 5: Update Documentation**
- [ ] Update `README.md`:
  - [ ] Coverage badge/text
  - [ ] Windows limitation note
- [ ] Update `docs/build.md`:
  - [ ] Add coverage section
  - [ ] Local development instructions
- [ ] Update `CHANGELOG.md`:
  - [ ] Add coverage feature entry
  - [ ] Record baseline metrics
- [ ] Update `docs/architecture.md`:
  - [ ] Move coverage from planned to implemented
  - [ ] Add details about coverage setup

**Step 6: Final Validation**
- [ ] Run `./quality_check.sh` → should pass
- [ ] Test default build: `cmake -B build && cmake --build build && ctest --test-dir build`
- [ ] Test coverage build: `cmake -B build_cov -DENABLE_COVERAGE=ON && cmake --build build_cov --target coverage`
- [ ] Push to feature branch and verify CI passes
- [ ] Review coverage HTML report from CI artifacts
- [ ] Check all 4 existing workflows still pass

**Step 7: Commit & Push**
- [ ] Commit with message: `feat: add code coverage reporting with gcov/lcov (#17)`
- [ ] Push to branch: `git push -u origin claude/analyze-pid-issue-01M8m4BuLobwJv6mFa4yp7Mz`
- [ ] Verify all CI checks pass
- [ ] Create pull request (if requested)

### 8.3 Feedback Loops

**During Implementation:**
1. After each CMake change → Test build immediately
2. After adding coverage target → Run and inspect HTML report
3. After CI workflow creation → Push and monitor workflow logs
4. After documentation → Read through as an end-user would

**Post-Implementation:**
1. If coverage < 85%, investigate which code is untested
2. If coverage unexpectedly low, verify exclusions are working
3. If CI fails, check workflow logs and test locally
4. If tests fail with coverage enabled, check for flag conflicts

**Long-Term:**
1. Monitor coverage trends over time (should increase)
2. Review coverage reports when adding new features
3. Consider enforcement (fail CI if coverage drops by >5%)
4. Periodically review untested code to determine if tests are needed

---

## 9. Potential Gotchas & Solutions

### 9.1 Common Coverage Issues

| Issue | Symptom | Solution |
|-------|---------|----------|
| **Inlined functions** | Low coverage on simple functions | Use `-fno-inline` flag for coverage builds |
| **Optimization interference** | Incorrect coverage data | Force `-O0` when coverage enabled |
| **Missing .gcda files** | "No coverage data" error | Ensure tests actually execute |
| **Path mismatches** | lcov can't find sources | Use `--base-directory` flag in lcov |
| **Unity framework included** | Coverage inflated by test code | Add `--remove` pattern for `*/tests/Unity/*` |
| **Windows failure** | gcov not found on Windows | Document as Linux/macOS only |

### 9.2 Recommended CMake Configuration

```cmake
if(ENABLE_COVERAGE)
    if(MSVC)
        message(WARNING "Code coverage not supported with MSVC")
    else()
        # Force debug mode and disable optimization for accurate coverage
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage -O0 -g")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")

        # Find lcov and genhtml
        find_program(LCOV_PATH lcov)
        find_program(GENHTML_PATH genhtml)

        if(NOT LCOV_PATH)
            message(FATAL_ERROR "lcov not found! Install with: sudo apt-get install lcov")
        endif()
    endif()
endif()
```

---

## 10. Final Recommendation

### 10.1 Go/No-Go Decision

**RECOMMENDATION: ✅ GO - Implement with modifications**

**Rationale:**
1. ✅ Strong project foundation (12 tests, quality gate, professional CI/CD)
2. ✅ Already planned feature (documented as future enhancement)
3. ✅ Low implementation risk (opt-in, doesn't affect default builds)
4. ✅ High value for embedded systems project
5. ✅ Clear success criteria and testing strategy
6. ✅ Feasible time estimate (4-5 hours including documentation)
7. ✅ No blockers or showstoppers identified

**Modifications from Original Issue:**
1. **Adjust threshold expectations**: Record baseline first, then set realistic targets
2. **Add exclusion patterns**: Unity framework and demo code should be excluded
3. **Phase badge implementation**: Start with text-based coverage %, add badges in Phase 2
4. **Make CI non-blocking initially**: Use `allow_failure: true` until coverage is stable
5. **Focus on library code**: `pid.c` and `motor.c` are the critical coverage targets

### 10.2 Success Metrics

**Immediate Success (Phase 1):**
- [ ] Coverage report generated successfully
- [ ] Baseline metrics documented (actual % for line/branch/function)
- [ ] HTML report readable and accurate
- [ ] Exclusions working correctly

**Short-Term Success (Phase 2):**
- [ ] CI workflow running without errors
- [ ] Coverage artifacts uploaded and downloadable
- [ ] Documentation complete and clear
- [ ] No regressions in existing workflows

**Long-Term Success (Ongoing):**
- [ ] Coverage metrics improving over time
- [ ] New features include tests that increase coverage
- [ ] Coverage reports used in code review process
- [ ] Project maintains "high coverage" reputation

### 10.3 Open Questions for User

Before implementation, confirm:

1. **Badge Service**: Do you want to use codecov.io, coveralls.io, or manual badges?
   - *Recommendation*: Start with text-based coverage %, add service integration later

2. **Threshold Enforcement**: Should CI fail if coverage drops below threshold?
   - *Recommendation*: No enforcement initially; monitor first

3. **Coverage Scope**: Should `main.c` be included in coverage?
   - *Recommendation*: Exclude it (demo code, not library)

4. **Baseline Acceptance**: If initial coverage is < 85%, is that acceptable?
   - *Recommendation*: Yes, use as baseline and improve iteratively

5. **Documentation Location**: Where should detailed coverage instructions go?
   - *Recommendation*: `docs/build.md` for local development

---

## 11. Conclusion

**Issue #17 is well-thought-out, technically sound, and ready for implementation.**

The request aligns perfectly with the project's quality-focused culture and fills a gap in the current testing infrastructure. The implementation is straightforward with low risk and high value.

**Key Strengths:**
- Builds on existing solid foundation (12 tests, quality gate)
- Already planned as future enhancement
- Clear requirements and success criteria
- Realistic time estimate
- Opt-in design (doesn't affect default builds)

**Key Considerations:**
- Adjust expectations for initial coverage metrics
- Exclude third-party and demo code from coverage
- Focus on library code (`pid.c`, `motor.c`)
- Document Windows incompatibility
- Start simple, add advanced features (badges, thresholds) later

**Ready to implement with high confidence of success.**

---

## Appendix A: Expected Coverage Breakdown

Based on the 12 existing tests, here's the likely coverage:

| Source File | Expected Line Coverage | Expected Branch Coverage | Rationale |
|-------------|------------------------|--------------------------|-----------|
| **pid.c** | 90-95% | 85-90% | 12 tests cover init, P, I, D, combined, limits, reset, edge cases |
| **motor.c** | 70-80% | 60-70% | Used in tests but not directly tested; simulation focus |
| **main.c** | 0% (excluded) | 0% (excluded) | Demo code, runs indefinitely, not tested |

**Overall Estimate (library code only):**
- Line coverage: ~85-90%
- Branch coverage: ~80-85%
- Function coverage: ~100% (all public functions tested)

**If main.c included:**
- Line coverage: ~60-70% (main.c untested)
- Branch coverage: ~55-65%

**Recommendation: Exclude main.c to get accurate library coverage metrics.**

---

## Appendix B: Files That Will Be Modified

**New Files:**
- `.github/workflows/coverage.yml` (new)
- `build_cov/coverage_html/*` (generated, not committed)

**Modified Files:**
- `CMakeLists.txt` (+30 lines)
- `.gitignore` (+5 lines)
- `README.md` (~10 line changes)
- `docs/build.md` (+50 lines)
- `docs/architecture.md` (~5 line changes)
- `CHANGELOG.md` (+10 lines)

**Total Code Changes:** ~110 lines (mostly documentation and CMake configuration)

---

**Analysis Complete. Awaiting approval to proceed with implementation.**
