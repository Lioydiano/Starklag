#include "stats.cpp"
#include <string.h>
#include <thread>
#ifdef _WIN32
    #include <windows.h>
#endif
#ifdef _WIN32
    #include <conio.h>
#elif __APPLE__
    #include <termios.h>

    struct termios orig_termios;
    void term_echooff() {
        struct termios noecho;

        tcgetattr(0, &orig_termios);

        noecho = orig_termios;
        noecho.c_lflag &= ~ECHO;

        tcsetattr(0, TCSANOW, &noecho);
    }
#elif __linux__    
    #include <unistd.h>
    #include <termios.h>

    char getch(void) {
        char buf = 0;
        struct termios old = {0};
        fflush(stdout);
        if(tcgetattr(0, &old) < 0)
            perror("tcsetattr()");
        old.c_lflag &= ~ICANON;
        old.c_lflag &= ~ECHO;
        old.c_cc[VMIN] = 1;
        old.c_cc[VTIME] = 0;
        if(tcsetattr(0, TCSANOW, &old) < 0)
            perror("tcsetattr ICANON");
        if(read(0, &buf, 1) < 0)
            perror("read()");
        old.c_lflag |= ICANON;
        old.c_lflag |= ECHO;
        if(tcsetattr(0, TCSADRAIN, &old) < 0)
            perror("tcsetattr ~ICANON");
        // printf("%c\n", buf);
        return buf;
    }
#endif


void saveOrganisms();
void loadOrganisms();
void input(bool&, bool&);
bool isDead(Organism*);
int freeSpacesAround(Organism*);


int main(int argc, char* argv[]) {
    #ifdef __APPLE__
        term_echooff();
    #endif
    std::cout << "\x1b]2;Starklag\x07"; // Set window title
    if (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))) {
        sista::clearScreen();
        std::cout << "\x1b[38;5;255mStarklag - The real-time evolution simulator\x1b[38;5;252m\n";
        std::cout << "https://github.com/Lioydiano/Starklag#usage" << std::endl;
        return 0;
    }
    sista::Border border(
        '#', ANSI::Settings(
            ANSI::ForegroundColor::F_WHITE,
            ANSI::BackgroundColor::B_BLACK,
            ANSI::Attribute::BRIGHT
        )
    );
    sista::Field field_(50, 30);
    field_.reset();
    field = &field_;
    sista::Cursor cursor;

    if (argc == 2) {
        if (!strcmp(argv[1], "-l") || !strcmp(argv[1], "--load")) {
            loadOrganisms();
        } else {
            std::cout << "Invalid arguments. Use \x1b[38;5;245mstarklag\x1b[38;5;252m \x1b[38;5;126m-h\x1b[38;5;252m for help." << std::endl;
        }
    } else {
        // Create the organisms
        for (int i = 0; i < 26; i++) {
            Organism* organism;
            char symbol = 'A' + i;
            sista::Coordinates coordinates(
                (short unsigned)(random_engine() % 30),
                (short unsigned)(random_engine() % 50)
            );
            ANSI::ForegroundColor foreground_color = (ANSI::ForegroundColor)(random_engine() % 8 + 30);
            ANSI::BackgroundColor background_color = (ANSI::BackgroundColor)(random_engine() % 8 + 40);
            if ((int)foreground_color == (int)background_color - 10) {
                foreground_color = ANSI::ForegroundColor::F_WHITE;
                background_color = ANSI::BackgroundColor::B_BLACK;
            }
            ANSI::Settings settings(
                foreground_color,
                background_color,
                ANSI::Attribute::UNDERSCORE
            );
            DNA* dna = new DNA();
            for (int j = 0; j < (int)(genes.size()); j++) {
                for (int k = 0; k < (int)(random_engine() % 3); k++) {
                    dna->alleles[j]->rational_mutate();
                }
            }
            Statistics stats{0, 0, {nullptr, nullptr}, {}};
            organism = new Organism(symbol, coordinates, settings, dna, stats);
            sista::Pawn* pawn_ = (sista::Pawn*)(Entity*)organism;
            field_.addPawn(pawn_);
        }

        // Create the food
        for (int i = 0; i < 40; i++) {
            Food* food;
            sista::Coordinates coordinates;
            while (true) {
                coordinates = {
                    (short unsigned)(random_engine() % 30),
                    (short unsigned)(random_engine() % 50)
                };
                if (*field_.getPawnIterator(coordinates) == nullptr) {
                    break;
                }
            }
            food = new Food(coordinates);
            field_.addPawn(food);
        }
    }

    // Start the simulation
    sista::clearScreen();
    field->print(border);
    Organism::dead_organisms.clear();

    bool paused = false;
    bool quit = false;
    std::thread input_thread(input, std::ref(paused), std::ref(quit));

    for (int _ = 0; !quit; _++) {
        dumpStats(_);
        for (int i = 0; i < 10; i++) {
            while (paused) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            // All the organisms may move
            for (std::vector<Organism*>::iterator it = Organism::organisms.begin(); it != Organism::organisms.end(); it++) {
                if (*it == nullptr) {
                    continue;
                }
                Organism* organism = *it;
                if (isDead(organism)) {
                    #if DEBUG
                        debug << "Organism " << organism << " (" << organism->id << ") is [already] dead with " << organism->health << " health and " << organism->left << " left at {" << organism->getCoordinates().y << ", " << organism->getCoordinates().x << "}" << std::endl;
                    #endif
                    sista::Coordinates coordinates = organism->getCoordinates();
                    if (*field->getPawnIterator(coordinates) == organism)
                        field->removePawn(coordinates);
                    continue;
                }
                // Check of asphyxiation
                int free_spaces = freeSpacesAround(organism);
                if (!free_spaces) {
                    #if DEBUG
                        debug << "Organism " << organism << " (" << organism->id << ") is asphyxiated with " << organism->health << " health and " << organism->left << " left at {" << organism->getCoordinates().y << ", " << organism->getCoordinates().x << "}" << std::endl;
                    #endif
                    Organism::dead_organisms.push_back(organism);
                    continue;
                } else if (free_spaces == 1 && organism->stats.age > 50) {
                    organism->health -= 5;
                }
                // Check of death and aging
                if (organism->left <= 0 || organism->health <= 0) {
                    #if DEBUG
                        debug << "Organism " << organism << " (" << organism->id << ") is dead with " << organism->health << " health and " << organism->left << " left at {" << organism->getCoordinates().y << ", " << organism->getCoordinates().x << "}" << std::endl;
                    #endif
                    Organism::dead_organisms.push_back(organism);
                    continue;
                }
                organism->has_given_birth = false;
                organism->stats.age++;
                organism->left--;
                if (organism->health < organism->dna->genes.at(Gene::STRENGTH)->value*5)
                    organism->left--; // If the organism is weak, it will die faster
                organism->move();
            }
            // All the organisms may meet
            for (int o = 0; o < (int)(Organism::organisms.size()); o++) {
                void* organism_ = Organism::organisms[o];
                if (organism_ == nullptr) {
                    continue;
                }
                Organism* organism = (Organism*)organism_;
                if (isDead(organism)) {
                    sista::Coordinates coordinates = organism->getCoordinates();
                    if (field->getPawn(coordinates) == organism)
                        field->removePawn(coordinates);
                    continue;
                }
                sista::Coordinates coordinates = organism->getCoordinates();
                sista::Coordinates neighbor_coordinates[4];
                neighbor_coordinates[0] = sista::Coordinates(coordinates.x, coordinates.y-1);
                neighbor_coordinates[1] = sista::Coordinates(coordinates.x, coordinates.y+1);
                neighbor_coordinates[2] = sista::Coordinates(coordinates.x-1, coordinates.y);
                neighbor_coordinates[3] = sista::Coordinates(coordinates.x+1, coordinates.y);
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
                if (MUTATION_RATE(random_engine))
                    organism->dna->rational_mutate();
            }
            // Clean the dead organisms
            for (Organism* organism : Organism::dead_organisms) {
                Organism::organisms.erase(std::remove(Organism::organisms.begin(), Organism::organisms.end(), organism), Organism::organisms.end());
            }
            Organism::dead_organisms.clear(); // I hope this doesn't cause a memory leak
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            #if __linux__ or __APPLE__
                ANSI::reset();
                sista::clearScreen();
                ANSI::reset();
                field->print(border);
            #endif
            for (int o = 0; o < (int)(Organism::organisms.size()); o++) {
                void* organism_;
                if (o > 100) {
                    organism_ = &*Organism::organisms.back();
                } else
                    organism_ = Organism::organisms[o];
                if (organism_ == nullptr) {
                    continue;
                }
                Organism* organism = (Organism*)organism_;
                if (isDead(organism)) {
                    continue;
                }
                cursor.set({(short unsigned)o, 54});
                #if _WIN32
                    ANSI::reset();
                    char void_[90] = {' '};
                    std::cout << void_;
                    cursor.set({(short unsigned)o, 54});
                #endif
                std::cout << "Organism ";
                organism->print();
                ANSI::reset();
                std::cout << " " << organism->id << " (" << organism->stats.age << "): " << organism->health << "hp, " << organism->left << " left,";
                std::cout << " DNA: ";
                organism->dna->printInline();
                std::cout << " {" << organism->getCoordinates().y << ", " << organism->getCoordinates().x << "}   ";
                if (o > 100)
                    break; // Don't print all the organisms over each other
            }
            std::cout << std::flush;
            if (quit) {
                break;
            }
        }
        #if _WIN32
            ANSI::reset();
            sista::clearScreen();
            ANSI::reset();
            field->print(border);
        #endif
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

    #if defined(_WIN32) or defined(__linux__)
        getch();
    #elif __APPLE__
        getchar();
    #endif
    #ifdef __APPLE__
        // noecho.c_lflag &= ~ECHO;, noecho.c_lflag |= ECHO;
        tcsetattr(0, TCSAFLUSH, &orig_termios);
    #endif
    return 0;
}


void saveOrganisms() {
    std::ofstream organisms("organisms_set.sklg");
    for (Organism* organism : Organism::organisms) {
        // Format: id, symbol, foreground, background, ...
        organisms << organism->id << ' ' << organism->getSymbol() << ' ' << organism->getSettings().foregroundColor << ' ' << organism->getSettings().backgroundColor << ' ';
        // ..., y, x, age, left, health, ...
        organisms << organism->getCoordinates().y << ' ' << organism->getCoordinates().x << ' ' << organism->stats.age << ' ' << organism->left << ' ' << organism->health << ' ';
        // ..., DNA
        for (Gene gene : genes) {
            organisms << organism->dna->genes[gene]->value << ' ';
        }
        organisms << '\n';
    }
    organisms << std::flush;
}
void loadOrganisms() {
    // Load scenario from file
    std::ifstream sklg_("organisms_set.sklg");
    int organisms_number = 0;
    std::string line;
    while (getline(sklg_, line)) {
        organisms_number++;
    }
    sklg_.close();
    std::ifstream sklg("organisms_set.sklg");
    for (int i = 0; i < organisms_number; i++) {
        Organism* organism;
        // Format: id, symbol, foreground, background, ...
        int id, foreground, background;
        char symbol;
        sklg >> id >> symbol >> foreground >> background;
        // ..., y, x, age, left, health, ...
        sista::Coordinates coord;
        int age, left, health;
        sklg >> coord.y >> coord.x >> age >> left >> health;
        // ..., DNA, ...
        DNA* dna = new DNA(true);
        for (Gene gene : genes) {
            int value;
            sklg >> value;
            Allele* allele = new Allele(gene, value);
            dna->alleles.push_back(allele);
            dna->genes[gene] = allele;
        }
        Statistics void_stats{age, 0, {nullptr, nullptr}, {}};
        organism = new Organism(
            symbol, coord, ANSI::Settings(
                (ANSI::ForegroundColor)foreground,
                (ANSI::BackgroundColor)background,
                ANSI::Attribute::BRIGHT
            ), dna, void_stats
        );
        organism->health = health;
        organism->left = left;
        organism->id = id;
        sista::Pawn* pawn_ = (sista::Pawn*)(Entity*)organism;
        field->addPawn(pawn_);
    }
}

void input(bool& paused, bool& quit) {
    while (true) {
        char c;
        #if defined(_WIN32) or defined(__linux__)
            c = getch();
        #elif __APPLE__
            c = getchar();
        #endif
        if (c == 'r') {
            std::string resume = "resume";
            bool resume_ = true;
            for (int i = 0; i < (int)(resume.size()); i++) {
                if (c != resume[i]) {
                    resume_ = false;
                    break;
                }
                #if defined(_WIN32) or defined(__linux__)
                    c = getch();
                #elif __APPLE__
                    c = getchar();
                #endif
            }
            if (resume_) {
                paused = false;
            }
        } else if (c == 'p') {
            std::string pause = "pause";
            bool pause_ = true;
            for (int i = 0; i < (int)(pause.size()); i++) {
                if (c != pause[i]) {
                    pause_ = false;
                    break;
                }
                #if defined(_WIN32) or defined(__linux__)
                    c = getch();
                #elif __APPLE__
                    c = getchar();
                #endif
            }
            if (pause_) {
                paused = true;
            }
        } else if (c == 'q') {
            std::string quit_string = "quit";
            bool quit_ = true;
            for (int i = 0; i < (int)(quit_string.size()); i++) {
                if (c != quit_string[i]) {
                    quit_ = false;
                    break;
                }
                #if defined(_WIN32) or defined(__linux__)
                    c = getch();
                #elif __APPLE__
                    c = getchar();
                #endif
            }
            if (quit_) {
                quit = true;
                return;
            }
        } else if (c == 's') {
            std::string save_string = "save";
            bool save_ = true;
            for (int i = 0; i < (int)(save_string.size()); i++) {
                if (c != save_string[i]) {
                    save_ = false;
                    break;
                }
                #if defined(_WIN32) or defined(__linux__)
                    c = getch();
                #elif __APPLE__
                    c = getchar();
                #endif
            }
            if (save_) {
                saveOrganisms();
            }
        }
    }
}

bool isDead(Organism* organism) {
    for (std::vector<Organism*>::iterator it = Organism::dead_organisms.begin(); it != Organism::dead_organisms.end(); it++) {
        if (*it == organism) {
            return true;
        }
    }
    return false;
}
int freeSpacesAround(Organism* organism) {
    sista::Coordinates coordinates = organism->getCoordinates();
    sista::Coordinates neighbor_coordinates[4];
    neighbor_coordinates[0] = sista::Coordinates(coordinates.y-1, coordinates.x);
    neighbor_coordinates[1] = sista::Coordinates(coordinates.y+1, coordinates.x);
    neighbor_coordinates[2] = sista::Coordinates(coordinates.y, coordinates.x-1);
    neighbor_coordinates[3] = sista::Coordinates(coordinates.y, coordinates.x+1);
    int free_spaces = 0;
    for (sista::Coordinates coordinates : neighbor_coordinates) {
        if (field->isOutOfBounds(coordinates)) {
            continue;
        }
        std::vector<sista::Pawn*>::iterator pawn = field->getPawnIterator(coordinates);
        if (*pawn == nullptr) {
            free_spaces++;
        }
    }
    return free_spaces;
}