"""
This script parses a docker logs file for the lv_benchmark summary results

The input looks something like:
```
{"log":"SO3: starting the initial process ...\r\n","stream":"stdout","time":"2025-03-14T20:32:17.712134853Z"}
{"log":"\r\n","stream":"stdout","time":"2025-03-14T20:32:17.712146014Z"}
{"log":"\r\n","stream":"stdout","time":"2025-03-14T20:32:17.712150624Z"}
{"log":"Starting LVGL Benchmark\r\n","stream":"stdout","time":"2025-03-14T20:32:17.816299617Z"}
{"log":"Warning: The guest is now late by 0.0 to 1.0 seconds\r\n","stream":"stdout","time":"2025-03-14T20:32:19.129030996Z"}
{"log":"Warning: The guest is now late by 1.0 to 2.0 seconds\r\n","stream":"stdout","time":"2025-03-14T20:32:40.699491404Z"}
{"log":"Warning: The guest is now late by 2.0 to 3.0 seconds\r\n","stream":"stdout","time":"2025-03-14T20:33:02.966355127Z"}
{"log":"Warning: The guest is now late by 3.0 to 4.0 seconds\r\n","stream":"stdout","time":"2025-03-14T20:33:30.06251689Z"}
{"log":"Warning: The guest is now late by 4.0 to 5.0 seconds\r\n","stream":"stdout","time":"2025-03-14T20:33:38.253535255Z"}
{"log":"Warning: The guest is now late by 5.0 to 6.0 seconds\r\n","stream":"stdout","time":"2025-03-14T20:33:51.010250882Z"}
{"log":"Benchmark Summary (9.3.0 dev)\r\r\n","stream":"stdout","time":"2025-03-14T20:33:57.855785347Z"}
{"log":"Name, Avg. CPU, Avg. FPS, Avg. time, render time, flush time\r\r\n","stream":"stdout","time":"2025-03-14T20:33:57.855997881Z"}
{"log":"Empty screen, 11%, 25, 0, 0, 0\r\r\n","stream":"stdout","time":"2025-03-14T20:33:57.857828373Z"}
{"log":"Moving wallpaper, 2%, 25, 0, 0, 0\r\r\n","stream":"stdout","time":"2025-03-14T20:33:57.858455924Z"}
{"log":"Single rectangle, 0%, 25, 0, 0, 0\r\r\n","stream":"stdout","time":"2025-03-14T20:33:57.859125276Z"}
{"log":"Multiple rectangles, 0%, 25, 0, 0, 0\r\r\n","stream":"stdout","time":"2025-03-14T20:33:57.859847899Z"}
{"log":"Multiple RGB images, 0%, 25, 0, 0, 0\r\r\n","stream":"stdout","time":"2025-03-14T20:33:57.860658884Z"}
{"log":"Multiple ARGB images, 23%, 25, 1, 1, 0\r\r\n","stream":"stdout","time":"2025-03-14T20:33:57.86156999Z"}
{"log":"Rotated ARGB images, 48%, 24, 20, 20, 0\r\r\n","stream":"stdout","time":"2025-03-14T20:33:57.862511197Z"}
{"log":"Multiple labels, 3%, 25, 0, 0, 0\r\r\n","stream":"stdout","time":"2025-03-14T20:33:57.863549175Z"}
{"log":"Screen sized text, 30%, 24, 11, 11, 0\r\r\n","stream":"stdout","time":"2025-03-14T20:33:57.864642275Z"}
{"log":"Multiple arcs, 18%, 24, 7, 7, 0\r\r\n","stream":"stdout","time":"2025-03-14T20:33:57.865801475Z"}
{"log":"Containers, 3%, 25, 0, 0, 0\r\r\n","stream":"stdout","time":"2025-03-14T20:33:57.867039657Z"}
{"log":"Containers with overlay, 88%, 21, 44, 44, 0\r\r\n","stream":"stdout","time":"2025-03-14T20:33:57.868379231Z"}
{"log":"Containers with opa, 10%, 24, 3, 3, 0\r\r\n","stream":"stdout","time":"2025-03-14T20:33:57.869736765Z"}
{"log":"Containers with opa_layer, 22%, 24, 8, 8, 0\r\r\n","stream":"stdout","time":"2025-03-14T20:33:57.871269383Z"}
{"log":"Containers with scrolling, 25%, 25, 10, 10, 0\r\r\n","stream":"stdout","time":"2025-03-14T20:33:57.872899952Z"}
{"log":"Widgets demo, 34%, 25, 13, 13, 0\r\r\n","stream":"stdout","time":"2025-03-14T20:33:57.874447759Z"}
{"log":"All scenes avg.,19%, 24, 7, 7, 0\r\r\n","stream":"stdout","time":"2025-03-14T20:33:57.875031879Z"}
{"log":"LVGL Benchmark Over\r\n","stream":"stdout","time":"2025-03-14T20:33:57.933458479Z"}
````

Outpus a json file with the format:
```json
[
    {
        "scene_name": "",
        "avg_cpu": 0,
        "avg_fps": 0,
        "avg_time": 0,
        "render_time": 0,
        "flush_time": 0,
    }
    ...
]
```
"""

import sys
import json


def main():
    if len(sys.argv) < 2:
        print("Missing output path")
        return

    output_path = sys.argv[1]
    found_start_of_results = False
    found_header = False
    results = []
    for line in sys.stdin:
        if "Benchmark Summary" in line:
            found_start_of_results = True
            continue
        if not found_start_of_results:
            continue

        fmt_line: str = json.loads(line)["log"].strip()
        cols = fmt_line.split(",")
        if len(cols) < 6:
            if fmt_line != "LVGL Benchmark Over":
                print(f"Warning found invalid log line '{fmt_line}'. Skipping...")
            continue

        if not found_header:
            found_header = True
            continue

        results.append(
            {
                "scene_name": cols[0],
                "avg_cpu": int(cols[1].replace("%", "")),
                "avg_fps": int(cols[2]),
                "avg_time": int(cols[3]),
                "render_time": int(cols[4]),
                "flush_time": int(cols[5]),
            }
        )

    with open(output_path, "w") as f:
        json.dump(results, f)


if __name__ == "__main__":
    main()
