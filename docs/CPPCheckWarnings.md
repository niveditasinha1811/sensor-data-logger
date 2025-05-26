# Cppcheck Warnings Explanation

> **Note:** The `docs/` directory normally holds generated analysis logs, but this file (`CPPCheckWarnings.md`) is hand-maintained and should be committed to the repository.

Cppcheck reports these unused functions in `sensor_logger.c`:

* `get_sensor_logger_entry_count()`
* `get_sensor_logger_head_index()`
* `get_sensor_logger_buffer_entry()`

These are style-level warnings (unused functions), not errors, and they won’t break the build. I added these getters to:

1. Prepare for future diagnostics or monitoring features.
2. Keep the internal buffer encapsulated behind a clean API.

**Options to handle:**

* **Inline suppression:**

  ```c
  // cppcheck-suppress unusedFunction
  ```
* **Suppression file:** Add to `cppcheck_suppressions.txt`:

  ```text
  src/sensor_logger.c:137: unusedFunction
  src/sensor_logger.c:143: unusedFunction
  src/sensor_logger.c:149: unusedFunction
  ```
* **Remove functions:** If unused indefinitely, delete their declarations and definitions.

**Next step:**
I’ll maintain the suppression file for now and revisit when these APIs get used in a future feature.
