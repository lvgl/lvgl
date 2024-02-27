import os
import sys

class MalformedInputError(RuntimeError):
	pass

class FuncData:
	def __init__(self, name: str = "", parent: str = "", info: str = "", time: float = .0):
		self.name = name # Function's name
		self.parent = parent # Function parent's name
		self.info = info # Optionnal supplementary data (currently unused)
		self.time = time # Function's execution time

	def __str__(self) -> str:
		my_str = "FuncData object{\nname:" 
		my_str += str(self.name) 
		my_str += "\nparent:" 
		my_str += str(self.parent) 
		my_str += "\ninfo:" 
		my_str += str(self.info) 
		my_str += "\ntime:" 
		my_str += str(self.time) 
		my_str += "\n}"
		return my_str


	def from_string(string: str) -> 'FuncData':
		func_data = FuncData()
		symbol_idx = 0
		# Separate symbols of string
		data = string.strip().split(" ")
		try:
			# Retrieve function's name and parent (if given)
			func = data[symbol_idx].split("/")
			if len(func) > 1:
				func_data.name = func[1]
				func_data.parent = func[0]
			else:
				func_data.name = func[0]
			symbol_idx += 1
			# Retrieve infos
			if "(" in data[symbol_idx]:
				func_data.info = data[symbol_idx].strip("()")
				symbol_idx += 1
			# Check that | separator is used before data
			if data[symbol_idx] == "|": symbol_idx += 1
			else: raise MalformedInputError("Required '|' separator is missing")
			# Retrieve data
			func_data.time = float(data[symbol_idx])
		except IndexError:
			raise MalformedInputError("Some required data is missing")
		except ValueError:
			raise MalformedInputError("Given data value cannot be converted to float")
		except OverflowError:
			raise MalformedInputError("Given data value is too big")
		
		return func_data


"""
	Input files are expected to have the following format:
	One header line with no defined format
	N lines with function data following the "[parent/]<name> [(info)] | <time>" format
	
	parent and info are optionnal. 
	time is expected to be a single value convertible to float. Eventual excess values will be discarded
"""

def get_func_data_from_file(file: str) -> dict[dict[float]]:
	data: dict[dict[float]] = dict()
	data_file = open(file)
	# Discard header line
	data_file.readline()
	# Parse data
	for line in data_file:
		# Parse line
		new_data: FuncData = FuncData.from_string(line)
		# Check if function already exists
		if new_data.name in data.keys():
			#Check that function wasn't already called from same parent
			if new_data.parent in data[new_data.name].keys():
				raise NotImplementedError("Current script does not support more than one execution time per parent-func pair")
		else: 
			data[new_data.name] = dict()
		#Store data
		data[new_data.name][new_data.parent] = new_data.time
	
	return data

def check_threshold(exec_times: dict[dict[float]], 
					thresholds: dict[dict[float]], 
					func: str, 
					parent: str) -> bool:
	if func not in thresholds.keys():
		print(f'WARN: No threshold set for function {func}')
		return True
	
	if parent in thresholds[func].keys():
		threshold = thresholds[func][parent]
	elif "" in thresholds[func].keys():
		print(f'INFO: Using default threshold for function {func} called from {parent}')
		threshold = thresholds[func][""]
	else:
		print(f'WARN: No default threshold set for function {func} called from {parent}')
		return True
			
	exec_time = exec_times[func][parent]
	if exec_time > threshold:
		print(f'FAIL: Function {func} called from {parent} executed in {exec_time}s but threshold is set to {threshold}s')
		return False
	
	return True

def check_prev_exec(exec_times: dict[dict[float]], 
					prv_exec_times: dict[dict[float]], 
					func: str, 
					parent: str):
	try:
		exec_time = exec_times[func][parent]
		prev_exec_time = prev_exec_times[func][parent]
		if exec_time > prev_exec_time:
			print(f'WARN: Function {func} called from {parent} slowed from {prev_exec_time}s to {exec_time}s')
	except KeyError:
		print(f'INFO: No previous execution of function {func} called from {parent}')
	return



if __name__ == "__main__":
	if len(sys.argv) < 3 or len(sys.argv) > 4:
		print(f'Usage: python3 {sys.argv[0]} <execution_time_file> <threshold_file> [previous_execution_time_file]')
		exit(-1)
	
	EXEC_TIME_FILE = sys.argv[1]
	THRESHOLD_FILE = sys.argv[2]
	PREV_EXEC_TIME_FILE = "" if len(sys.argv) < 4 else sys.argv[3]
	prev_perf_exists = os.path.exists(PREV_EXEC_TIME_FILE)

	func_over_threshold = 0

	thresholds = get_func_data_from_file(THRESHOLD_FILE)
	exec_times = get_func_data_from_file(EXEC_TIME_FILE)
	if prev_perf_exists:
		prev_exec_times = get_func_data_from_file(PREV_EXEC_TIME_FILE)

	for func in exec_times.keys():
		for parent in exec_times[func].keys():
			
			if not check_threshold(exec_times, thresholds, func, parent):
				func_over_threshold += 1
				continue

			if prev_perf_exists:
				check_prev_exec(exec_times, prev_exec_times, func, parent)
			
	if func_over_threshold > 0:
		print(f'Performance check failed: {func_over_threshold} functions over set threshold')
			
	exit(func_over_threshold)