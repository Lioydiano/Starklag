#include "organism.hpp"
#include <random>


std::bernoulli_distribution breeding_probability(0.1);


Organism::Organism(char symbol_, sista::Coordinates coordinates_, ANSI::Settings settings_, DNA* dna_, Statistics stats_):
    sista::Pawn(symbol_, coordinates_, settings_), dna(dna_), stats(stats_) {
    health = dna->genes.at(Gene::STRENGTH)->value*10;
    left = dna->genes.at(Gene::LIFESPAN)->value;
    this->id = id_counter++;
    stats.age = 0;
}
Organism::Organism(char symbol_, sista::Coordinates coordinates_, ANSI::Settings& settings_, DNA* dna_, Statistics stats_, bool by_reference_):
    sista::Pawn(symbol_, coordinates_, settings_, by_reference_), dna(dna_), stats(stats_) {
    health = dna->genes.at(Gene::STRENGTH)->value*10;
    left = dna->genes.at(Gene::LIFESPAN)->value;
    this->id = id_counter++;
    stats.age = 0;
}
Organism::~Organism() {
    delete dna;
}

std::vector<Organism*> Organism::breed(Organism* other) {
    // Only the youngest organism can breed
    if (stats.age > other->stats.age) {
        other->breed(this);
    }
    std::vector<Organism*> children;
    for (int i = 0; i < dna->genes.at(Gene::FERTILITY)->value; i++) {
        // Combine the DNA of the two organisms
        DNA* new_dna = dna->combine(other->dna);
        // Create a new organism with the new DNA
        Organism* child = new Organism(
            symbol, coordinates, settings, new_dna,
            {0, stats.generation + 1, {this, other}, {}}
        );
        // Add the child to the parents' children
        stats.children.push_back(child);
        other->stats.children.push_back(child);
        children.push_back(child);
    }
    return children;
}

bool Organism::breedable(const Organism* other) const {
    DNA* other_dna = other->dna;
    int too_different_alleles = 0;
    for (Allele* allele : dna->alleles) {
        int distance_coefficient = std::abs(
            allele->value - other_dna->genes.at(allele->name)->value
        ) / std::max(allele->value, other_dna->genes.at(allele->name)->value);
        if (distance_coefficient > 0.5) {
            too_different_alleles++;
        }
    }
    return too_different_alleles < dna->alleles.size() / 2;
}