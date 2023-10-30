#include "organism.cpp"
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


bool isDead(Organism* organism) {
    for (std::vector<Organism*>::iterator it = Organism::dead_organisms.begin(); it != Organism::dead_organisms.end(); it++) {
        if (*it == organism) {
            return true;
        }
    }
    return false;
}
bool isAsphyxiated(Organism* organism) {
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
            return false; // There is at least one free space
        }
    }
    return true; // There is no free space
}


int main() {
    #ifdef _WIN32
        CONSOLE_FONT_INFOEX font_info;
        font_info.cbSize = sizeof(font_info);
        font_info.dwFontSize.X = 31;
        font_info.dwFontSize.Y = 11;
        font_info.FontFamily = FF_DONTCARE;
        font_info.FontWeight = FW_NORMAL;
        SetCurrentConsoleFontEx(
            GetStdHandle(STD_OUTPUT_HANDLE),
            false, &font_info
        );
    #endif
    #ifdef __APPLE__
        term_echooff();
    #endif
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
    sista::Cursor cursor;

    // Create the organisms
    for (int i = 0; i < 26; i++) {
        Organism* organism;
        char symbol = 'A' + i;
        sista::Coordinates coordinates(
            (unsigned)(random_engine() % 30),
            (unsigned)(random_engine() % 50)
        );
        ANSI::ForegroundColor foreground_color = (ANSI::ForegroundColor)(random_engine() % 8 + 30);
        ANSI::BackgroundColor background_color = (ANSI::BackgroundColor)(random_engine() % 8 + 40);
        if ((int)foreground_color == (int)background_color) {
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
    Organism::dead_organisms.clear();

    bool paused = false;
    std::thread input_thread([&paused]() {
        while (true) {
            #if defined(_WIN32) or defined(__linux__)
                getch();
            #elif __APPLE__
                getchar();
            #endif
            paused = !paused;
        }
    });
    for (int _ = 0; _ < 100; _++) {
        for (int i = 0; i < 10; i++) {
            while (paused) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            // All the organisms may move
            for (std::vector<Organism*>::iterator it = Organism::organisms.begin(); it != Organism::organisms.end(); it++) {
                Organism* organism = *it;
                if (organism == nullptr) {
                    continue;
                }
                if (isDead(organism)) {
                    debug << "Organism " << organism << " (" << organism->id << ") is [already] dead with " << organism->health << " health and " << organism->left << " left at {" << organism->getCoordinates().y << ", " << organism->getCoordinates().x << "}" << std::endl;
                    sista::Coordinates coordinates = organism->getCoordinates();
                    if (field->getPawn(coordinates) == organism)
                        field->removePawn(organism);
                    continue;
                }
                if (organism->left <= 0 || organism->health <= 0) {
                    debug << "Organism " << organism << " (" << organism->id << ") is dead with " << organism->health << " health and " << organism->left << " left at {" << organism->getCoordinates().y << ", " << organism->getCoordinates().x << "}" << std::endl;
                    Organism::dead_organisms.push_back(organism);
                    continue;
                }
                // Check of asphyxiation
                if (isAsphyxiated(organism)) {
                    debug << "Organism " << organism << " (" << organism->id << ") is asphyxiated with " << organism->health << " health and " << organism->left << " left at {" << organism->getCoordinates().y << ", " << organism->getCoordinates().x << "}" << std::endl;
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
                        field->removePawn(organism);
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
            ANSI::reset();
            sista::clearScreen();
            ANSI::reset();
            field->print(border);
            for (int o = 0; o < (int)(Organism::organisms.size()); o++) {
                void* organism_ = Organism::organisms[o];
                if (organism_ == nullptr) {
                    continue;
                }
                Organism* organism = (Organism*)organism_;
                if (isDead(organism)) {
                    continue;
                }
                cursor.set({(short unsigned)o, 54});
                std::cout << "Organism " << organism->id << " (" << organism->stats.age << "): " << organism->health << " health, " << organism->left << " left";
                std::cout << " DNA: ";
                organism->dna->printInline();
                std::cout << " {" << organism->getCoordinates().y << ", " << organism->getCoordinates().x << "}"; 
            }
            std::cout << std::flush;
        }
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
}