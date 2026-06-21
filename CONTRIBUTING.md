# Contributing To expected_fs

Thank you for helping improve `expected_fs`.

## Development Setup

Build and test from the repository root:

```bash
cmake -S . -B build -DCPM_SOURCE_CACHE=$PWD/.cpm-cache
cmake --build build
ctest --test-dir build -VV
```

The project requires C++23 or newer with `<expected>` support. C++26-only
standard library facilities must be guarded by their feature-test macros.

## Guidelines

* Keep changes focused and small enough to review.
* Add or update tests for behavior changes.
* Prefer thin wrappers around the non-throwing `std::filesystem` overloads.
* Preserve the public error channel as `std::error_code` unless an API change is
  discussed first.
* Avoid adding runtime dependencies; this library is intended to remain
  header-only.

## Pull Requests

Use [PULL_REQUEST_TEMPLATE.md](PULL_REQUEST_TEMPLATE.md) and include:

* The problem being solved.
* The relevant API behavior.
* The commands used to build and test.
* Any compatibility concerns for C++23/C++26 compilers or standard libraries.

To skip CI for documentation-only commits, include `[skip ci]` or `[ci skip]` in
the commit message.
