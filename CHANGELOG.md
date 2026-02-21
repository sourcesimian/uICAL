# Changelog

---

## [0.0.4] - 2026-02-21

### Added
- `InvalidDateMode` enum to control handling of invalid recurrence dates:
  - `SKIP` (strict RFC 5545 default)
  - `BACKWARD` (calendar-style fallback behavior)
- Runtime configuration via `getInvalidDateMode()` / `setInvalidDateMode()`.
- Support for DATE-only (`YYYYMMDD`) values for all-day events.
- Additional test coverage for recurrence edge cases.

### Changed
- MONTHLY and YEARLY recurrences now correctly default to `DTSTART`’s day and month when BY* parts are absent.
- Improved internal counter synchronization and cascade seeking logic.
- Refactored Arduino `readuntil()` to support arbitrarily long lines while preserving fixed chunk size.
- Improved C++ STL example and documentation.

### Fixed
- Correct handling of invalid recurrence dates per RFC 5545:
  - `BYMONTHDAY` skips invalid days (e.g. 31 in February).
  - `BYYEARDAY=366` only valid in leap years.
- Fixed `BYDAY` seeking when `DTSTART` does not match any specified weekday.
- Prevented crashes when optional VEVENT properties (`DTEND`, `SUMMARY`) are missing.
- Corrected internal date comparison logic and year/day validation bugs.

---

## [0.0.3] - 2025-07-31

### Changed
- Updated Docker development environment to better support pull request contributions.

---

## [0.0.2] - 2025-01-27

### Changed
- Version updates in `library.json`, `library.properties`, and `setup.py` to force Arduino Library Manager refresh.
- Merged improvements and fixes from contributors.

### Fixed
- Added missing `<stdexcept>` include.
- Improved line folding support per RFC 5545.
- String handling fixes for ESP32 compatibility.
- Removed rogue characters and whitespace handling issues.
- General parser robustness improvements.

### Added
- Python test for line folding behavior.
- Support for RFC 5545 line folding (content lines beginning with whitespace are unfolded correctly).

---

## [0.0.1] - 2020-09-20

### Added
- Initial public release.
- Basic iCalendar parsing support.
- Arduino compatibility.
- Inclusion in Arduino Library Manager.
