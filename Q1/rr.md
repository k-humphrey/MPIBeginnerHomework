PROMPT INFO IS IN "RoundRobinAI.c" *****

Reflection:
After I used AI, I noticed that I probably didn't understand the prompt initially.
I still am not sure whether I'm supposed to increment everytime a message is passed, or
if I wanted to increment every full revolution around the loop.
I have decided to just keep the two seperate interpretations of the problem, as they are really similar in functions.

I didn't understand that "during compile time" meant to hard code the number of revolutions.
I will be changing mine to what the AI did, because it works fine and makes sense to me.

I also liked the elegant way that AI divised who to send and recieve from. I will revise my code to do the same. Shown below:
next = (rank + 1) % size;
prev = (rank - 1 + size) % size;

Everything else AI chose to do was the same as my program. I have messed with MPI just a tiny bit before from the LLNL tutorial,
so maybe we are pulling our inspiration from the same place on the web.