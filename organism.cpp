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