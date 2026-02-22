Ideas:
- print generations to files to check output.

- use cartesian topology, ghost cells, and possibly other data types to achieve communication.

Workflow:
1. make a working character vector printer (1D to represent a 2D grid);
2. on an even grid, figure out cell distribution (making each process replace cells with their rank to confirm boundaries)
3. 