#pragma once
#include <vector>
#include <unordered_map>


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

    bool operator==(const DNA&) const;
    bool operator!=(const DNA&) const;
};