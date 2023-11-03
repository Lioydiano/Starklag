#include "organism.hpp"
#include "dna.cpp"
#include <random>
#include <chrono>
#define DEBUG 1


std::bernoulli_distribution breeding_probability(0.1);
std::bernoulli_distribution attack_probability(0.3);
sista::Field* field = nullptr;
#if DEBUG
    std::ofstream debug("debug.txt");
#endif

struct Range {
    int start;
    int stop;
    int step;
};


Entity::Entity(char symbol_, sista::Coordinates coordinates_, ANSI::Settings settings_):
    sista::Pawn(symbol_, coordinates_, settings_) {
    is_food = false;
}
Entity::Entity(char symbol_, sista::Coordinates coordinates_, ANSI::Settings& settings_, bool by_reference_):
    sista::Pawn(symbol_, coordinates_, settings_, by_reference_) {
    is_food = false;
}
Entity::~Entity() {}


Food::Food(sista::Coordinates coordinates_):
    Entity('@', coordinates_, ANSI::Settings(ANSI::ForegroundColor::F_GREEN, ANSI::BackgroundColor::B_BLACK, ANSI::Attribute::BRIGHT)) {
    foods.push_back(this);
    is_food = true;
    energy = random_engine() % 10 + 1;
}
Food::~Food() {}


Organism::Organism(char symbol_, sista::Coordinates coordinates_, ANSI::Settings settings_, DNA* dna_, Statistics stats_):
    Entity(symbol_, coordinates_, settings_), dna(dna_), stats(stats_) {
    Organism::organisms.push_back(this);
    health = dna->genes.at(Gene::STRENGTH)->value*10;
    left = std::pow(dna->genes.at(Gene::LIFESPAN)->value, 2)*100;
    has_given_birth = false;
    this->id = id_counter++;
    stats.age = 0;
}
Organism::Organism(char symbol_, sista::Coordinates coordinates_, ANSI::Settings& settings_, DNA* dna_, Statistics stats_, bool by_reference_):
    Entity(symbol_, coordinates_, settings_, by_reference_), dna(dna_), stats(stats_) {
    Organism::organisms.push_back(this);
    health = dna->genes.at(Gene::STRENGTH)->value*10;
    left = dna->genes.at(Gene::LIFESPAN)->value;
    has_given_birth = false;
    this->id = id_counter++;
    stats.age = 0;
}
Organism::~Organism() {
    delete dna;
}

void Organism::move() {
    std::bernoulli_distribution moving_probability(0.2*dna->genes.at(Gene::SPEED)->value);
    if (moving_probability(random_engine)) {
        sista::Coordinates new_coordinates = coordinates;
        do {
            new_coordinates = coordinates;
            switch (random_engine() % 4) {
                case 0: {
                    new_coordinates.x++;
                    break;
                }
                case 1: {
                    new_coordinates.x--;
                    break;
                }
                case 2: {
                    new_coordinates.y++;
                    break;
                }
                case 3: {
                    new_coordinates.y--;
                    break;
                }
            }
        } while (field->isOutOfBounds(new_coordinates) || new_coordinates == coordinates);
        if (field->isOccupied(new_coordinates)) {
            Entity* other = nullptr;
            for (Food* food : Food::foods) {
                if (food->getCoordinates() == new_coordinates) {
                    other = (Entity*)food;
                    break;
                }
            }
            for (Organism* organism : Organism::organisms) {
                if (organism->getCoordinates() == new_coordinates) {
                    other = (Entity*)organism;
                    break;
                }
            }
            if (other == nullptr) {
                return;
            }
            this->meet(other);
        } else {
            field->movePawn(this, new_coordinates);
            coordinates = new_coordinates;
        }
    }
}

void Organism::meet(Entity* other) {
    if (other == nullptr) {
        return;
    }
    if (other->is_food) {
        this->eat((Food*)other);
    } else {
        this->meet((Organism*)other);
    }
}

void Organism::meet(Organism* other) {
    #if DEBUG
        debug << this << " is trying to meet with " << other << std::endl;
        debug << "\t" << this << " stats: {" << this->stats.age << ", " << this->stats.generation << ", ";
        debug << "{" << this->stats.parents[0] << ", " << this->stats.parents[1] << "}}";
        debug << " output: {'" << this->symbol << "', {" << this->getCoordinates().y << ", " << this->getCoordinates().x << "}}\n"; 
        debug << "\t" << other << " stats: {" << other->stats.age << ", " << other->stats.generation << ", ";
        debug << "{" << other->stats.parents[0] << ", " << other->stats.parents[1] << "}}";
        debug << " output: {'" << other->symbol << "', {" << other->getCoordinates().y << ", " << other->getCoordinates().x << "}}\n";
        debug << std::flush;
    #endif

    if (attack_probability(random_engine)) {
        this->attack(other);
    } else if (breeding_probability(random_engine)) {
        this->breed(other);
    }
}

void Organism::breed(Organism* other) {
    if (stats.age < 10 || other->stats.age < 10)
        return;
    // The youngest organism is more likely to breed
    if (stats.age > other->stats.age && (random_engine() % 3 != 0)) {
        debug << this << " is younger than " << other << std::endl;
        return other->breed(this);
    }
    if (!breedable(other))
        return;
    if (this->has_given_birth || other->has_given_birth)
        return; // Can't give birth more than once for each frame
    this->has_given_birth = true;
    #if DEBUG
        debug << "BREED!";
        debug << this << " with " << other << std::endl;
        debug << "\t" << "'" << this->symbol << "' ";
        this->dna->printInline(debug);
        debug << std::endl;
        debug << "\t" << "'" << other->symbol << "' ";
        other->dna->printInline(debug);
        debug << std::endl;
    #endif

    std::vector<Organism*> children;
    for (int i = 0; i < dna->genes.at(Gene::FERTILITY)->value; i++) {
        // Combine the DNA of the two organisms
        DNA* new_dna = dna->combine(other->dna);
        // Create a new organism with the new DNA
        Organism* child = new Organism(
            symbol, coordinates, settings, new_dna, // Coordinates will be changed later
            {0, stats.generation + 1, {this, other}, {}}
        );
        // Add the child to the parents' children
        stats.children.push_back(child);
        other->stats.children.push_back(child);
        children.push_back(child);
        #if DEBUG
            debug << "\t" << child << " is born with ";
            child->dna->printInline(debug);
            debug << std::endl;
        #endif
    }
    // Now we have to place the children in the field
    for (Organism* child : children) {
        sista::Coordinates new_coordinates = coordinates;
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

        int random = rand() % 4;
        for (int i = -10; i < 10; i++) {
            for (int j = -10; j < 10; j++) {
                if (i == 0 && j == 0) {
                    continue;
                }
                if (random == 0) {
                    i = -i;
                } else if (random == 1) {
                    j = -j;
                } else if (random == 2) {
                    i = -i;
                    j = -j;
                }
                new_coordinates.y = coordinates.y + i;
                new_coordinates.x = coordinates.x + j;
                if (field->isOutOfBounds(new_coordinates)) {
                    continue;
                }
                if (field->isFree(new_coordinates)) {
                    #if DEBUG
                        debug << "\t" << child << " is placed at delta {" << i << ", " << j << "}" << std::endl;
                    #endif
                    goto found;
                }
                std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
                if (std::chrono::duration_cast<std::chrono::duration<double>>(end - begin).count() > 0.1) {
                    goto not_found;
                }
            }
        }
        goto not_found;
        found:
        child->coordinates = new_coordinates;
        field->addPawn((sista::Pawn*)child);
        continue;
        not_found:
        #if DEBUG
            debug << "\t" << child << " couldn't find a place to be born" << std::endl;
        #endif
        organisms.erase(std::find(organisms.begin(), organisms.end(), child));
        children.erase(std::find(children.begin(), children.end(), child));
        delete child;
        return; // There's no space for other children
    }
}

void Organism::attack(Organism* other) {
    // First we get the nature of the organisms
    Nature this_nature = (Nature)dna->genes.at(Gene::NATURE)->value;
    Nature other_nature = (Nature)other->dna->genes.at(Gene::NATURE)->value;
    // Then we check if the attack is possible and if it is, we do it
    if (this_nature == Nature::NEUTRAL) { // Won't attack
        return;
    } else if (this_nature == Nature::PASSIVE) { // May have provoked the other
        if (other_nature == Nature::AGGRESSIVE) {
            other->attack(this);
            return;
        }
    } else if (this_nature == Nature::AGGRESSIVE) { // Will attack
        #if DEBUG
            debug << "ATTACK!";
            debug << this << " attacks " << other << ", whom nature is " << other_nature << std::endl;
        #endif
        int this_attack = dna->genes.at(Gene::ATTACK)->value;
        int this_defense = dna->genes.at(Gene::DEFENSE)->value;
        int other_defense = other->dna->genes.at(Gene::DEFENSE)->value;
        int other_attack = other->dna->genes.at(Gene::ATTACK)->value;

        if (other_nature == Nature::PASSIVE) { // Will only defend
            int damage_done = std::max(this_attack - other_defense, 0);
            int damage_taken = std::max(other_defense, 0);

            health -= damage_taken;
            other->health -= damage_done;
        } else if (other_nature == Nature::AGGRESSIVE) { // Will both attack and defend
            int damage_done = std::max(this_attack - other_defense, 0);
            int damage_taken = std::max(other_attack - this_defense, 0);
            damage_done += std::max(other_defense, 0);
            damage_taken += std::max(this_defense, 0);

            while (health > 0 && other->health > 0) {
                health -= damage_taken;
                other->health -= damage_done;
            } // At least one of the two will be dead, they're fighting to the death
        } else if (other_nature == Nature::NEUTRAL) { // Won't fight back
            int damage_done = std::max(this_attack, 0);

            other->health -= damage_done;
        } else if (other_nature > Nature::NEUTRAL) { // So peaceful that it won't take nor give damage
            return;
        }
    } else if (this_nature < (int)Nature::AGGRESSIVE) { // Will attack only if the other is aggressive or over...
        // ...then the most aggressive organism will win without taking any damage
        if (other_nature <= Nature::AGGRESSIVE) {
            if (this_nature > other_nature) {
                other->health = 0;
            } else {
                health = 0;
            }
        }
    }
    // Check if someone died
    if (health <= 0) {
        #if DEBUG
            debug << "\t" << this << " was defeated" << std::endl;
        #endif
        dead_organisms.push_back(this);
    }
    if (other->health <= 0) {
        #if DEBUG
            debug << "\t" << other << " was defeated" << std::endl;
        #endif
        dead_organisms.push_back(other);
        if (other_nature != Nature::AGGRESSIVE) {
            // Eat the other organism
            #if DEBUG
                debug << "\t" << this << " gains " << other->dna->genes.at(Gene::STRENGTH)->value*5 << " health points eating " << other << std::endl;
            #endif
            health += other->dna->genes.at(Gene::STRENGTH)->value*5;
        }
    }
}

void Organism::eat(Food* food) {
    if (food == nullptr) {
        return;
    }
    #if DEBUG
        debug << "EAT! ";
        debug << this << " eats " << food << std::endl;
    #endif
    health += food->energy;
    health = std::min(dna->genes.at(Gene::STRENGTH)->value*10, health);
    field->removePawn(food);
    Food::foods.erase(std::find(Food::foods.begin(), Food::foods.end(), food));
    delete food;
}


bool Organism::breedable(const Organism* other) const {
    if (other == nullptr) {
        #if DEBUG
            debug << "\t" << this << " can't breed with " << other << " because it's nullptr" << std::endl;
        #endif
        return false;
    }
    if (this->id == other->id) {
        return false;
    }
    if (this->symbol == other->symbol) {
        if (this->stats.parents[0] == other || this->stats.parents[1] == other)
            return false; // Can't breed with its parent
        if (other->stats.parents[0] == this || other->stats.parents[1] == this)
            return false; // Can't breed with its child
        if (this->stats.parents[0] == other->stats.parents[0] || this->stats.parents[0] == other->stats.parents[1])
            return false; // Can't breed with its sibling
        if (this->stats.parents[1] == other->stats.parents[0] || this->stats.parents[1] == other->stats.parents[1])
            return false; // Can't breed with its sibling
        if (random_device() % 2 && random_device() % 2)
            return false; // Reduce to 25% the probability of breeding with an organism of the same strain (or, at least, with the same external appearance)
    }

    DNA* other_dna = other->dna;
    #if DEBUG
        debug << "\t" << this << " is trying to breed with " << other << std::endl;
        debug << "\t\t";
        this->dna->printInline(debug);
        debug << "\n\t\t";
        other_dna->printInline(debug);
        debug << std::endl;
    #endif
    int too_different_alleles = 0;
    for (Allele* allele : dna->alleles) {
        int other_value = other_dna->genes.at(allele->name)->value;
        if (!std::max(allele->value, other_value)) {
            continue;
        }
        int distance_coefficient = std::abs(allele->value - other_value) / std::max(allele->value, other_value);
        if (distance_coefficient > 0.5) {
            too_different_alleles++;
        }
    }
    if (too_different_alleles >= 3) {
        #if DEBUG
            debug << "\t" << this << " can't breed with " << other << " because they have " << too_different_alleles << " too different alleles" << std::endl;
        #endif
    } else {
        #if DEBUG
            debug << "\t" << this << " can breed with " << other << " because they have only " << too_different_alleles << " too different alleles" << std::endl;
        #endif
    }
    return too_different_alleles < 3;
}