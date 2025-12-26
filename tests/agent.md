# LVGL Test Development Agent Instructions

You are an AI agent helping with LVGL test case development. You have access to 4 MCP tools for testing workflow.

## MCP Server Setup

The MCP server script is at `scripts/mcp_server.py`. To enable these tools:

### VS Code / Cursor
Add to `.vscode/mcp.json` (create if not exists):
```json
{
  "servers": {
    "lvgl-test": {
      "type": "stdio",
      "command": "python3",
      "args": ["${workspaceFolder}/scripts/mcp_server.py"],
      "env": {}
    }
  }
}
```

### Claude Desktop
Add to `~/.config/claude/claude_desktop_config.json` (Linux) or `~/Library/Application Support/Claude/claude_desktop_config.json` (macOS):
```json
{
  "mcpServers": {
    "lvgl-test": {
      "command": "python3",
      "args": ["/absolute/path/to/lvgl/scripts/mcp_server.py"]
    }
  }
}
```

### Verify Setup
Run self-test to verify the server works:
```bash
python3 scripts/mcp_server.py --self-test -v
```

List available tools:
```bash
python3 scripts/mcp_server.py --list-tools
```

Generate mcp.json config:
```bash
python3 scripts/mcp_server.py --gen-config
```

### Requirements
- Python 3.9+
- Install MCP SDK: `pip install mcp`

## Available Tools

### lvgl_run_tests
Run LVGL tests (auto build + test).
- test_filter (optional): filter tests by name, e.g. "arc", "btn", "menu"
- report (optional, default true): generate coverage report
- clean (optional, default false): clean build before running

Examples:
- Run all: {}
- Run arc tests: {"test_filter": "arc"}
- Clean build: {"clean": true}

### lvgl_check_coverage
Check code coverage for a file or directory.
- path (required): file or dir path relative to lvgl root, e.g. "src/widgets/lv_arc.c"
- fail_under (optional, default 0): fail if coverage below this percentage

Examples:
- Check file: {"path": "src/widgets/lv_btn.c"}
- Check dir: {"path": "src/misc"}
- Require 80%: {"path": "src/core/lv_obj.c", "fail_under": 80}

Output shows coverage percentage and lists uncovered line numbers.

### lvgl_format_code
Format code with astyle.
- target (optional): "demos", "examples", "src", or "tests". Default formats all.

Examples:
- Format all: {}
- Format tests only: {"target": "tests"}

### lvgl_gdb_debug
Debug tests with GDB.
- test_name (optional): test name like "arc", "btn". If empty, lists available tests.
- breakpoint (required): "file.c:123" or "function_name"
- commands (optional, default ["bt", "info locals", "continue"]): GDB commands after breakpoint hit
- max_hits (optional, default 3): max breakpoint hits

Examples:
- Debug arc test: {"test_name": "arc", "breakpoint": "test_arc_creation_successful"}
- Set line breakpoint: {"test_name": "btn", "breakpoint": "test_btn.c:50"}
- Custom commands: {"test_name": "label", "breakpoint": "lv_label_set_text", "commands": ["bt", "print text", "continue"]}

## Workflow

### Writing new tests (standard flow)
1. Write/modify test code in tests/src/test_cases/widgets/ or appropriate subdir
2. Format code: lvgl_format_code {"target": "tests"}
3. Run tests: lvgl_run_tests {"test_filter": "your_test"}
4. Check coverage: lvgl_check_coverage {"path": "src/your_module.c"}
5. Read uncovered lines from coverage output
6. Improve test to cover missed branches/conditions
7. Repeat steps 2-6 until coverage target is met

### Troubleshooting

If gcov files fail to generate or coverage data seems stale:
- Run with clean build: lvgl_run_tests {"test_filter": "your_test", "clean": true}

If code logic is unclear from static analysis:
- Use GDB to step through: lvgl_gdb_debug {"test_name": "...", "breakpoint": "function_or_line"}
- Add "step", "next", "print var" to commands for detailed inspection

### Improving coverage
1. Check current: lvgl_check_coverage {"path": "src/widgets/lv_btn.c"}
2. Read uncovered lines from output
3. Write tests targeting those branches/conditions
4. Rerun: lvgl_run_tests {"test_filter": "btn"}
5. Verify improvement: lvgl_check_coverage {"path": "...", "fail_under": 80}

### Debugging failures
1. Run failing test: lvgl_run_tests {"test_filter": "failing_test"}
2. Find FAIL assertion in output
3. Set breakpoint: lvgl_gdb_debug {"test_name": "...", "breakpoint": "..."}
4. Use "print var" in commands to inspect variables
5. Fix and rerun

## Test File Structure

Tests live in:
- lvgl/tests/src/test_cases/widgets/ - widget tests (test_arc.c, test_btn.c, etc.)
- lvgl/tests/src/test_cases/libs/ - library tests
- lvgl/tests/src/test_cases/draw/ - drawing tests

## Test Code Template

```c
#include "../lv_test.h"
#include "lvgl.h"

void test_widget_basic(void)
{
    lv_obj_t * obj = lv_widget_create(lv_screen_active());
    lv_widget_set_value(obj, 100);
    TEST_ASSERT_EQUAL(100, lv_widget_get_value(obj));
}

void test_widget_edge_case(void)
{
    lv_obj_t * obj = lv_widget_create(lv_screen_active());
    lv_widget_set_value(obj, INT32_MAX);
    TEST_ASSERT_EQUAL(INT32_MAX, lv_widget_get_value(obj));
}
```

## Assertions

Basic:
- TEST_ASSERT(condition)
- TEST_ASSERT_TRUE(condition)
- TEST_ASSERT_FALSE(condition)
- TEST_ASSERT_NULL(ptr)
- TEST_ASSERT_NOT_NULL(ptr)

Comparison (use type-specific versions for correct comparison):
- TEST_ASSERT_EQUAL(expected, actual) - for int
- TEST_ASSERT_EQUAL_UINT(expected, actual) - for unsigned int
- TEST_ASSERT_EQUAL_UINT8(expected, actual) - for uint8_t
- TEST_ASSERT_EQUAL_UINT16(expected, actual) - for uint16_t
- TEST_ASSERT_EQUAL_UINT32(expected, actual) - for uint32_t
- TEST_ASSERT_EQUAL_SIZE_T(expected, actual) - for size_t
- TEST_ASSERT_EQUAL_INT8(expected, actual) - for int8_t
- TEST_ASSERT_EQUAL_INT16(expected, actual) - for int16_t
- TEST_ASSERT_EQUAL_INT32(expected, actual) - for int32_t
- TEST_ASSERT_EQUAL_PTR(expected, actual) - for pointers
- TEST_ASSERT_EQUAL_STRING(expected, actual) - for strings
- TEST_ASSERT_EQUAL_MEMORY(expected, actual, len) - for memory blocks
- TEST_ASSERT_NOT_EQUAL as above but inverted

IMPORTANT: Always use the correct type-specific assertion. Using wrong type may cause incorrect comparison results or compiler warnings.

## Test Naming

Use: test_<module>_<feature>_<scenario>
Examples:
- test_arc_creation_successful
- test_arc_should_truncate_to_max_range
- test_btn_click_event_fired

## Coverage Targets

- New code: aim for 80%+
- Core modules: aim for 90%+
- Always test if/else branches, switch cases, edge conditions

## Before Commit

Always run:
1. lvgl_format_code {}
2. lvgl_run_tests {}
3. lvgl_check_coverage {"path": "src/changed_file.c", "fail_under": 70}

## Notes

- Test timeout is 600 seconds. Use test_filter to run specific tests if builds are slow.
- Coverage data comes from the most recent test run with report=true.
- GDB may show LeakSanitizer errors - this is normal and can be ignored.
- Call lvgl_gdb_debug with empty test_name to list available test executables.
