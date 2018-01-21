import sys
import numpy as np
from matplotlib import pyplot as plt

#given a file name, it will create a graph of the turnaround or wait time
def create_graph(file_name):
	
	x = np.array([0,5,10,15,20,25])
	#lists to hold results with quantum 50, 100, etc
	y50 = []
	y100 = []
	y250 = []
	y500 = []

	count = 0
	multiple = 6
	#handle file and add values to correct line on graph of a specific quantum value on graph
	with open(file_name,'r') as f:
		lines = f.read().splitlines()
		for num in lines:
			if count < multiple:
				y50 = np.append(y50, float(num))
			elif count < multiple*2:
				y100 = np.append(y100, float(num))
			elif count < multiple*3:
				y250 = np.append(y250, float(num))
			elif count < multiple*4:
				y500 = np.append(y500, float(num))
			count += 1
	#convert lists to numpy arrays
	y50 = np.array(y50)
	y100 = np.array(y100)
	y250 = np.array(y250)
	y500 = np.array(y500)
	#create size and attributes of graph
	plt.figure(figsize=(20,10))
	plt.title("Round Robin Scheduler - Tasks: 1000, Value: 3000")
	plt.xlabel("Dispatch Overhead")

	if file_name == 'python_input_wait.txt':
		plt.ylabel("Average Waiting Time")
	else:
		plt.ylabel("Average Turnaround Time")

	plt.plot(x,y50, label='q=50', marker='o')
	plt.plot(x,y100, label='q=100', marker='o')
	plt.plot(x,y250, label='q=250', marker='o')
	plt.plot(x,y500, label='q=500', marker='o')
	plt.legend()
	plt.show()

#Checks if file name matches and then passes the file name to a function to create a graph
def main():
	args = str(sys.argv)
	file_name = str(sys.argv[1])
	if file_name != 'python_input_wait.txt' and file_name != 'python_input_ta.txt':
		print "Please enter input file as either python_input_wait.txt or python_input_ta.txt ."
	else:
		create_graph(file_name)

if __name__ == "__main__":
    main()
