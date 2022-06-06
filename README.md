# Parallelization-Project
Collaborators:

1.https://github.com/VcelistMC

2.https://github.com/AbdelrahmanAli-gif

3.https://github.com/dovaaa

A small university project to parallelize Histogram Creation Using [OpenMp](https://www.openmp.org/) and [MPI](https://www.open-mpi.org/)

Project needs to be built using mpicc which is available in [MPICH package](https://www.mpich.org/)

How it Works:

you compile using the mpicc -fopenmp Parallelization_Histogram.c command

then execute using "mpiexec -n (number of processes) ./a.out

you then enter the number of points, bars, Threads(for openmp)

Output:
a Histogram Stating Ranges and The count of each point in its respective range, 
Points are entered in dataset.txt file

![image](https://user-images.githubusercontent.com/60916972/172076630-a83b34a0-e32e-411c-99e9-c17011fbc5c4.png)
