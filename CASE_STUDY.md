# LogHarbor
## Legacy Log Investigation & UTC Reporting

### Context and business problem
Older applications and infrastructure often emit incompatible text logs. Mixed time zones, inconsistent severity labels and malformed lines slow incident investigation. The practical need is a tool that can inspect actual inputs, preserve the source, and produce evidence that another person can review.

### Delivered solution
A working streaming C++17 log engine with a local investigation workspace that reads real files, normalizes supported timestamps to UTC, filters events and exports clear reports.

- Streaming parsing of ISO application, Apache access and classic Syslog text
- UTC normalization, explicit Syslog year and severity mapping
- Case-insensitive message filtering with bounded matching-event output
- Malformed-line diagnostics plus downloadable JSON, CSV and HTML reports

### A complete working flow
Real UTF-8 log file or upload → streaming C++17 parser → UTC / severity normalization → message filter → investigation / export.

The interface calls a real compiled executable for each analysis. Results are not hardcoded. Users can replace the bundled fixture with their own directory or log path. Exports are generated from the latest successful analysis.

### Engineering decisions
- C++17 standard-library implementation without external runtime libraries.
- Read-only access to original inputs; generated reports stay in the project's runtime directory.
- Explicit data limits, descriptive input errors, and visible partial-result conditions.
- Python binds only to the loopback interface; Host and Origin checks protect browser requests.
- UI text is escaped; exported HTML is escaped; CSV fields that resemble spreadsheet formulas are prefixed safely.

### What was verified
Compiled with Apple clang++ in C++17 mode with no warnings. Twelve engine tests and six live HTTP checks passed, including a 25,000-line input with a 500-event output cap. The mixed fixture yielded 13 parsed events and 2 deliberate malformed lines. See TEST_RESULTS.md and executable tests for exact coverage.

### Outcome
The delivered outcome is a locally runnable utility with an input-to-report workflow, source code, build configuration, meaningful automated checks and upload-ready portfolio copy. No revenue, time-saving percentage, client endorsement or production adoption is invented.

### Accurate positioning
Independent new modernization-support tool; no historical client migration is claimed. Windows execution and MSVC compilation are not verified. UTF-8 input and three documented formats only. Zone-free timestamps assume UTC; Syslog year is supplied. No multiline stack-trace reconstruction or live tailing. Reports retain the first 500 matches.

This project demonstrates modernization-support engineering. It is not represented as a completed port of a pre-existing customer application. All bundled example data is explicitly synthetic; the software itself processes real user inputs.

### Deliverables
- C++ source and CMake configuration
- Local Python dashboard with a start script
- Input fixtures and executable tests
- CSV, JSON and HTML export routes
- Malt description, case study, verification record and screenshot captions
- Actual browser screenshots and portfolio PDF added during final packaging
