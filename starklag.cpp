#include "organism.cpp"


bool isDead(Organism* organism) {
    for (std::vector<Organism*>::iterator it = Organism::dead_organisms.begin(); it != Organism::dead_organisms.end(); it++) {
        if (*it == organism) {
            return true;
        }
    }
    return false;
}


int main() {
    sista::Border border(
        '#', ANSI::Settings(
            ANSI::ForegroundColor::F_WHITE,
            ANSI::BackgroundColor::B_BLACK,
            ANSI::Attribute::BRIGHT
        )
    );
    sista::Field field_(50, 30);
    field_.reset();
    field_.clear();
    field = &field_;

    // Create the organisms
    for (int i = 0; i < 15; i++) {
        Organism* organism;
        std::cout << "Debug 0" << std::endl;
        char symbol = 'A' + random_engine() % 26;
        sista::Coordinates coordinates(
            (unsigned)(random_engine() % 30),
            (unsigned)(random_engine() % 50)
        );
        std::cout << "x: " << coordinates.x << ", y: " << coordinates.y << std::endl;
        std::cout << "Debug 1" << std::endl;
        ANSI::Settings settings(
            (ANSI::ForegroundColor)(random_engine() % 8 + 30),
            (ANSI::BackgroundColor)(random_engine() % 8 + 40),
            ANSI::Attribute::UNDERSCORE
        );
        std::cout << "Debug 2" << std::endl;
        DNA* dna = new DNA();
        for (int j = 0; j < (int)(genes.size()); j++) {
            for (int k = 0; k < (int)(random_engine() % 3); k++) {
                dna->alleles[j]->rational_mutate();
            }
        }
        std::cout << "Debug 3" << std::endl;
        Statistics stats{0, 0, {nullptr, nullptr}, {}};
        organism = new Organism(symbol, coordinates, settings, dna, stats);
        std::cout << "Debug 4" << std::endl;
        sista::Pawn* pawn_ = (sista::Pawn*)(Entity*)organism;
        std::cout << "Debug 5" << std::endl;
        field_.addPawn(pawn_);
        std::cout << "Debug 6" << std::endl;
    }

    // Create the food
    for (int i = 0; i < 40; i++) {
        Food* food;
        sista::Coordinates coordinates(
            (unsigned)(random_engine() % 30),
            (unsigned)(random_engine() % 50)
        );
        food = new Food(coordinates);
        field_.addPawn(food);
    }

    // Start the simulation
    sista::clearScreen();
    field->print(border);

    while (true) {
        for (int i = 0; i < 10; i++) {
            // All the organisms may move
            for (Organism* organism : Organism::organisms) {
                if (organism == nullptr) {
                    continue;
                }
                if (isDead(organism)) {
                    sista::Coordinates coordinates = organism->getCoordinates();
                    if (field->getPawn(coordinates) == organism)
                        field->removePawn(coordinates);
                    continue;
                }
                if (organism->left <= 0 || organism->health <= 0) {
                    Organism::dead_organisms.push_back(organism);
                    continue;
                }
                organism->stats.age++;
                organism->left--;
                if (organism->health < organism->dna->genes.at(Gene::STRENGTH)->value*5)
                    organism->left--; // If the organism is weak, it will die faster
                organism->move();
            }
            // All the organisms may meet
            for (Organism* organism : Organism::organisms) {
                if (organism == nullptr) {
                    continue;
                }
                if (isDead(organism)) {
                    continue;
                }
                sista::Coordinates neighbor_coordinates[4] = {
                    {organism->getCoordinates().x, organism->getCoordinates().y-1},
                    {organism->getCoordinates().x, organism->getCoordinates().y+1},
                    {organism->getCoordinates().x-1, organism->getCoordinates().y},
                    {organism->getCoordinates().x+1, organism->getCoordinates().y}
                };
                for (sista::Coordinates coordinates : neighbor_coordinates) {
                    if (field->isOutOfBounds(coordinates)) {
                        continue;
                    }
                    sista::Pawn* pawn = field->getPawn(coordinates);
                    if (pawn == nullptr) {
                        continue;
                    }
                    if (pawn == organism) {
                        continue;
                    }
                    organism->meet((Entity*)pawn);
                }
            }
            // All organisms may mutate
            for (Organism* organism : Organism::organisms) {
                if (organism == nullptr) {
                    continue;
                }
                if (isDead(organism)) {
                    continue;
                }
                // if (MUTATION_RATE(random_engine))
                //     organism->dna->rational_mutate();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        // ANSI::reset();
        // sista::clearScreen();
        // ANSI::reset();
        // field->print(border);
    }
    for (Organism* organism : Organism::organisms) {
        delete organism;
    }
    for (Food* food : Food::foods) {
        delete food;
    }
    for (Organism* organism : Organism::dead_organisms) {
        delete organism;
    }
    std::cin.get();
}