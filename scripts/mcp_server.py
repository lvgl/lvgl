#!/usr/bin/env python3
"""
LVGL Test MCP Server

An MCP (Model Context Protocol) server that provides tools for running LVGL tests,
checking code coverage, formatting code, and debugging with GDB.
"""

import asyncio
import glob
import json
import os
import subprocess
import sys
from typing import Any

# Check Python version
if sys.version_info < (3, 9):
    sys.exit("Error: Python 3.9 or higher is required to run this script.")

# Get LVGL root directory
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
LVGL_ROOT = os.path.dirname(SCRIPT_DIR)

# Command execution timeout in seconds (10 minutes)
COMMAND_TIMEOUT = 600


def run_command(cmd: list[str], cwd: str = None) -> tuple[int, str, str]:
    """
    Execute a command and return the result.

    Args:
        cmd: Command and arguments as a list of strings
        cwd: Working directory for the command

    Returns:
        Tuple of (return_code, stdout, stderr)
    """
    try:
        result = subprocess.run(
            cmd, cwd=cwd, capture_output=True, text=True, timeout=COMMAND_TIMEOUT
        )
        return result.returncode, result.stdout, result.stderr
    except subprocess.TimeoutExpired:
        error_message = (
            f"Command timed out after {COMMAND_TIMEOUT} seconds.\n"
            f"  Command: {cmd}\n"
            f"  Working directory: {cwd or os.getcwd()}\n"
            "This may indicate a long-running build, test, or other operation.\n"
            "Review the command's output/logs to check for progress or hangs, and "
            "consider increasing COMMAND_TIMEOUT or optimizing the command if needed."
        )
        return -1, "", error_message
    except Exception as e:
        return -1, "", str(e)


def truncate_output(text: str, max_length: int, head_portion_ratio: float = 0.5) -> str:
    """
    Truncate long output text while preserving head and tail.

    Args:
        text: The text to truncate
        max_length: Maximum allowed length
        head_portion_ratio: Proportion of the head section to preserve relative to max_length (0.0-1.0)

    Returns:
        Truncated text with marker if truncation occurred
    """
    marker = "\n\n... (output truncated) ...\n\n"

    # If the text already fits within the maximum length, return it unchanged.
    if len(text) <= max_length:
        return text

    # If max_length is too small to accommodate the marker plus any meaningful
    # content, fall back to a simple hard cutoff.
    if max_length <= len(marker):
        return text[:max_length]

    # Reserve space for the marker when calculating head and tail lengths.
    available_length = max_length - len(marker)
    head_len = int(available_length * head_ratio)
    tail_len = available_length - head_len

    # Guard against degenerate cases where tail_len might be zero or negative.
    if tail_len <= 0:
        return text[:available_length] + marker

    return text[:head_len] + marker + text[-tail_len:]


def create_mcp_server():
    """Create and configure the MCP server with all tools."""
    # Import MCP SDK (only when needed for server mode)
    from mcp.server import Server
    from mcp.types import Tool, TextContent

    server = Server("lvgl-test-server")

    @server.list_tools()
    async def list_tools() -> list[Tool]:
        """List all available tools."""
        return get_tool_definitions(Tool)

    @server.call_tool()
    async def call_tool(name: str, arguments: dict[str, Any]) -> list[TextContent]:
        """Route tool calls to appropriate handlers."""
        handlers = {
            "lvgl_run_tests": run_tests,
            "lvgl_check_coverage": check_coverage,
            "lvgl_format_code": format_code,
            "lvgl_gdb_debug": gdb_debug,
        }

        handler = handlers.get(name)
        if handler:
            return await handler(arguments, TextContent)
        return [TextContent(type="text", text=f"Unknown tool: {name}")]

    return server


def get_tool_definitions(Tool=None):
    """
    Get tool definitions. Works with or without MCP Tool class.

    Args:
        Tool: MCP Tool class (optional, for server mode)

    Returns:
        List of tool definitions (as Tool objects or dicts)
    """
    tools_data = [
        {
            "name": "lvgl_run_tests",
            "description": """Run LVGL test cases (automatic build + test).

Parameters:
- test_filter: (optional) Test filter to run only matching tests, e.g., "menu" runs only tests containing "menu"
- report: (optional) If true, generate test report, default true
- clean: (optional) If true, clean build directory before building

Examples:
- Run all tests: {}
- Run menu-related tests: {"test_filter": "menu"}
- Run with report: {"report": true}
""",
            "inputSchema": {
                "type": "object",
                "properties": {
                    "test_filter": {
                        "type": "string",
                        "description": "Test filter to run only matching tests",
                    },
                    "report": {
                        "type": "boolean",
                        "description": "Generate test report",
                        "default": True,
                    },
                    "clean": {
                        "type": "boolean",
                        "description": "Clean build directory",
                        "default": False,
                    },
                },
                "required": [],
            },
        },
        {
            "name": "lvgl_check_coverage",
            "description": """Check code coverage.

Parameters:
- path: File or directory path to check coverage (relative to lvgl directory or absolute path)
- fail_under: (optional) Fail if coverage is below this percentage, default 0

Examples:
- Check single file coverage: {"path": "path/to/file.c"}
- Check entire directory: {"path": "src/widgets"}
- Require 80% coverage: {"path": "path/to/file.c", "fail_under": 80}
""",
            "inputSchema": {
                "type": "object",
                "properties": {
                    "path": {
                        "type": "string",
                        "description": "File or directory path to check coverage",
                    },
                    "fail_under": {
                        "type": "number",
                        "description": "Fail if coverage is below this percentage",
                        "default": 0,
                    },
                },
                "required": ["path"],
            },
        },
        {
            "name": "lvgl_format_code",
            "description": """Run code formatting tool (astyle).

Parameters:
- target: (optional) Target directory to format, options: "demos", "examples", "src", "tests". Default formats all directories

Examples:
- Format all code: {}
- Format only tests: {"target": "tests"}
- Format only src: {"target": "src"}
""",
            "inputSchema": {
                "type": "object",
                "properties": {
                    "target": {
                        "type": "string",
                        "description": "Target directory to format",
                        "enum": ["demos", "examples", "src", "tests"],
                    }
                },
                "required": [],
            },
        },
        {
            "name": "lvgl_gdb_debug",
            "description": """Debug test cases using GDB.

Parameters:
- test_name: (optional) Test case name filter, e.g., "widgets" to debug test_widgets
- breakpoint: Breakpoint location, e.g., "file.c:123" or function name "my_function"
- commands: (optional) GDB command list to execute after breakpoint hit, default: bt, info locals, continue
- max_hits: (optional) Maximum breakpoint hit count, default 3

Examples:
- Set breakpoint at line: {"test_name": "test_name", "breakpoint": "file.c:123"}
- Set breakpoint at function: {"test_name": "test_name", "breakpoint": "function_name"}
- Custom commands: {"test_name": "test_name", "breakpoint": "file.c:123", "commands": ["bt", "print var", "continue"]}
""",
            "inputSchema": {
                "type": "object",
                "properties": {
                    "test_name": {
                        "type": "string",
                        "description": "Test case name filter",
                    },
                    "breakpoint": {
                        "type": "string",
                        "description": "Breakpoint location (file:line or function name)",
                    },
                    "commands": {
                        "type": "array",
                        "items": {"type": "string"},
                        "description": "GDB commands to execute after breakpoint hit",
                        "default": ["bt", "info locals", "continue"],
                    },
                    "max_hits": {
                        "type": "number",
                        "description": "Maximum breakpoint hit count",
                        "default": 3,
                    },
                },
                "required": ["breakpoint"],
            },
        },
    ]

    if Tool is None:
        return tools_data
    return [Tool(**tool) for tool in tools_data]


async def run_tests(arguments: dict[str, Any], TextContent) -> list:
    """
    Run LVGL tests (automatic build + test).

    Args:
        arguments: Tool arguments containing test_filter, report, clean options
        TextContent: MCP TextContent class for response formatting

    Returns:
        List of TextContent with test results
    """
    test_filter = arguments.get("test_filter", "")
    report = arguments.get("report", True)
    clean = arguments.get("clean", False)

    tests_dir = os.path.join(LVGL_ROOT, "tests")
    main_py = os.path.join(tests_dir, "main.py")

    # Build command: ./tests/main.py test --report
    cmd = [sys.executable, main_py, "test"]

    if report:
        cmd.append("--report")

    if clean:
        cmd.append("--clean")

    if test_filter:
        cmd.extend(["--test-suite", test_filter])

    # Execute command
    returncode, stdout, stderr = run_command(cmd, cwd=tests_dir)

    output_parts = []
    output_parts.append(f"Command: {' '.join(cmd)}")
    output_parts.append(f"Working directory: {tests_dir}")
    output_parts.append(f"Return code: {returncode}")

    if stdout:
        stdout = truncate_output(stdout, 10000)
        output_parts.append(f"\nStandard output:\n{stdout}")
    if stderr:
        stderr = truncate_output(stderr, 5000)
        output_parts.append(f"\nStandard error:\n{stderr}")

    status = "✅ Tests passed" if returncode == 0 else "❌ Tests failed"
    output_parts.insert(0, status)

    return [TextContent(type="text", text="\n".join(output_parts))]


async def check_coverage(arguments: dict[str, Any], TextContent) -> list:
    """
    Check code coverage.

    Args:
        arguments: Tool arguments containing path and fail_under options
        TextContent: MCP TextContent class for response formatting

    Returns:
        List of TextContent with coverage results
    """
    path = arguments.get("path", "")
    fail_under = arguments.get("fail_under", 0)

    if not path:
        return [TextContent(type="text", text="Error: 'path' parameter is required")]

    scripts_dir = os.path.join(LVGL_ROOT, "scripts")
    check_script = os.path.join(scripts_dir, "check_gcov_coverage.py")

    # Build command
    cmd = [sys.executable, check_script, "--path", path]

    if fail_under > 0:
        cmd.extend(["--fail-under", str(fail_under)])

    # Execute command
    returncode, stdout, stderr = run_command(cmd, cwd=LVGL_ROOT)

    output_parts = []
    output_parts.append(f"Command: {' '.join(cmd)}")
    output_parts.append(f"Working directory: {LVGL_ROOT}")
    output_parts.append(f"Return code: {returncode}")

    if stdout:
        output_parts.append(f"\nStandard output:\n{stdout}")
    if stderr:
        output_parts.append(f"\nStandard error:\n{stderr}")

    status = (
        "✅ Coverage check passed" if returncode == 0 else "❌ Coverage check failed"
    )
    output_parts.insert(0, status)

    return [TextContent(type="text", text="\n".join(output_parts))]


async def format_code(arguments: dict[str, Any], TextContent) -> list:
    """
    Run code formatting.

    Args:
        arguments: Tool arguments containing target option
        TextContent: MCP TextContent class for response formatting

    Returns:
        List of TextContent with formatting results
    """
    target = arguments.get("target", "")

    scripts_dir = os.path.join(LVGL_ROOT, "scripts")
    format_script = os.path.join(scripts_dir, "code-format.py")

    # Build command
    cmd = [sys.executable, format_script]

    if target:
        cmd.append(target)

    # Execute command
    returncode, stdout, stderr = run_command(cmd, cwd=LVGL_ROOT)

    output_parts = []
    output_parts.append(f"Command: {' '.join(cmd)}")
    output_parts.append(f"Working directory: {LVGL_ROOT}")
    output_parts.append(f"Return code: {returncode}")

    if stdout:
        output_parts.append(f"\nStandard output:\n{stdout}")
    if stderr:
        output_parts.append(f"\nStandard error:\n{stderr}")

    status = (
        "✅ Code formatting completed"
        if returncode == 0
        else "❌ Code formatting failed"
    )
    output_parts.insert(0, status)

    return [TextContent(type="text", text="\n".join(output_parts))]


async def gdb_debug(arguments: dict[str, Any], TextContent) -> list:
    """
    Debug test cases using GDB.

    Args:
        arguments: Tool arguments containing test_name, breakpoint, commands, max_hits
        TextContent: MCP TextContent class for response formatting

    Returns:
        List of TextContent with GDB debug output
    """
    test_name = arguments.get("test_name", "")
    breakpoint_loc = arguments.get("breakpoint", "")
    commands = arguments.get("commands", ["bt", "info locals", "continue"])
    max_hits = arguments.get("max_hits", 3)

    if not breakpoint_loc:
        return [
            TextContent(type="text", text="Error: 'breakpoint' parameter is required")
        ]

    # Find test executable
    build_dir = os.path.join(LVGL_ROOT, "tests", "build_test_sysheap")

    # If test_name is empty, list available tests
    if not test_name:
        available = glob.glob(os.path.join(build_dir, "test_*"))
        if available:
            test_names = [
                os.path.basename(test_path).replace("test_", "")
                for test_path in available
                if os.path.isfile(test_path)
            ]
            return [
                TextContent(
                    type="text",
                    text=f"Error: 'test_name' parameter is required.\nAvailable tests: {', '.join(sorted(test_names))}",
                )
            ]
        return [
            TextContent(
                type="text",
                text=(
                    "Error: 'test_name' parameter is required and no test executables "
                    "were found in the test build directory.\n\n"
                    "Make sure the LVGL tests have been built (for example, by building "
                    "the tests in 'tests/build_test_sysheap') before running this command, "
                    "or consult the project test build instructions."
                ),
            )
        ]

    test_exe = os.path.join(build_dir, f"test_{test_name}")

    if not os.path.exists(test_exe):
        # Try to find matching test file
        pattern = os.path.join(build_dir, f"test_*{test_name}*")
        matches = glob.glob(pattern)
        if matches:
            test_exe = matches[0]
        else:
            available = glob.glob(os.path.join(build_dir, "test_*"))
            return [
                TextContent(
                    type="text",
                    text=f"Error: Cannot find test executable: {test_exe}\nAvailable files: {available}",
                )
            ]

    # Build GDB command with batch mode
    cmd = [
        "gdb",
        "-batch",
        "-ex",
        "set pagination off",
        "-ex",
        "set breakpoint pending on",
        "-ex",
        f"break {breakpoint_loc}",
        "-ex",
        "run",
    ]

    # Add commands for each breakpoint hit
    for _ in range(max_hits):
        for gdb_command in commands:
            cmd.extend(["-ex", gdb_command])

    cmd.append(test_exe)

    # Run GDB
    returncode, stdout, stderr = run_command(cmd, cwd=build_dir)

    output_parts = []
    output_parts.append(f"GDB Debug: {test_exe}")
    output_parts.append(f"Breakpoint: {breakpoint_loc}")
    output_parts.append(f"Commands: {' '.join(commands)}")
    output_parts.append(f"Return code: {returncode}")

    if stdout:
        # Filter and format output
        lines = stdout.split("\n")
        filtered_lines = [
            line
            for line in lines
            if "Reading symbols" not in line and "Debuginfod" not in line
        ]
        stdout = "\n".join(filtered_lines)
        stdout = truncate_output(stdout, 15000)
        output_parts.append(f"\nGDB output:\n{stdout}")

    if stderr:
        stderr = truncate_output(stderr, 5000)
        output_parts.append(f"\nStandard error:\n{stderr}")

    status = (
        "✅ GDB debug completed"
        if returncode == 0
        else "❌ GDB debug failed (check output for details)"
    )
    output_parts.insert(0, status)

    return [TextContent(type="text", text="\n".join(output_parts))]


async def run_server():
    """Run the MCP server."""
    from mcp.server.stdio import stdio_server

    server = create_mcp_server()
    async with stdio_server() as (read_stream, write_stream):
        await server.run(
            read_stream, write_stream, server.create_initialization_options()
        )


def run_self_test(verbose: bool = False) -> bool:
    """
    Run self-test to verify basic functionality.

    Args:
        verbose: If True, print detailed test information

    Returns:
        True if all tests pass, False otherwise
    """
    print("=" * 60)
    print("LVGL MCP Server Self-Test")
    print("=" * 60)

    tests_passed = 0
    tests_failed = 0
    test_results = []

    def log_test(name: str, passed: bool, details: str = ""):
        nonlocal tests_passed, tests_failed
        if passed:
            tests_passed += 1
            status = "✅ PASS"
        else:
            tests_failed += 1
            status = "❌ FAIL"
        test_results.append((name, passed, details))
        print(f"  {status}: {name}")
        if verbose and details:
            for line in details.split("\n"):
                print(f"         {line}")

    # Test 1: Check LVGL root directory
    print("\n[1/6] Checking LVGL root directory...")
    lvgl_exists = os.path.isdir(LVGL_ROOT)
    log_test("LVGL root directory exists", lvgl_exists, f"Path: {LVGL_ROOT}")

    # Test 2: Check required scripts
    print("\n[2/6] Checking required scripts...")
    scripts_to_check = [
        ("tests/main.py", "Test runner script"),
        ("scripts/check_gcov_coverage.py", "Coverage check script"),
        ("scripts/code-format.py", "Code formatting script"),
    ]
    for script_path, description in scripts_to_check:
        full_path = os.path.join(LVGL_ROOT, script_path)
        exists = os.path.isfile(full_path)
        log_test(f"{description} ({script_path})", exists, f"Full path: {full_path}")

    # Test 3: Check tool definitions
    print("\n[3/6] Validating tool definitions...")
    try:
        tools = get_tool_definitions()
        expected_tools = [
            "lvgl_run_tests",
            "lvgl_check_coverage",
            "lvgl_format_code",
            "lvgl_gdb_debug",
        ]
        tool_names = [t["name"] for t in tools]

        for expected in expected_tools:
            found = expected in tool_names
            log_test(
                f"Tool '{expected}' defined",
                found,
                f"Found tools: {tool_names}" if not found else "",
            )

        # Validate tool schemas
        for tool in tools:
            has_schema = "inputSchema" in tool and isinstance(tool["inputSchema"], dict)
            log_test(f"Tool '{tool['name']}' has valid schema", has_schema)
    except Exception as e:
        log_test("Failed to parse tool definitions", False, f"Error: {e}")

    # Test 4: Check command execution
    print("\n[4/6] Testing command execution...")
    returncode, stdout, stderr = run_command(["echo", "test"])
    cmd_works = returncode == 0 and "test" in stdout
    log_test(
        "Command execution works",
        cmd_works,
        f"Return code: {returncode}, stdout: {stdout.strip()}",
    )

    # Test 5: Check Python executable
    print("\n[5/6] Checking Python environment...")
    py_returncode, py_stdout, _ = run_command([sys.executable, "--version"])
    py_works = py_returncode == 0
    log_test("Python executable works", py_works, f"Python: {py_stdout.strip()}")

    # Test 6: Check MCP SDK availability
    print("\n[6/6] Checking MCP SDK...")
    try:
        from mcp.server import Server
        from mcp.types import Tool, TextContent

        mcp_available = True
        mcp_details = "MCP SDK imported successfully"
    except ImportError as e:
        mcp_available = False
        mcp_details = f"Import error: {e}"
    log_test("MCP SDK available", mcp_available, mcp_details)

    # Summary
    print("\n" + "=" * 60)
    print("Test Summary")
    print("=" * 60)
    total = tests_passed + tests_failed
    print(f"  Total tests: {total}")
    print(f"  Passed: {tests_passed}")
    print(f"  Failed: {tests_failed}")

    if tests_failed == 0:
        print("\n✅ All tests passed! Server is ready to use.")
    else:
        print(f"\n⚠️  {tests_failed} test(s) failed. Please check the configuration.")

    print("\nConfiguration:")
    print(f"  LVGL root: {LVGL_ROOT}")
    print(f"  Script directory: {SCRIPT_DIR}")
    print(f"  Python: {sys.executable}")

    print("\nAvailable tools:")
    for tool in get_tool_definitions():
        print(f"  - {tool['name']}")

    return tests_failed == 0


def generate_mcp_config(output_dir: str, server_name: str = "lvgl-test") -> bool:
    """
    Generate mcp.json configuration file for VS Code.

    Args:
        output_dir: Directory where mcp.json will be created (must exist)
        server_name: Name identifier for the MCP server (default: "lvgl-test")

    Returns:
        True if generation succeeded, False otherwise
    """
    # Validate output directory
    if not output_dir:
        print("Error: Output directory must be specified")
        return False

    output_dir = os.path.abspath(output_dir)
    if not os.path.isdir(output_dir):
        print(f"Error: Output directory does not exist: {output_dir}")
        return False

    # Get the absolute path to this script
    script_path = os.path.abspath(__file__)

    # Generate configuration
    config = {
        "servers": {
            server_name: {
                "type": "stdio",
                "command": sys.executable,
                "args": [script_path],
                "env": {},
            }
        }
    }

    # Write configuration file
    output_file = os.path.join(output_dir, "mcp.json")
    try:
        with open(output_file, "w", encoding="utf-8") as f:
            json.dump(config, f, indent=4)
    except Exception as e:
        print(f"Error: Failed to write configuration file: {e}")
        return False

    print(f"✅ Generated MCP configuration: {output_file}")
    print("\n" + "=" * 60)
    print("Configuration Details")
    print("=" * 60)
    print(f"  Server name: {server_name}")
    print(f"  Script path: {script_path}")
    print(f"  Output file: {output_file}")
    print("\n" + "=" * 60)
    print("Usage Instructions")
    print("=" * 60)
    print(
        f"""
To use this MCP server with VS Code:

1. Copy the generated mcp.json to your workspace's .vscode/ directory:
   cp {output_file} <your_workspace>/.vscode/mcp.json

2. Or if the output directory is already .vscode/, you're all set!

3. Reload VS Code window (Ctrl+Shift+P -> "Developer: Reload Window")

4. The MCP server will be available in GitHub Copilot Chat

Alternative: Using ${{workspaceFolder}} variable
----------------------------------------------
If you want the config to be portable (not use absolute paths), 
you can manually edit mcp.json to use workspace-relative paths:

{{
    "servers": {{
        "{server_name}": {{
            "type": "stdio",
            "command": "python3",
            "args": [
                "${{workspaceFolder}}/lvgl/scripts/mcp_server.py"
            ],
            "env": {{}}
        }}
    }}
}}

Note: Adjust the path based on your workspace structure.
"""
    )
    return True


def main():
    """Main entry point."""
    import argparse

    parser = argparse.ArgumentParser(
        description="LVGL Test MCP Server - Tools for running tests, checking coverage, and debugging",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s                      Start the MCP server
  %(prog)s --self-test          Run self-test to verify configuration
  %(prog)s --self-test -v       Run self-test with verbose output
  %(prog)s --list-tools         List available tools
  %(prog)s --gen-config /path/to/.vscode
                                Generate mcp.json in specified directory
  %(prog)s --gen-config .vscode --server-name my-lvgl
                                Generate with custom server name
""",
    )
    parser.add_argument(
        "--self-test",
        action="store_true",
        help="Run self-test to verify server configuration and basic functionality",
    )
    parser.add_argument(
        "-v",
        "--verbose",
        action="store_true",
        help="Enable verbose output (use with --self-test)",
    )
    parser.add_argument(
        "--list-tools",
        action="store_true",
        help="List available tools and their descriptions",
    )
    parser.add_argument(
        "--gen-config",
        metavar="OUTPUT_DIR",
        type=str,
        help="Generate mcp.json configuration file in the specified directory (e.g., .vscode/)",
    )
    parser.add_argument(
        "--server-name",
        type=str,
        default="lvgl-test",
        help="Server name identifier for the MCP config (default: lvgl-test)",
    )

    args = parser.parse_args()

    if args.self_test:
        success = run_self_test(verbose=args.verbose)
        sys.exit(0 if success else 1)

    if args.list_tools:
        print("LVGL MCP Server - Available Tools")
        print("=" * 60)
        for tool in get_tool_definitions():
            print(f"\n{tool['name']}")
            print("-" * len(tool["name"]))
            # Print first line of description
            desc_lines = tool["description"].strip().split("\n")
            print(f"  {desc_lines[0]}")
        sys.exit(0)

    if args.gen_config:
        success = generate_mcp_config(args.gen_config, args.server_name)
        sys.exit(0 if success else 1)

    # Default: run the MCP server
    asyncio.run(run_server())


if __name__ == "__main__":
    main()
