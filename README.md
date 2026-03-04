# MPIBeginnerHomework
assignment 1 Parallel computing 

# Make commands
These commands can be used from the main directory to help you build, run, and clean up everything faster.
make all - this will build all executables

make Q1-Q5 - this can make individual questions executables

make run_executablename NP=# POINTS=# OPTION=(1-5)- this runs a certain program,
NP is number of processes,
POINTS is how many points you want to generate with the dotProduct program.,
OPTION is a number that represents these choices for the collectives program:
    - 1 = Broadcast and reduce
    - 2 = All reduce

    - 3 = gather and broadcast
    - 4 = allgather

    - 5 = all to all

You dont have to set these values as they have a default of 8, 1024, and 1.

make clean - this removes all executable files


# installation with WSL
1. wsl --install
2. wsl --version
3. wsl --install Ubuntu
4. create user and password
5. sudo apt install libopenmpi-dev openmpi-bin


