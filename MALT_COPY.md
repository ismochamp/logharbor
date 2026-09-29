# LogHarbor — Legacy Log Investigation & UTC Reporting

**Category:** Legacy Software Modernization  
**Project status:** Independent working project, September 2026. No client engagement or historical production deployment is claimed.

## Short description
A working streaming C++17 log engine with a local investigation workspace that reads real files, normalizes supported timestamps to UTC, filters events and exports clear reports.

## Portfolio description
Older applications and infrastructure often emit incompatible text logs. Mixed time zones, inconsistent severity labels and malformed lines slow incident investigation.

I built LogHarbor to turn this technical uncertainty into a reviewable workflow. A working streaming C++17 log engine with a local investigation workspace that reads real files, normalizes supported timestamps to UTC, filters events and exports clear reports.

- Streaming parsing of ISO application, Apache access and classic Syslog text
- UTC normalization, explicit Syslog year and severity mapping
- Case-insensitive message filtering with bounded matching-event output
- Malformed-line diagnostics plus downloadable JSON, CSV and HTML reports

The implementation combines a compiled C++17 engine with a local Python web interface. Inputs are processed on the user's machine, source files remain unchanged, and the output can be shared as a report.

**Verified evidence:** Compiled with Apple clang++ in C++17 mode with no warnings. Twelve engine tests and six live HTTP checks passed, including a 25,000-line input with a 500-event output cap. The mixed fixture yielded 13 parsed events and 2 deliberate malformed lines.

**Scope:** Independent new modernization-support tool; no historical client migration is claimed. Windows execution and MSVC compilation are not verified. UTF-8 input and three documented formats only. Zone-free timestamps assume UTC; Syslog year is supplied. No multiline stack-trace reconstruction or live tailing. Reports retain the first 500 matches.

**Skills:** C++17 · debugging · compatibility analysis · filesystem / text processing · Python integration · automated verification

## Screenshot captions
1. The running log workspace accepts local files or uploads with severity, text and Syslog-year controls.
2. Real parsing output from the labeled synthetic mixed-format fixture, including normalized timestamps and diagnostic totals.

## Client conversation starter
Are inconsistent application logs slowing down troubleshooting? I can help make the relevant events searchable and explain what failed across your existing systems.
