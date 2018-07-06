# CalculateSumOfPowersMPI

## General Info
Program calculates sum of all numbers raised to the second power from given file. Numbers are divided into parts, where each processor has it's own part to calculate.

It uses MPI Library.

After each group operation, there is a timer, which shows estimated time, how long it took.

Program takes into consideration situation, where numbers are not divided into equal parts ( for example for 100000 numbers and 3 processors, there is one number left )
## Requirements
* MPI Lib
* Timer Lib ( included in project )

## How to run project

Firstly compile source code with command: ` mpic++ ScatterCalculatePow.cpp -o ScatterCalculatePow.exe timer.o `
Then run the project with command: ` mpirun -np x ScatterCalculatePow.exe `, where x stand for number of processor, which should take part in calculating

## Example:

` mpirun -np 3 ScatterCalculatePow.exe `
returns 
`0
33334
66667
Sum: 328628736
`
