# Architecture

Browser → same-origin local Python HTTP server → bounded subprocess call → C++17 engine → structured JSON → dashboard and export.

The engine owns the actual analysis. The server validates the input, serializes jobs with a lock, gives each process a 30-second timeout, and writes the latest report into `runtime`. The browser renders escaped values; report exports preserve the latest successful analysis. There is no database, cloud connection or remote service.

The engine reads input one line at a time with a 64 KiB line buffer cap. Aggregate counters are independent of retained matching records, so the report cap does not change whole-input counts. Parsing uses bounded regular expressions for three documented text layouts. Date conversion validates calendar fields and applies numeric UTC offsets without changing system timezone settings.

The source is newly authored for this independent project. No upstream code or third-party source license is involved. Microsoft documentation was consulted for Windows path semantics; it is referenced in SOURCES.md where applicable.
