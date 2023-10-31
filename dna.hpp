#pragma once
#include <vector>
#include <unordered_map>
#include <random>


enum Gene {
    SPEED, STRENGTH, FERTILITY,
    NATURE, LIFESPAN, ATTACK,
    DEFENSE, VISION
};


class Allele {
public:
    Gene name;
    int value;

    Allele(Gene, int);
    Allele(Gene);

    void rational_mutate();
    void random_mutate();

    Allele* combine(const Allele*) const;

    bool operator==(const Allele&) const;
    bool operator!=(const Allele&) const;
};


class DNA {
public:
    std::vector<Allele*> alleles;
    std::unordered_map<Gene, Allele*> genes;

    DNA(std::vector<Allele*>);
    DNA(std::unordered_map<Gene, Allele*>);
    DNA(std::vector<Gene>);
    DNA(std::unordered_map<Gene, int>);
    DNA();
    ~DNA();

    void rational_mutate();
    void random_mutate();

    DNA* combine(const DNA*) const;

    void print();
    void printInline();
    void printInline(std::ofstream&);

    bool operator==(const DNA&) const;
    bool operator!=(const DNA&) const;
};


// Need to be defined in a .cpp file but otherwise it will throw a linker error
extern std::random_device random_device;
extern std::mt19937 random_engine;
extern std::bernoulli_distribution MUTATION_RATE;
extern std::uniform_int_distribution<int> MUTATION_AMOUNT;

enum Nature {AGGRESSIVE=1, PASSIVE=2, NEUTRAL=3}; // Neutral won't attack and won't defend
enum Speed {SLOW=1, MEDIUM=2, FAST=3};
enum Strength {WEAK=1, MODERATE=2, STRONG=3};
enum Fertility {LOW=1, MEDIAN=3, HIGH=5};
enum LifeSpan {SHORT_=1, AVERAGE=2, LONG_=3}; // If more than LONG, then it's potentially immortal
enum Vision {NEAR=1, CLEAR=3, FAR=5};

extern std::unordered_map<Gene, int> default_allele_value;
extern std::unordered_map<Gene, std::vector<int>> possible_random_allele_values;
extern std::vector<Gene> genes;