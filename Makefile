

all: helikopter

run: 
	mpirun -np 10 --hostfile mpi_hosts --map-by node helikopter

helikopter:  main.cpp monitor.cpp def.h
	mpic++ main.cpp monitor.cpp -o helikopter -lpthread -lrt

clean:
	rm -f helikopter

