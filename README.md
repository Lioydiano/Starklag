# Starklag

Starklag, which is a contraction of *"den starkastes lag"* which in Swedish means *"the law of the strongest"*, is a partial evolution simulator with real time view on the field, logging, statistics and graphical representation of the population and of the expressions of the genes.

![image](https://github.com/user-attachments/assets/21c07f0f-0fd1-447d-a046-18a23c1c38b6)

The simulator is written in `C++17` and uses the [`Sista`](https://github.com/FLAK-ZOSO/Sista) library for the real time field view.

![image](https://github.com/user-attachments/assets/006b26a0-e3bd-4961-a529-1f854807ffb4)

The statistics are dumped in `.txt` files, they can be plotted using the `Python` script in the `demo` folder, which uses `matplotlib`.

![fertility-alleles-stats](https://github.com/user-attachments/assets/2ebdf5cb-3557-453f-87b9-ffeb9b3b18f4)

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
- *"save`<ENTER>`"* to save the simulation to `organisms_set.sklg`

### Output

The output is a real time view of the field, with the organisms represented by ANSI-styled uppercase letters (`[A-Z]`) and the food represented by a green `@`.

It is suggested to keep a zoom that allows you to see the whole field in height, but not in width, so that you can see the side logs.

## Example

![image](https://github.com/user-attachments/assets/aa2114ff-d43e-494c-9b6d-740180eb4119)

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

### Load a simulation

You should do the same as for running a new simulation, but...

```bash
../../starklag --load
cd ../..
python3 graphics.py <insert-{i} here>
```

...in this case you add `--load`.

Be sure to have put in `demo/{i}` the `organisms_set.sklg` from which data will be loaded.

```txt
0 A 34 42 22 16 23 377 10 1 1 0 4 2 1 1 2 
1 B 37 46 7 45 23 1577 20 1 2 1 3 4 0 1 0 
2 C 32 45 11 27 23 377 30 2 3 1 0 2 1 1 2 
3 D 37 42 9 4 23 77 10 1 1 2 2 1 1 3 1 
4 E 35 46 16 4 23 77 10 1 1 1 1 1 2 0 1 
5 F 34 41 21 21 23 877 10 1 1 1 2 3 2 1 0 
6 G 31 45 20 10 23 377 10 0 1 1 3 2 0 3 1 
7 H 33 40 23 47 23 877 10 0 1 1 3 3 1 0 1 
8 I 35 44 20 47 23 77 10 2 1 0 1 1 0 1 2 
9 J 34 41 6 40 23 377 10 2 1 2 2 2 1 2 2 
10 K 37 45 7 38 23 377 10 1 1 1 2 2 1 2 1 
11 L 37 44 13 27 23 877 20 0 2 0 2 3 1 1 0 
12 M 30 45 10 41 23 877 10 2 1 2 2 3 1 1 3 
13 N 31 45 14 29 23 377 6 0 1 0 1 2 2 1 1 
14 O 37 40 9 31 23 377 10 0 1 2 2 2 0 3 2 
15 P 31 42 5 42 23 377 10 1 1 1 1 2 0 1 0 
16 Q 35 44 22 19 23 877 10 1 1 1 2 3 1 0 3 
18 S 37 46 17 42 23 377 10 1 1 2 0 2 1 0 2 
19 T 37 41 20 49 23 377 20 2 2 0 1 2 1 1 2 
20 U 31 44 19 18 23 377 20 1 2 3 2 2 2 1 1 
21 V 35 46 1 36 23 377 10 1 1 2 2 2 0 1 1 
22 W 35 47 10 0 23 877 10 1 1 1 2 3 1 1 1 
23 X 37 41 28 12 23 377 10 1 1 1 3 2 1 2 0 
24 Y 34 41 14 35 23 77 20 1 2 1 2 1 1 0 1 
25 Z 34 46 5 35 23 377 10 1 1 1 2 2 1 0 1 
26 X 37 41 17 20 2 398 30 2 3 1 3 2 1 1 0 
```

Be sure to duplicate the directory if you want to run more simulations from the same save.
