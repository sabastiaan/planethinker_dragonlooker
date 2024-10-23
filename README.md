# planethinker_dragonlooker


This a internship test for a company renowened for its IDEs.

Requirements are to create a path evaluator with possible operations on them:

Prioritzations:
Speed
Full utf support

Design:
We have two big components:
Expression parsing/evaluator 
Json parser
Path evuator

## Expression parsing
In the context of the project, its fair to assume these expressions are relatively small,
implying that clean code/extensability is the most important part, we choose whatever data structure so probably optimize for the json parser

However, a lot of required functionality is in here, more compiler design aspects

## Json parser
Probably biggest chunk of time will be spent in here, 
Writing parsers are hard, big space between speed, complexity and portability (SIMD).
C++ 20 does not have complete support for SIMD, only experimental
SIMD is cool, but also takes too much time



Overal design:
SIMD parsing is cool, but to deal with UTF and nested \ is a hard.
Therefore we opt for a two pass model:
The first phase is dispatching, we go over the json structure and dispatch on top level items (or lower),
then for each we spawn a parsing thread.

This is nice since we have a nice set of upgrade paths:
For spawning a thread on the top level -> simplest case
Can extend to SIMD later on for the structure 




## Project planning
Most important task is to get somethign that forfills all requirements
Second is to make it fasest.

Structure of the project:
    1. Get basic complete implementaiton
    2. Create a couple of simple benchmarks for performance
    3. Multi-threading 