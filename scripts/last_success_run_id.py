import json
import sys
import os

if __name__ == "__main__":
	if len(sys.argv) < 2 or not os.path.exists(sys.argv[1]):
		print(f'Usage: {sys.argv[0]} <github_workflow_runs_json>')
		exit(1)
		
	json_file = open(sys.argv[1])
	data = json.load(json_file)
	for run in data["workflow_runs"]:
		if run["conclusion"] == "success":
			print(f'{run["id"]}')
			exit(0)
	
	print("0")