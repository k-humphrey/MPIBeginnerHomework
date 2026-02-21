https://lumetta.web.engr.illinois.edu/408-Sum25/slide-copies/ece408-lecture11-reduction%20tree-Sum25-x4.pdf
^link used to help with my tree reduction (non-AI part)

Funny thing with this non ai part. I spent about 2 hours trying to figure out my tree reduction. In the end, I had just accidentally used the wrong variable to stop the reduction when it was finished. Sigh....

AI: Chatgpt
AI Prompt used:
"Code this in c++: Write a parallel vector-vector element by element multiplication program (by hand, no LLM) followed by
a tree reduction to compute the dot-product of two vectors of length N with P processes using MPI. For
instance, show with N = 1024,2048,4096 and P = 1,2,4.
Use only point-to-point operations. Code your own tree reduction using sends and receives.
Divide the data as equally as you can between your processes. We will discuss how to compute these
partitions in class early next week (week of February 16).
Use MPI
Wtime() to measure the cost of the operation as you vary P and N. We will explain how to use
this operation in lecture.
initialize each vector to twos."

When I compare the two programs, I see that mine is pretty similar. The math I did vs the math that the AI did gets the same outcome, but just in a different way. 
AI totally wins here,, it remembers variables better than I do. and its faster..


Lets compare output
Mine: np 1
The final sum is: 4192
This took process with rank 0, 3.232e-06 seconds

AI: np 1
N = 1048  P = 1  Dot Product = 4192  Time = 5.406e-06 seconds

Mine: np 4
The final sum is: 4192
This took process with rank 0, 1.7335e-05 seconds

AI: np 4
N = 1048  P = 4  Dot Product = 4192  Time = 2.3731e-05 seconds

Mine: np 8
The final sum is: 4192
This took process with rank 0, 0.000429406 seconds

AI: np 8
N = 1048  P = 8  Dot Product = 4192  Time = 0.000193627 seconds