#pragma once
#include "include/sista/sista.hpp"
#include "dna.hpp"


class Food;
class Organism;
struct Statistics {
    int age;
    int generation;
    Organism* parents[2];
    std::vector<Organism*> children;
};


class Entity: public sista::Pawn {
public:
    bool is_food;
    Entity(char, sista::Coordinates, ANSI::Settings);
    Entity(char, sista::Coordinates, ANSI::Settings&, bool);
    ~Entity();
};


class Food: public Entity {
public:
    int energy; // Energy of the food when eaten
    Food(char, sista::Coordinates, ANSI::Settings);
    Food(char, sista::Coordinates, ANSI::Settings&, bool);
    ~Food();
};


class Organism: public Entity {
    static inline unsigned id_counter = 0;
    static std::vector<Organism*> organisms;
    static std::vector<Organism*> dead_organisms;

public:
    unsigned id;
    Statistics stats;
    DNA* dna; // DNA of the organism
    int health; // Health of the organism [starting value deduced from DNA, strength]
    int left; // Time left to live [starting value deduced from DNA, lifespan]

    Organism(char, sista::Coordinates, ANSI::Settings, DNA*, Statistics);
    Organism(char, sista::Coordinates, ANSI::Settings&, DNA*, Statistics, bool);
    ~Organism();

    void meet(Entity*); // Can lead to eating or to meet(Organism*)
    void meet(Organism*); // Can lead to breeding, attacking, defending, or nothing
    std::vector<Organism*> breed(Organism*); // Returns the children, they have to be placed in the sista::Field
    void attack(Organism*);
    void eat(Food*);

    bool breedable(const Organism*) const; // Check if the two organisms can breed [depends on DNA]
}; // Doesn't inherit from sista::Pawn, which will be put in attributes


// Need to be defined in a .cpp file but otherwise it will throw a linker error
extern std::bernoulli_distribution breeding_probability;
extern std::bernoulli_distribution attack_probability;
extern sista::Field* field;