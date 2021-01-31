#include <stdio.h>
#include "mpi.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

int main(int argc, char** argv) {
    
    int n,k,i;                          //n == plhthos arithmon
    int my_rank, p;                     //p == plhthos processors
    int partition;           //partition == arithmoi ana processor
    int target;
    //int data[100];         //For testing purposes is commented
    int finAns=1,ans=1;                 // if (finAns==1) tote sorted
    int data_loc[100];
    int firstLast[100];
    int tag1 =1, tag2 =2, tag3 = 3, tag4 = 4, tag5 = 5;        
                            //tag1 gia plhthos arithmon
                            //tag2 gia paralavei meridiou
                            //tag3 gia topika apotelesmata
                            //tag4 gia epistrofh prwtou
                            //tag5 gia epistrofh teleutaiou
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Status status;
    int data[20] = {1, 4 ,5 ,6, 77, 2, 6, 7, 9, 10, 49, 5, 2, 66, 10, 4, 5, 76, 5, 3}; n = 20;
    if (my_rank == 0)  {
        
        /*printf("Dose plithos aritmon:\n");
        scanf("%d", &n);
        printf("Dose tous %d arithmous:\n", n);             //For testing purposes is commented
        
        for (k=0; k<n; k++)
            scanf("%d", &data[k]);*/
            
          
  
        for (target = 1; target < p; target++) 
            
            MPI_Send(&n, 1, MPI_INT, target, tag1, MPI_COMM_WORLD);     //steile to plhthos ton arithmon stoyw yploipous
            partition = n/p;  k=partition;
            //printf("Hello %d\n n = %d\n p = %d",partition, n, p);
        for (target = 1; target < p; target++)  {
            
            MPI_Send(&data[k], partition, MPI_INT, target, tag2, MPI_COMM_WORLD);   //steile ston kathe epe3ergasth to meridio tou
            k += partition;  
        }
        for (k=0; k<partition; k++)
            data_loc[k]=data[k];    //to meridio tou rank = 0 (master)
    }
    else  {
        MPI_Recv(&n, 1, MPI_INT, 0, tag1, MPI_COMM_WORLD, &status);      //parelave to plhthos ton arithmon
        partition = n/p;                                                 // ftia3e to partition
        MPI_Recv(&data_loc[0], partition, MPI_INT, 0, tag2, MPI_COMM_WORLD, &status);    //parelave to meridio sou
    }
    
    for(k=1;k<partition-1;k++){             //ekteleite apo OLOUS
        if(data_loc[k-1]>data_loc[k]) {     //elegxos ta3inomhshs
            ans = 0;
            }
    }
    /*if (my_rank != 0)  {
        MPI_Send(&data_loc[0], 1, MPI_INT, 0, tag4, MPI_COMM_WORLD);        //oi loipoi epe3ergastes epistrefoun to prwto tous
        MPI_Send(&data_loc[partition], 1, MPI_INT, 0, tag5, MPI_COMM_WORLD);//oi loipoi epe3ergastes epistrefoun to teleutaio tous
    }
    else {
        firstLast[0] = data_loc[0];
        firstLast[1] = data_loc[partition-1];
        for (target = 1; target < p; target++)  {
            MPI_Recv(&firstLast[target+1], 1, MPI_INT, target, tag4, MPI_COMM_WORLD);   
            MPI_Recv(&firstLast[target+2], 1, MPI_INT, target, tag5, MPI_COMM_WORLD);
            k+=partition;  
        }
    }*/
    
    firstLast[0] = data_loc[0];
    firstLast[1] = data_loc[partition-1];
    i=0;
    for(k=partition+1;k<n;k+partition) {
        firstLast[i] = data[k];
        firstLast[i+1] = data[k+partition-1];
        i += 2;
    }
    
    for(k=1;k<i;k++){             
        if(firstLast[k-1]>firstLast[k]) {     //Teleutaios elegxos ta3inomhshs
            ans = 0;
            }
        
        if (my_rank != 0)  {
            MPI_Send(&ans, 1, MPI_INT, 0, tag3, MPI_COMM_WORLD);    //oi loipoi epe3ergastes epistrefoun ta TOPIKA apotelesmata tous
        }
        else  {
            finAns = ans;
            printf("\n Apotelesma of process %d: %d\n", my_rank, ans);
            for (k = 1; k < p; k++)  {
                MPI_Recv(&ans, 1, MPI_INT, k, tag3, MPI_COMM_WORLD, &status);   //o main cpu lamvanei ta TOPIKA apotelesmata
                finAns = finAns && ans;
                printf("\n Apotelesma of process %d: %d\n", k, ans);
            }
        printf("\n\n\n Teliko Apotelesma: %d\n", finAns);
    
        }
    }
    
    MPI_Finalize();
    return 0;
}   
    
    

