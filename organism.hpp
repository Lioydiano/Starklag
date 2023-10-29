#pragma once
#include "include/sista/sista.hpp"
#include "dna.hpp"


struct Statistics {
    int age;
    int generation;
    Organism* parents[2];
    std::vector<Organism*> children;
};


class Organism: public sista::Pawn {
    static inline unsigned id_counter = 0;

public:
    unsigned id;
    Statistics stats;
    DNA* dna; // DNA of the organism
    int health; // Health of the organism [starting value deduced from DNA, strength]
    int left; // Time left to live [starting value deduced from DNA, lifespan]

    Organism(char, sista::Coordinates, ANSI::Settings, DNA*, Statistics);
    Organism(char, sista::Coordinates, ANSI::Settings&, DNA*, Statistics, bool);
    ~Organism();

    void meet(Organism*); // Can lead to breeding, attacking, defending, or nothing
    void breed(Organism*); // Only a certain probability of conception, based on delta age
    void attack(Organism*);
    void defend(Organism*);

    bool breedable(const Organism*) const; // Check if the two organisms can breed [depends on DNA]
}; // Doesn't inherit from sista::Pawn, which will be put in attributes


// Need to be defined in a .cpp file but otherwise it will throw a linker error
extern std::bernoulli_distribution breeding_probability;