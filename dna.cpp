#include "dna.hpp"
#include <iostream>
#include <fstream>


std::random_device random_device;
std::mt19937 random_engine(random_device());
std::bernoulli_distribution MUTATION_RATE(0.05);
std::uniform_int_distribution<int> MUTATION_AMOUNT(-1, 1);
std::unordered_map<Gene, int> default_allele_value = {
    {SPEED, Speed::SLOW}, {STRENGTH, Strength::WEAK},
    {FERTILITY, Fertility::LOW}, {NATURE, Nature::PASSIVE},
    {LIFESPAN, LifeSpan::AVERAGE}, {ATTACK, 1}, {DEFENSE, 1},
    {VISION, Vision::NEAR}, {BREATH, Breath::ANAEROBIC}
};
std::unordered_map<Gene, std::vector<int>> possible_random_allele_values = {
    {SPEED, {Speed::SLOW, Speed::MEDIUM, Speed::FAST}},
    {STRENGTH, {Strength::WEAK, Strength::MODERATE, Strength::STRONG}},
    {FERTILITY, {Fertility::LOW, Fertility::MEDIAN, Fertility::HIGH}},
    {NATURE, {Nature::AGGRESSIVE, Nature::PASSIVE, Nature::NEUTRAL}},
    {LIFESPAN, {LifeSpan::SHORT_, LifeSpan::AVERAGE, LifeSpan::LONG_}},
    {ATTACK, {1, 2, 3}}, {DEFENSE, {1, 2, 3}},
    {VISION, {Vision::NEAR, Vision::CLEAR, Vision::FAR}},
    {BREATH, {Breath::PHOTOAUTOTROPH, Breath::ANAEROBIC, Breath::AEROBIC}}
};
std::vector<Gene> genes = {
    SPEED, STRENGTH, FERTILITY, NATURE,
    LIFESPAN, ATTACK, DEFENSE, VISION, BREATH
};
std::unordered_map<Gene, std::string> gene_to_string = {
    {Gene::SPEED, "speed"}, {Gene::STRENGTH, "strength"},
    {Gene::FERTILITY, "fertility"}, {Gene::NATURE, "nature"},
    {Gene::LIFESPAN, "lifespan"}, {Gene::ATTACK, "attack"},
    {Gene::DEFENSE, "defense"}, {Gene::VISION, "vision"},
    {Gene::BREATH, "breath"}
};
std::unordered_map<Gene, std::unordered_map<int, std::string>> allele_to_string = {
    {Gene::SPEED, {{Speed::SLOW, "slow"}, {Speed::MEDIUM, "medium"}, {Speed::FAST, "fast"}}},
    {Gene::STRENGTH, {{Strength::WEAK, "weak"}, {Strength::MODERATE, "moderate"}, {Strength::STRONG, "strong"}}},
    {Gene::FERTILITY, {{Fertility::LOW, "low"}, {Fertility::MEDIAN, "median"}, {Fertility::HIGH, "high"}}},
    {Gene::NATURE, {{Nature::AGGRESSIVE, "aggressive"}, {Nature::PASSIVE, "passive"}, {Nature::NEUTRAL, "neutral"}}},
    {Gene::LIFESPAN, {{LifeSpan::SHORT_, "short"}, {LifeSpan::AVERAGE, "average"}, {LifeSpan::LONG_, "long"}}},
    {Gene::ATTACK, {{1, "weak"}, {2, "moderate"}, {3, "strong"}}},
    {Gene::DEFENSE, {{1, "weak"}, {2, "moderate"}, {3, "strong"}}},
    {Gene::VISION, {{Vision::NEAR, "near"}, {Vision::CLEAR, "clear"}, {Vision::FAR, "far"}}},
    {Gene::BREATH, {{Breath::PHOTOAUTOTROPH, "photoautotroph"}, {Breath::ANAEROBIC, "anaerobic"}, {Breath::AEROBIC, "aerobic"}}}
};


Allele::Allele(Gene name_, int value_): name(name_), value(value_) {}
Allele::Allele(Gene name_): name(name_), value(default_allele_value.at(name_)) {}

void Allele::rational_mutate() {
    value += MUTATION_AMOUNT(random_engine);
    if (name != Gene::BREATH)
        value = std::abs(value);
    if (name == Gene::LIFESPAN)
        value = std::max(value, (int)LifeSpan::SHORT_);
    if (name == Gene::STRENGTH)
        value = std::max(value, (int)Strength::WEAK);
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

void DNA::print() {
    std::cout << "DNA: " << std::endl;
    for (Allele* allele : this->alleles) {
        std::cout << "\t" << allele->name << ": " << allele->value << std::endl;
    }
}
void DNA::printInline() {
    std::cout << "{";
    for (std::vector<Allele*>::iterator allele = this->alleles.begin(); allele != this->alleles.end(); allele++) {
        std::cout << (*allele)->value;
        if (allele != this->alleles.end() - 1)
            std::cout << " : ";
    }
    std::cout << "}" << std::flush;
}
void DNA::printInline(std::ofstream& fout) {
    fout << "{";
    for (Allele* allele : this->alleles) {
        fout << allele->value << ":";
    }
    fout << "}" << std::flush;
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
DNA::DNA(bool void_flag) {}
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