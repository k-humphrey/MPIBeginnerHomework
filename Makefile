NP ?= 8
POINTS ?= 1024
OPTION ?= 1

.PHONY: Q1 Q2 Q3 Q4 Q5

all: Q1 Q2 Q3 Q4 Q5

run_roundRobin: roundRobin
	mpirun -np $(NP) roundRobin

run_roundRobinAI: roundRobinAI
	mpirun -np $(NP) roundRobinAI

run_dotProduct: dotProduct
	mpirun -np $(NP) dotProduct $(POINTS)

run_dotProductAI: dotProductAI
	mpirun -np $(NP) dotProductAI $(POINTS)

run_worldSplit: worldSplit
	mpirun -np 8 worldSplit

run_collectives: collectives
	mpirun -np $(NP) collectives $(OPTION)

run_gameOfLife: gameOfLife
	mpirun -np $(NP) gameOfLife

Q1: roundRobin roundRobinAI

Q2: dotProduct dotProductAI

Q3: worldSplit

Q4: collectives

Q5: gameOfLife

gameOfLife: 
	mpic++ Q5/gameOfLife.cpp -o gameOfLife

collectives:
	mpicc Q4/collectives.c -o collectives

worldSplit: 
	mpicc Q3/worldSplit.c -o worldSplit

dotProduct: 
	mpic++ Q2/dotProduct.cpp -o dotProduct

dotProductAI: 
	mpic++ Q2/dotProductAI.cpp -o dotProductAI

roundRobin:
	mpicc Q1/roundRobin.c -o roundRobin

roundRobinAI:
	mpicc Q1/roundRobinAI.c -o roundRobinAI


clean:
	rm -f roundRobin
	rm -f roundRobinAI
	rm -f dotProduct
	rm -f dotProductAI
	rm -f collectives
	rm -f worldSplit
	rm -f gameOfLife