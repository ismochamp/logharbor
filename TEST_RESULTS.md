# Verification record — LogHarbor

Date: 2026-09-28. Environment: macOS 15.7.8, Python 3.14.7, Apple clang++ with `-std=c++17 -O2 -Wall -Wextra -Wpedantic`.

## Build

Compilation completed successfully with no compiler warnings.

## Automated engine checks

**12 tests passed** with `python3 -m unittest discover -s tests -v`.

- ISO, Apache and Syslog parsed in one input
- UTC normalization across previous/next date boundaries
- Invalid leap day and invalid timezone rejected; valid leap day accepted
- Severity plus case-insensitive message filtering
- 25,000 lines counted while only 500 matching records retained
- Oversized line skipped safely; subsequent line still parsed
- Empty file and CRLF/Unicode/quote handling
- Explicit Syslog year and fatal severity mapping
- Apache HTTP status mapped to severity
- Invalid filter and nonexistent file rejected
- Fixture helper verifies source hash is unchanged

## Live HTTP checks

**6 tests passed** against the running local dashboard on port 8114: health route, invalid Host rejected, invalid Origin rejected, non-object JSON rejected, missing input rejected, real-engine response and all three download formats returned successfully. Initial network attempts were blocked by the execution sandbox; the same read-only local checks passed after local-network permission was applied.

## Measured fixture result

Compiled with Apple clang++ in C++17 mode with no warnings. Twelve engine tests and six live HTTP checks passed, including a 25,000-line input with a 500-event output cap. The mixed fixture yielded 13 parsed events and 2 deliberate malformed lines.

## Deliberately unverified

Windows runtime, Visual Studio/MSVC compilation, multi-user deployment, real customer outcomes, unattended production operation, performance under concurrent load and every possible third-party log/filename convention. Browser screenshots and final PDF layout are verified during packaging separately.

## Publication preparation — 2026-09-30

Recompiled C++17 from source without compiler warnings. All 12 engine tests and all 6 live HTTP checks passed. The synthetic `.log` fixture remains included in source control; executable outputs and private uploads remain excluded. Windows/MSVC execution remains unverified.
