# planethinker_dragonlooker
This a internship test for a company renowened for its IDEs.

# BUILD INSTRUCTIONS

```
mkdir build
cmake ..
make 
```


For tests
```
make test
```



# Design

Requirements are to create a path evaluator with possible operations on them



Design:
We have two big components:
Expression parsing/evaluator 
Json parser

## Expression parsing
In the context of the project, its fair to assume these expressions are relatively small,
implying that clean code/extensability is the most important part, we choose whatever data structure so probably optimize for the json parser

However, a lot of required functionality is in here, more compiler design aspects

## Json parser
Probably biggest chunk of time will be spent in here, 
Writing parsers are hard, big space between speed, complexity and portability (SIMD).
C++ 20 does not have complete support for SIMD, only experimental
SIMD is cool, but also takes too much time so it was out of scope for now.



The current implementation is a simple LL parser. This was mostly thanks to time constraints.
Ideally I would implement a two pass model, with the first pass looking for the structure of the json document, then parsing the actual content multi-threaded.
The parsing of the structure could be extended to SIMD if benchmarks would discover that the limtiing factor would remain parsing.





## Project planning
Sadly this project happened during a really busy time for me so I prioritzed having a good tested but slow project first before focusing on pure speed.
