"""
This script takes json and mpk input files and combines them

An mpk (msgpack) file contains the benchmark result history for a specific config.
Unpacked, this file will have the following format:
```json
[
    {
        "commit_hash": "<commit_hash>"
        "scenes": [
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
    },
    ...
]
```

A json file contains the benchmark result for a specific commit:

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

This script combines the json files with the mpk files and outputs new mpk files

The files are expected to follow the format 'results-<image_type>-<config_name>.[mpk|json]'

There are 3 possibilities when handling these files

1. The pair (`file1.mpk` `file1.json`) exists: Normal operation, we have a history file and some new results.
    In this case we take the information inside the `file1.json` and append it to the `file1.mpk` before exporting it

2. `file1.json` exists but `file1.mpk` doesn't: Happens when a new config and/or image type are added and there's no history yet
    In this case we create a new `file1.mpk` with a single history entry.

3. `file1.mpk` exists but `file1.json` doesn't: Happens when an old config isn't tested anymore for any reason.
    In this case we simpyl copy the old `file1.mpk`
"""

import argparse
import os
import shutil
import msgpack
import json


def save_mpk(mpk_path, mpk_data):

    with open(mpk_path, "wb") as f:
        f.write(mpk_data)


def generate_new_mpk(json_path, output_path, commit_hash):

    with open(json_path, "r") as f:
        json_data: list = json.load(f)

    mpk_data = msgpack.packb([{"commit_hash": commit_hash, "scenes": json_data}])
    save_mpk(output_path, mpk_data)


def append_json_to_mpk(mpk_path, json_path, output_path, commit_hash):
    with open(mpk_path, "rb") as f:
        mpk_data: list = msgpack.unpackb(f.read())

    with open(json_path, "r") as f:
        json_data = json.load(f)

    mpk_data.append({"commit_hash": commit_hash, "scenes": json_data})

    save_mpk(output_path, msgpack.packb(mpk_data))


def path(folder, filename):
    return os.path.join(folder, filename)


def main():
    parser = argparse.ArgumentParser(
        description="Merge new results into previous results"
    )

    parser.add_argument("--commit-hash", type=str, required=True, help="Commit Hash")
    parser.add_argument(
        "--input",
        type=str,
        required=True,
        help="Input Folder",
    )
    parser.add_argument(
        "--output",
        type=str,
        required=True,
        help="Output Folder",
    )

    args = parser.parse_args()
    input_folder: str = args.input
    output_folder: str = args.output
    commit_hash: str = args.commit_hash

    input_files = os.listdir(input_folder)
    input_mpk: set[str] = {file for file in input_files if ".mpk" in file}
    input_json: list[str] = [file for file in input_files if ".json" in file]

    for json_file in input_json:
        mpk_file = json_file.replace(".json", ".mpk")
        json_path = path(input_folder, json_file)
        output_path = path(output_folder, mpk_file)

        if mpk_file not in input_mpk:
            # First time this config is being run
            print(
                f"Couldn't find pair for {json_file} - ({mpk_file} not found). Generating new mpk file"
            )
            generate_new_mpk(json_path, output_path, commit_hash)
        else:
            print(f"Found pair ({mpk_file} {json_file}). Combining")
            mpk_path = path(input_folder, mpk_file)

            append_json_to_mpk(mpk_path, json_path, output_path, commit_hash)
            input_mpk.remove(mpk_file)

    # Keep old mpk files
    for mpk_file in input_mpk:
        print(f"Couldn't find new reults to add to {mpk_file}. Copying it")
        mpk_path = path(input_folder, mpk_file)
        output_path = path(output_folder, mpk_file)
        shutil.copy2(mpk_path, output_path)


if __name__ == "__main__":
    main()
