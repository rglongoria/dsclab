#
# Hot can in a bath simulation
# ME 144L Lab - Spring 2026
#
import math
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

def main():
	# estimate thermal time constant taut
	# You must provide taut from experimentally measured data
	taut = 1
	print(f"Estimated thermal time constant {taut:0.2f} sec")

	# T1o = initial temperature of water in cup
	T1o = 95+273.15 	# temperature in K
	# To = initial temperature of water bath (assume fixed)
	To = 25+273.15 		# temperature in K

	# Set simulation time parameters
	dt, t0, tf = 0.001, 0.0, 500
	N = math.floor(abs(tf-t0)/dt)
	x0 = np.array([T1o])
	t = np.linspace(0, tf, N)

	sol = rk4fixed(hot_can_in_a_bath, x0, t, args=(taut, To))
	T1 = sol[:,0]-273.15

	# Specify filename for data collected in cool down experiment
	fileName = "your_file_name.csv"
	# Read csv file
	data = pd.read_csv(fileName)

	# Extract data from dataframe
	tmeas  = np.array(data.Time)
	TthC = np.array(data.TthC)

	plt.plot(t, T1, c='b', label='T1')
	plt.plot(tmeas, TthC, 'g.', label='T1 meas (thermistor)', markevery=15)
	plt.axhline(y=To-273.15, color='red', linestyle='--', linewidth=2, label='Bath temp')
	plt.legend(loc="upper right")
	plt.xlabel('Time (sec)')
	plt.grid()
	plt.show()


# define the system ODEs
def hot_can_in_a_bath(x, t, taut, To):
	T1 = x[0]
	T1dot = -(T1-To)/taut

	# specify outputs
	y = 0

	return np.array([T1dot]), y

# This is a fixed-step, 4th order Runge-Kutta
# Never have to change this routine
def rk4fixed(f, x0, t, args=()):
    import numpy
    n = len(t)
    x = numpy.zeros((n, len(x0)))
    x[0] = x0
    for i in range(n - 1):
        h = t[i+1] - t[i]
        k1, _ = f(x[i], t[i], *args)
        k2, _ = f(x[i] + k1 * h / 2., t[i] + h / 2., *args)
        k3, _ = f(x[i] + k2 * h / 2., t[i] + h / 2., *args)
        k4, _ = f(x[i] + k3 * h, t[i] + h, *args)
        x[i+1] = x[i] + (h / 6.) * (k1 + 2*k2 + 2*k3 + k4)
    return x

main()