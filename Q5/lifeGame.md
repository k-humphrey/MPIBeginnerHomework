Ideas:
- print generations to files to check output.

- use cartesian topology, ghost cells, and possibly other data types to achieve communication.

Workflow:
1. make a working character vector printer (1D to represent a 2D grid);
2. on an even grid, figure out cell distribution (making each process replace cells with their rank to confirm boundaries)
3. 

#include <mpi.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#define PRINT_ITERATIONS
#define DEBUG_PROGRAM

using namespace std;

// forward decls: mapping functions for linear distributions used in Life
// 1D just have complete rows in processes, and split evenly as possible
//
void forwardLinearMapping(int P, int N, int I, int &p, int &i); 
void inverseLinearMapping(int P, int N, int p, int i, int &I);
void partitionSize(int P, int N, int p, int &entries);

// 1D life updater
void do_update(MPI_Comm comm, char *old_map, char *new_map, int m, int N, int my_rank);
void print_array(int iteration, char *array, int M, int N, int Mdelta, int Ndelta);
  
int main(int argc, char **argv)
{
   int M=0, N=0, iterations=0;
   int size=0, rank=-1;
   char *global_array = nullptr; // used only in process 0
   int *displs =nullptr, *thecounts=nullptr; // used in process zero for Scatterv/Gatherv

   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &size);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);

   if(0 == rank)
   {
      do
      { 
         cout << "Enter Iterations: ";
         cin >> iterations;
      }
      while(iterations < 0);

      string filename;
      do
      {
	 cout << "File name to read: ";
	 cin >> filename;

      } while(0);

      // we need to read the initial map from a file, right?
      ifstream file(filename);
      if (file.is_open())
      {
        string line;

	getline(file, line);
        stringstream ss(line);
	ss >> M >> N;

	cout << "M = " << M << ", N = " << N << endl;

	// process zero makes a global array (not scalable choice):
	global_array = new char[M*N]; // no halos!
	int row = 0;
	const char *str = line.c_str();
		
        while (getline(file, line))
	{
            cout << line << endl;

	    for(unsigned long int k = 0; k < line.length(); ++k)
	      global_array[row*N + k] = (line[k] == '*') ? '*' : ' ';
	    ++row;
        }
        file.close();

      }
    else
    {
        cerr << "Unable to open file";
	MPI_Abort(MPI_COMM_WORLD, -1);
    }

   }
   
   int share_vector[3];
   if(0 == rank)
   {
      share_vector[0] = M;
      share_vector[1] = N;
      share_vector[2] = iterations;
   }
   
   MPI_Bcast(share_vector, 3, MPI_INT, 0, MPI_COMM_WORLD);
   if(0 != rank)
   {
     M          = share_vector[0];
     N          = share_vector[1];
     iterations = share_vector[2];
   }

#ifdef DEBUG_PROGRAM   
   cout << rank << ": M = " << M << ", N = " << N << ", iterations = " << iterations << endl;
#endif   

   if(iterations > 0)
   {
     // all processes know the M, N size of the life world.
     // and # of iterations to do.

     int my_rows; // linear load balanced distribution gives me this many rows:
     partitionSize(size, M, rank, my_rows);

#ifdef DEBUG_PROGRAM
     cout << rank  << ": my_rows=" << my_rows << ", N = " << N << endl;
#endif     

     // create the old and new worlds
     char *new_world = new char[(my_rows+2)*N];
     char *old_world = new char[(my_rows+2)*N]; // primitive arrays for now.
                                     // assume ROW MAJOR ORDER IN ACCESS
                                     // add halo above and below.
          // ex: index (i,j) of new_world[(i+1)*N+j]; -- skip top halo; row major


#ifdef DEBUG_PROGRAM
     cout << "Array sizes: " << (my_rows+2)*N << endl;
     
     cout << "Filling local buffers with @, #" << endl;
     for(int ii = 0; ii < my_rows; ++ii)
       for(int jj = 0; jj < N; ++jj)
       {
	 old_world[(ii+1)*N+jj] = '@';
	 new_world[(ii+1)*N+jj] = '#';
       }
     
#endif
     
     // Split the world too using MPI_Scatterv():
     //int MPI_Scatterv(const void *sendbuf, const int *thecounts, const int *displs,
     //              MPI_Datatype sendtype, void *recvbuf, int recvcount,
     //              MPI_Datatype recvtype, int root, MPI_Comm comm)

     if(0 == rank)
     {
       displs = new int[size];
       thecounts = new int[size];
	 
     // let process zero compute the displacements & counts
     // these arguments only value at the root (zero)
       displs[0] = 0;
       thecounts[0] = my_rows*N;
       for(int p = 1; p < size;  ++p)
       { 
         partitionSize(size, M, p, thecounts[p]); thecounts[p] *= N;

         displs[p] = displs[p-1] + thecounts[p-1];
       }
     }

#ifdef DEBUG_PROGRAM
     if(0 == rank)
     {
       cout << "displs:" <<endl;
       for(int ii = 0; ii < size; ++ii)
	 cout << "displs[" << ii << "] = " << displs[ii] << endl;
       cout << endl;

       cout << "thecounts:" <<endl;
       for(int ii = 0; ii < size; ++ii)
	 cout << "thecounts[" << ii << "] = " << thecounts[ii] << endl;
       cout << endl;

     }
     
     if(0 == rank)
     {  
        cout << "Before Scatterv (old_world):" << endl;
        print_array(-1, old_world, my_rows, N, 1, 0);

     }
#endif     

#if 1
     // receive into old_world at the halo-adjusted offset (skip top row)
     MPI_Scatterv(global_array, thecounts, displs, MPI_CHAR, &old_world[N], my_rows*N,
		  MPI_CHAR, 0, MPI_COMM_WORLD);
     // keep displs and thecounts for printing function to be done later!
#else
     {
     cout << "size = " << size << endl;
       
     MPI_Request requests[size+1];

     // all do this:
     MPI_Irecv(old_world+N, my_rows*N, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &requests[size]);

     cout << "after Irecv " << endl;

     if(0 == rank)
     {  
        for(int msgs = 0; msgs < size; msgs++)
	{
	   cout << "msg = " << msgs << endl;;
	  
	   MPI_Isend(global_array+displs[msgs], thecounts[msgs],MPI_CHAR, msgs, 0, MPI_COMM_WORLD, &requests[msgs]);
	}
     }
     cout <<"Before Waitall/wait" << endl;
     
     if(0 == rank)
       MPI_Waitall(size+1, requests, MPI_STATUSES_IGNORE);
     else
       MPI_Wait(&requests[size], MPI_STATUS_IGNORE);
     }	   
#endif     

#ifdef DEBUG_PROGRAM
     if(0 == rank)
     {  
        cout << "After Scatterv (old_world):" << endl;
        print_array(-1, old_world, my_rows, N, 1, 0);

     }
#endif     

     for(int i = 0; i < iterations; ++i)
     {
        do_update(MPI_COMM_WORLD, old_world, new_world, my_rows, N, rank);

#ifdef PRINT_ITERATIONS
	// if we want to print, we need to aggregate map back to process 0,
	// and print... use MPI_Gatherv()...

	//int MPI_Gatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
        //      void *recvbuf, const int recvcounts[], const int displs[],
        //      MPI_Datatype recvtype, int root, MPI_Comm comm)

	// skip the halo at top when gathering:

#ifdef DEBUG_PROGRAM
	if(0 == rank)
	{
	    for(int ii = 0; ii < M; ++ii)
    	      for(int jj = 0; jj < N; ++jj)
	         global_array[ii*N+jj] = 'x';
	}
#endif	
	
	MPI_Gatherv(&new_world[N], my_rows*N, MPI_CHAR, global_array, thecounts, displs,
		    MPI_CHAR, 0, MPI_COMM_WORLD);

	if(0 == rank)
	{
 	   print_array(i, global_array, M, N, 0, 0);
	}
#endif	

	// swap state:
	char *tmp = old_world;
	old_world = new_world;
	new_world = tmp;

      }

     delete[] old_world;
     delete[] new_world;

     old_world = nullptr;
     new_world = nullptr;
   }

   if(0 == rank)
   {
      delete[] global_array;
      global_array = nullptr;
   }

   MPI_Finalize();
}


// life specific:

bool update_element(char *old_world, int m, int N, int i, int j) // m not used here.
{
  // if dead, and three neighbors, life next iteration
  // if alive, 2 or 3 neighors, stay alive
  //
  const int i_idx = (i+1); // this is always in range 1...m
  
  int sum = (old_world[(i_idx-1)*N+j]           == '*') + // N
            (old_world[(i_idx+1)*N+j]           == '*') + // S
            (old_world[(i_idx)*N+((j-1+N)%N)]   == '*') + // W
            (old_world[(i_idx)*N+((j+1)%N)]     == '*') + // E
            (old_world[(i_idx-1)*N+((j-1+N)%N)] == '*') + // NW
            (old_world[(i_idx-1)*N+((j+1)%N)]   == '*') + // NE
            (old_world[(i_idx+1)*N+((j-1+N)%N)] == '*') + // SW
            (old_world[(i_idx+1)*N+((j+1)%N)]   == '*');  // SE

  bool health = ((old_world[i_idx*N+j] == '*') && (sum==2 || sum==3))
                || (sum==3);

  return health;
  
}

void do_update(MPI_Comm comm, char *old_map, char *new_map, int m, int N, int my_rank)
{
  int size;
  MPI_Request requests[4];

  MPI_Comm_size(comm, &size);

  const int TOP =1;
  const int BOTTOM =2;
  // halo exchange up and down :-)
  // fill in halos of old_map so that the inner computation works.

    // int MPI_Irecv(void *buf, int count, MPI_Datatype datatype, int source, int tag,
  //              MPI_Comm comm, MPI_Request *request)

  // Best practice: post receives before sends to avoid unexpected receives!
  
  // Receive from process below into my bottom halo:
  MPI_Irecv(&old_map[(m+1)*N+0], N, MPI_CHAR, (my_rank+1)%size, TOP, comm, &requests[0]);

  // Receive from process above into my top halo:
  MPI_Irecv(&old_map[0*N+0], N, MPI_CHAR, (my_rank+size-1)%size, BOTTOM, comm, &requests[1]);

  // send my top row to process one above me into its halo row
  //int MPI_Isend(const void *buf, int count, MPI_Datatype datatype, int dest,
  //          int tag, MPI_Comm comm, MPI_Request *request)
  MPI_Isend(&old_map[1*N+0], N, MPI_CHAR, (my_rank+size-1)%size, TOP, comm, &requests[2]);
  
  // send my bottom row to process one below me into its halo
  MPI_Isend(&old_map[(m-1+1)*N+0], N, MPI_CHAR, (my_rank+1)%size, BOTTOM, comm, &requests[3]);

  // complete all 4 transfers.
  MPI_Waitall(4, requests, MPI_STATUSES_IGNORE);

  for(int i = 0; i < m; ++i) // rows
  {
    for(int j = 0; j < N; ++j)
    {
      bool health = update_element(old_map, m, N, i, j);
      new_map[(i+1)*N+j] = health ? '*' : ' ';
    }
  }
}

// linear mapping functions:

#if 0
void forwardLinearMapping(int P, int N, int I, int &p, int &i)
{

   int L = N/P; // nominal amount of entries per partition
   int R = N%P; // any extras don't fit

   // for an index I, what partition does it go into?
   // and at what offset?
   // for b!=0, (a/b)*b = a - a mod b
   if(R == 0)
   {
      p = I/L;
      i = I-p*L;
   }
   else // non-divisibility
   {
     if(I < (L+1)*R)
     {
        p = I/(L+1);
        i = I - (L+1)*p;
     }
     else
     {
        p = R + (I -(L+1)*R)/L;
        i = I - R*(L+1) - (p-R)*L;
     }
   }
}


void inverseLinearMapping(int P, int N, int p, int i, int &I)
{

   int L = N/P; // nominal amount of entries per partition
   int R = N%P; // any extras don't fit

   // for an index I, what partition does it go into?
   // and at what offset?

   // for b!=0, (a/b)*b = a - a mod b
   if(R == 0)
   {
      I = i + p*L;
   }
   else // non-divisibility
   {
     if(I < (L+1)*R)
     {
        I = i + (L+1)*p;
     }
     else
     {
       // i = I - R*(L+1) - (p-R)*L;
       I = i + R*(L+1) + (p-R)*L;
     }
   }
}
#endif

void partitionSize(int P, int N, int p, int &entries)
{
   int L = N/P; // nominal amount of entries per partition
   int R = N%P; // any extras don't fit
   if (p < R)
     entries = (L+1);
   else
     entries = L;
}

void print_array(int iteration, char *array, int M, int N, int Mdelta=0, int Ndelta=0)
{
  cout << "Iteration " << iteration << ":" << ", M = " << M << ", N = " << N << endl;
  for(int i = 0; i < M; ++i)
  {
    for(int j = 0; j < N; ++j)
    {
      cout << array[(i+Mdelta)*(N+2*Ndelta)+(j+Ndelta)];
#if 0      
      int ix = array[(i+Mdelta)*(N+2*Ndelta)+(j+Ndelta)];
      cout << ix << " ";
#endif      
    }
    cout << endl;
  }
  
}

    
