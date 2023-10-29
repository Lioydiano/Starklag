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
    static inline std::vector<Food*> foods;
    int energy; // Energy of the food when eaten
    Food(sista::Coordinates);
    ~Food();
};


class Organism: public Entity {
    static inline unsigned id_counter = 0;

public:
    static inline std::vector<Organism*> organisms;
    static inline std::vector<Organism*> dead_organisms;

    unsigned id;
    int health; // Health of the organism [starting value deduced from DNA, strength]
    int left; // Time left to live [starting value deduced from DNA, lifespan]
    DNA* dna; // DNA of the organism
    Statistics stats;

    Organism(char, sista::Coordinates, ANSI::Settings, DNA*, Statistics);
    Organism(char, sista::Coordinates, ANSI::Settings&, DNA*, Statistics, bool);
    ~Organism();

    void move();
    void meet(Entity*); // Can lead to eating or to meet(Organism*)
    void meet(Organism*); // Can lead to breeding, attacking, defending, or nothing
    void breed(Organism*); // Places the children in the sista::Field
    void attack(Organism*);
    void eat(Food*);

    bool breedable(const Organism*) const; // Check if the two organisms can breed [depends on DNA]
}; // Doesn't inherit from sista::Pawn, which will be put in attributes


// Need to be defined in a .cpp file but otherwise it will throw a linker error
extern std::bernoulli_distribution breeding_probability;
extern std::bernoulli_distribution attack_probability;
extern sista::Field* field;