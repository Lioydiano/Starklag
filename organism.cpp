#include "organism.hpp"


Organism::Organism(char symbol_, sista::Coordinates coordinates_, ANSI::Settings settings_, DNA* dna_, Statistics stats_):
    sista::Pawn(symbol_, coordinates_, settings_), dna(dna_), stats(stats_) {
    health = dna->genes.at(Gene::STRENGTH)->value*10;
    left = dna->genes.at(Gene::LIFESPAN)->value;
    stats.age = 0;
}
Organism::Organism(char symbol_, sista::Coordinates coordinates_, ANSI::Settings& settings_, DNA* dna_, Statistics stats_, bool by_reference_):
    sista::Pawn(symbol_, coordinates_, settings_, by_reference_), dna(dna_), stats(stats_) {
    health = dna->genes.at(Gene::STRENGTH)->value*10;
    left = dna->genes.at(Gene::LIFESPAN)->value;
    stats.age = 0;
}
Organism::~Organism() {
    delete dna;
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