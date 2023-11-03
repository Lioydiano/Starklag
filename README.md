# Starklag

Starklag, which is a contraction of *"den starkastes lag"* which in Swedish means *"the law of the strongest"*, is a partial evolution simulator with real time view on the field, logging, statistics and graphical representation of the population and of the expressions of the genes.

The simulator is written in `C++17` and uses the [`Sista`](https://github.com/FLAK-ZOSO/Sista) library for the real time field view.

The statistics are dumped in `.txt` files, they can be plotted using the `Python` script in the `demo` folder, which uses `matplotlib`.

## Supported platforms

The simulator has been tested on `Linux` and `Windows`, is reported to work on `MacOS` and should work on any platform that supports `C++17` and `g++`, since `Sista` is cross-platform.

## Dependencies

The only dependencies are the [`Sista`](https://github.com/FLAK-ZOSO/Sista) library and `matplotlib` for the plotting script.

## Installation

### Download

You can download the source code using `git`...

```bash
git clone https://github.com/Lioydiano/Starklag
```

...or you can download the latest zip file from the [releases](https://github.com/Lioydiano/Starklag/releases) page.

### Compilation

You only need to compile the `starklag.cpp` file using `g++` with the `-std=c++17` flag.

```bash
g++ -std=c++17 starklag.cpp -o starklag
```

## Usage

### Folder structure

First, you should create a folder like `demo/1`, let's call it `demo/{i}`, and navigate to it.

```bash
../../starklag
```

This will make `demo/{i}` the folder where the simulation data will be stored.

```bash
pc:~/Starklag/demo/2$ tree
.
├── attack-stats.png
├── attack-stats.txt
├── debug.txt
├── defense-stats.png
├── defense-stats.txt
├── dna-stats.txt
├── fertility-stats.png
├── fertility-stats.txt
├── lifespan-stats.png
├── lifespan-stats.txt
├── nature-stats.png
├── nature-stats.txt
├── organisms-count.png
├── organisms-count.txt
├── speed-stats.png
├── speed-stats.txt
├── start.bash
├── strength-stats.png
├── strength-stats.txt
├── vision-stats.png
└── vision-stats.txt
```

This is the content of the folder after the simulation has ended and the plotting is done.

### Running the simulation

Anyway, you should better do the two steps separately, since starklag's simulations don't always end as expected.

```bash
../../starklag
cd ../..
python3 graphics.py <insert-{i} here>
```

### Input

As of `v0.4`, you can only input the following...

- *"pause`<ENTER>`"* to pause the simulation
- *"resume`<ENTER>`"* to resume the simulation
- *"quit`<ENTER>`"* to quit the simulation

### Output

The output is a real time view of the field, with the organisms represented by ANSI-styled uppercase letters (`[A-Z]`) and the food represented by a green `@`.

It is suggested to keep a zoom that allows you to see the whole field in height, but not in width, so that you can see the side logs.

## Example

The following is an example of a simulation after 9 frames.

```txt
####################################################
#                                               @  #
#                                                  #
#    @                                       @     #
#                   B           @         @        #
#              C         @                         #
#               N            V   @                 #
#M    @        U                           @       #
#                           @    @                 #
#     @      @           L                         #
#                    I             @   Q          @#
#        @                                         #
#         @                                        #
#                    P          @K                 #
#  D                                   @           #
#             E       W                            #
#             F                 @                  #
#                                                  #
#    @                                    @        #
#                                          @Y     @#
#                    @         @                   #
#   Z   H         S                G           O   #
#                     @       @                    #
#                                                  #
#    @                         @                @  #
#          @      @                                #
#                                                  #
#       @                        @           R     #
#     A                           @                #
#  X               J   @      T @                 @#
#                                                  #
####################################################
```

The logs on the right side of the field are like the following.

```txt
Organism A 0 (9): 10hp, 1591 left, DNA: {2 : 1 : 1 : 5 : 2 : 1 : 1 : 1} {27, 5}
Organism B 1 (9): 20hp, 91 left, DNA: {1 : 2 : 0 : 1 : 2 : 1 : 2 : 1} {3, 19}
Organism C 2 (9): 10hp, 391 left, DNA: {1 : 1 : 1 : 2 : 1 : 2 : 1 : 1} {4, 14}
Organism D 3 (9): 10hp, 391 left, DNA: {1 : 2 : 1 : 2 : 1 : 1 : 1 : 1} {13, 2}
Organism E 4 (9): 10hp, 391 left, DNA: {0 : 1 : 2 : 2 : 2 : 2 : 1 : 2} {14, 13}
Organism F 5 (9): 10hp, 391 left, DNA: {1 : 0 : 0 : 2 : 1 : 3 : 1 : 2} {15, 13}
Organism G 6 (9): 20hp, 91 left, DNA: {1 : 1 : 1 : 1 : 1 : 0 : 2 : 0} {20, 34}
Organism H 7 (9): 9hp, 891 left, DNA: {0 : 1 : 1 : 3 : 1 : 1 : 1 : 1} {20, 7}
Organism I 8 (9): 10hp, 891 left, DNA: {1 : 1 : 1 : 3 : 2 : 1 : 1 : 1} {9, 20}
Organism J 9 (9): 20hp, 91 left, DNA: {1 : 2 : 1 : 1 : 2 : 0 : 2 : 1} {28, 18}
Organism K 10 (9): 10hp, 1591 left, DNA: {2 : 2 : 1 : 4 : 2 : 2 : 1 : 0} {12, 32}
Organism L 11 (9): 10hp, 91 left, DNA: {0 : 1 : 1 : 1 : 2 : 1 : 1 : 1} {8, 24}
Organism M 12 (9): 10hp, 391 left, DNA: {1 : 1 : 0 : 2 : 2 : 1 : 1 : 0} {6, 0}
Organism N 13 (9): 20hp, 91 left, DNA: {0 : 1 : 2 : 1 : 1 : 1 : 2 : 0} {5, 15}
Organism O 14 (9): 10hp, 891 left, DNA: {2 : 2 : 1 : 3 : 2 : 1 : 1 : 1} {20, 46}
Organism P 15 (9): 20hp, 391 left, DNA: {1 : 0 : 1 : 2 : 2 : 1 : 2 : 1} {12, 20}
Organism Q 16 (9): 20hp, 391 left, DNA: {0 : 1 : 1 : 2 : 2 : 0 : 2 : 1} {9, 38}
Organism R 17 (9): 10hp, 91 left, DNA: {1 : 0 : 1 : 1 : 2 : 1 : 1 : 1} {26, 44}
Organism S 18 (9): 10hp, 391 left, DNA: {0 : 0 : 1 : 2 : 2 : 2 : 1 : 1} {20, 17}
Organism T 19 (9): 10hp, 391 left, DNA: {2 : 0 : 1 : 2 : 2 : 2 : 1 : 1} {28, 29}
Organism U 20 (9): 10hp, 891 left, DNA: {1 : 0 : 0 : 3 : 2 : 2 : 1 : 1} {6, 14}
Organism V 21 (9): 10hp, 391 left, DNA: {1 : 1 : 2 : 2 : 4 : 0 : 1 : 0} {5, 28}
Organism W 22 (9): 10hp, 391 left, DNA: {1 : 0 : 2 : 2 : 2 : 0 : 1 : 0} {14, 21}
Organism X 23 (9): 10hp, 391 left, DNA: {0 : 1 : 1 : 2 : 1 : 1 : 1 : 1} {28, 2}
Organism Y 24 (9): 20hp, 391 left, DNA: {1 : 0 : 1 : 2 : 1 : 0 : 2 : 0} {18, 43}
Organism Z 25 (9): 10hp, 391 left, DNA: {0 : 1 : 1 : 2 : 3 : 2 : 2 : 2} {20, 3}
```
