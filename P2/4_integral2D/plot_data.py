import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D


def plot_wave_data(filename, Nx, Ny):
    wave_data = np.fromfile(filename, dtype=np.float64).reshape((Nx, Ny))

    # Define the grid. Assuming the data is centered in the squares:
    x = np.linspace(-1, 1, Nx)
    y = np.linspace(-1, 1, Ny)

    # Create a meshgrid
    X, Y = np.meshgrid(x, y, indexing='ij')

    # Plotting
    fig = plt.figure(figsize=(10, 8))
    ax = fig.add_subplot(111, projection='3d')
    surf = ax.plot_surface(X, Y, wave_data, cmap='viridis', edgecolor='none')
    fig.colorbar(surf)
    ax.set_title('Surface plot')
    ax.set_xlabel('x')
    ax.set_ylabel('y')
    ax.set_zlabel('f(x,y)')
    plt.show()


# Example usage:
# plot_wave_data('path_to_your_file.dat', Nx=100, Ny=100)

import os
if __name__ == '__main__':

    if len(os.sys.argv) != 4:
        print('Usage: python plot_data.py <filename> <Nx> <Ny>')
        exit(1)

    filename = os.sys.argv[1]
    Nx = int(os.sys.argv[2])
    Ny = int(os.sys.argv[3])
    plot_wave_data(filename, Nx, Ny)