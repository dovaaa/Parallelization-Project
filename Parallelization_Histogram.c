#include <omp.h>
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

void readFromFile(int *arr)
{
    FILE *file;
    file = fopen("dataset.txt", "r");
    int line[20];
    int i = 0;
    while (fscanf(file, "%d", line) == 1)
    {
        arr[i] = *line;
        i++;
    };
}
int Round(double num)
{
    return (int)(num + 0.5);
}

int min(int a, int b)
{
    return a>b? b: a;
}

int main(int argc, char *argv[])
{
    int rank, pSize;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &pSize);

    int numberOfPoints = 20, numberOfBars = 4, numberOfThreads = 5, numberOfProcesses;
    int *splittedData, splittedDataSize;                             /*Section for data recieved by each process and size*/
    int *points /* points array to hold all points*/, *localMaximas; // array of local maximas for each process
    int localMax = -1;                                               // local maxima
    int globalMax = 0, step = 0;
    int *ranges_array, *count_array;

    if (rank == 0)
    {
        printf("Enter number of points, bars, and threads\n");
        // read user input for number of points/ number of bars/ number of threads
        scanf("%d %d %d", &numberOfPoints, &numberOfBars, &numberOfThreads);

        // pad array if number of points not divisible by process size
        while ((numberOfPoints % pSize) != 0)
            numberOfPoints++;

        points = calloc(numberOfPoints, sizeof(int));
        localMaximas = calloc(pSize, sizeof(int));

        // initilize array
        for (int i = 0; i < numberOfPoints; i++)
            points[i] = -1;

        // read dataset file into array
        readFromFile(points);
        splittedDataSize = numberOfPoints / pSize;
        splittedData = calloc(splittedDataSize, sizeof(int));

        int i = 1;
        for (; i < pSize; i++)
        {
            MPI_Send(&splittedDataSize, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
        // send data
        MPI_Scatter(points, splittedDataSize, MPI_INT, splittedData, splittedDataSize, MPI_INT, 0, MPI_COMM_WORLD);
        for (i = 0; i < splittedDataSize; i++)
        {
            if (splittedData[i] > localMax)
            {
                localMax = splittedData[i];
            }
        }
        // gather local max from each process
        MPI_Gather(&localMax, 1, MPI_INT, localMaximas, 1, MPI_INT, 0, MPI_COMM_WORLD);

        for (int i = 0; i < pSize; i++)
        {
            if (globalMax < localMaximas[i])
                globalMax = localMaximas[i];
        }
        step = Round((double)globalMax / (double)numberOfBars);
        ranges_array = calloc(numberOfBars, sizeof(int));
        count_array = calloc(numberOfBars, sizeof(int));
        for (int i = 0; i < numberOfBars; i++)
        {
            ranges_array[i] = step * (i + 1); //[step*1,step*2,step*3,step*4] for number bars = 4 say step = 6 so [6,12,18,24]
            count_array[i] = 0;
        }
       
        int ppThread = (numberOfPoints + numberOfThreads - 1) / numberOfThreads;
        omp_set_num_threads(numberOfThreads);
        
        #pragma omp parallel shared(points)
        {
            int rank = omp_get_thread_num();
            int startidx = ppThread * rank;
            int endidx = min(startidx + ppThread, numberOfPoints);

            for (int i = startidx; i < endidx; i++)
            {
                if (points[i] == -1 || points[i] == 0)
                    continue;
                for (int j = 0; j < numberOfBars; j++)
                {
                    if (points[i] <= ranges_array[j])
                    {
                        #pragma omp critical
                        count_array[j]++;
                        break;
                    }
                }
              }
            
        }
        for(int i = 0; i < numberOfBars; i++){
            if(i==0)printf("The range start with 0, end with %d with count %d\n",ranges_array[i],count_array[i]);
            if(i>0) printf("The range start with %d, end with %d with count %d\n",ranges_array[i-1],ranges_array[i],count_array[i]);
        }
    }
    else
    {
        MPI_Recv(&splittedDataSize, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        splittedData = calloc(splittedDataSize, sizeof(int));

        MPI_Scatter(NULL, 0, MPI_INT, splittedData, splittedDataSize, MPI_INT, 0, MPI_COMM_WORLD);
        int i;
        for (i = 0; i < splittedDataSize; i++)
        {
            if (splittedData[i] > localMax)
            {
                localMax = splittedData[i];
            }
        }
        MPI_Gather(&localMax, 1, MPI_INT, NULL, 0, MPI_INT, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
