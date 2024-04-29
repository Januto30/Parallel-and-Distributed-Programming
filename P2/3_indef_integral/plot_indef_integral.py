import matplotlib.pyplot as plt
import numpy as np
import sys
import os

if len(sys.argv) != 2:
    print("Usage: python plot_indef_integral.py <filename>")
    sys.exit(1)

filename_info = sys.argv[1]
files_path = os.path.dirname(filename_info)
if(files_path != ""):
    files_path += "/"

with open(filename_info, "r") as file:
    title = file.readline().strip().lstrip('#').strip()
    filename_data = file.readline().strip().split()[1]
    line_tokens = file.readline().strip().split()
    N = int(line_tokens[1])
    line_tokens = file.readline().strip().split()
    x_max = float(line_tokens[1])
    line_tokens = file.readline().strip().split()
    size = float(line_tokens[1])


# Read the file
print("Reading data from file: ", files_path+filename_data)
data = np.fromfile(files_path+filename_data, dtype=np.float64)

# Plot the data
x_values = np.linspace(0, x_max, N)

if len(x_values) != len(data):
    print("Error: the load data has not the expeted size, please check the MPI output")
    sys.exit(1)

plt.plot(x_values, data)
plt.title(title)
plt.xlabel("x")
plt.ylabel("integral(x)")
plt.grid(True)
plt.show()
