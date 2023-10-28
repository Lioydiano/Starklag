#include "dna.hpp"
#include <random>


std::random_device random_device;
std::mt19937 random_engine(random_device());
std::bernoulli_distribution MUTATION_RATE = std::bernoulli_distribution(0.05);
std::uniform_int_distribution<int> MUTATION_AMOUNT(-1, 1);
enum Nature {AGGRESSIVE=1, PASSIVE=2, NEUTRAL=3}; // Neutral won't attack and won't defend
enum Speed {SLOW=1, MEDIUM=2, FAST=3};
enum Strength {WEAK=1, MODERATE=2, STRONG=3};
enum Fertility {LOW=1, MEDIAN=3, HIGH=5};
enum LifeSpan {SHORT=50, AVERAGE=250, LONG=1250}; // If more than LONG, then it's potentially immortal
enum Vision {NEAR=1, CLEAR=3, FAR=5};

std::unordered_map<Gene, int> default_allele_value = {
    {SPEED, 1}, {STRENGTH, 1}, {FERTILITY, 1},
    {NATURE, Nature::PASSIVE}, {LIFESPAN, 1},
    {ATTACK, 1}, {DEFENSE, 1}, {VISION, 1}
};
std::unordered_map<Gene, std::vector<int>> possible_random_allele_values = {
    {SPEED, {Speed::SLOW, Speed::MEDIUM, Speed::FAST}},
    {STRENGTH, {Strength::WEAK, Strength::MODERATE, Strength::STRONG}},
    {FERTILITY, {Fertility::LOW, Fertility::MEDIAN, Fertility::HIGH}},
    {NATURE, {Nature::AGGRESSIVE, Nature::PASSIVE, Nature::NEUTRAL}},
    {LIFESPAN, {LifeSpan::SHORT, LifeSpan::AVERAGE, LifeSpan::LONG}},
    {ATTACK, {1, 2, 3}}, {DEFENSE, {1, 2, 3}},
    {VISION, {Vision::NEAR, Vision::CLEAR, Vision::FAR}}
};


Allele::Allele(Gene name_, int value_): name(name_), value(value_) {}
Allele::Allele(Gene name_): name(name_), value(default_allele_value.at(name_)) {}

void Allele::rational_mutate() {
    value += name == Gene::LIFESPAN ? MUTATION_AMOUNT(random_engine)*50 : MUTATION_AMOUNT(random_engine);
    value = std::abs(value);
}
void Allele::random_mutate() {
    std::vector<int>* possible_alleles = &possible_random_allele_values.at(name);
    value = possible_alleles->at(std::rand() % possible_alleles->size());
}

Allele* Allele::combine(const Allele* other) const {
    Allele* new_allele = new Allele(name);
    new_allele->value = rand() % 2 ? value : other->value;
    return new_allele;
}

bool Allele::operator==(const Allele& other) const {
    return name == other.name && value == other.value;
}
bool Allele::operator!=(const Allele& other) const {
    return !(*this == other);
}


DNA::DNA(std::vector<Allele*> alleles_): alleles(alleles_) {
    for (auto allele : alleles) {
        genes[allele->name] = allele;
    }
}
DNA::DNA(std::unordered_map<Gene, Allele*> genes_): genes(genes_) {
    for (std::pair<Gene, Allele*> gene : genes) {
        alleles.push_back(gene.second);
    }
}
DNA::DNA(std::vector<Gene> genes_) {
    for (Gene gene : genes_) {
        Allele* allele = new Allele(gene);
        alleles.push_back(allele);
        genes[gene] = allele;
    }
}
DNA::DNA(std::unordered_map<Gene, int> genes_) {
    for (std::pair<Gene, int> gene : genes_) {
        Allele* allele = new Allele(gene.first, gene.second);
        alleles.push_back(allele);
        genes[gene.first] = allele;
    }
}
DNA::DNA() {
    for (std::pair<Gene, int> gene : default_allele_value) {
        Allele* allele = new Allele(gene.first, gene.second);
        alleles.push_back(allele);
        genes[gene.first] = allele;
    }
}
DNA::~DNA() {
    for (Allele* allele : alleles) {
        delete allele;
    }
}

void DNA::rational_mutate() {
    for (Allele* allele : alleles) {
        if (MUTATION_RATE(random_engine)) {
            allele->rational_mutate();
        }
    }
}
void DNA::random_mutate() {
    for (Allele* allele : alleles) {
        if (MUTATION_RATE(random_engine)) {
            allele->random_mutate();
        }
    }
}

DNA* DNA::combine(const DNA* other) const {
    std::vector<Allele*> new_alleles;
    for (Allele* allele : alleles) {
        new_alleles.push_back(
            allele->combine(
                other->genes.at(allele->name)
            )
        );
    }
    return new DNA(new_alleles);
}

bool DNA::operator==(const DNA& other) const {
    for (Allele* allele : alleles) {
        if (*allele != *other.genes.at(allele->name)) {
            return false;
        }
    }
    return true;
}
bool DNA::operator!=(const DNA& other) const {
    return !(*this == other);
}