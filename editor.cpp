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


void loadOrganisms();
void printOrganism();
void editOrganism() {}
void removeOrganism() {}
void newOrganism() {}
void saveOrganisms();
bool actionFromChar(char);
sista::Coordinates* coordinates;
sista::Cursor* cursor;
sista::Border* border;


int main() {
    #ifdef __APPLE__
        term_echooff();
    #endif
    sista::Coordinates coordinates_(0, 0);
    sista::Cursor cursor_;
    sista::Border border_(
        '#', ANSI::Settings(
            ANSI::ForegroundColor::F_WHITE,
            ANSI::BackgroundColor::B_BLACK,
            ANSI::Attribute::BRIGHT
        )
    );
    coordinates = &coordinates_;
    cursor = &cursor_;
    border = &border_;
    sista::Coordinates cursor_coordinates[2];
    sista::Field field_(50, 30);
    field = &field_;
    loadOrganisms();
    while (true) {
        field->print(border_);
        coordinates_.y %= 30;
        coordinates_.x %= 50;
        cursor_coordinates[0] = coordinates_;
        cursor_coordinates[1] = coordinates_;
        cursor_coordinates[0].x = 50;
        cursor_coordinates[1].y = 30;
        cursor_.set(cursor_coordinates[0]);
        std::cout << "<";
        cursor_.set(cursor_coordinates[1]);
        std::cout << "^";
        cursor_.set(coordinates_);
        std::cout << "X" << std::flush;
        #if defined(_WIN32) or defined(__linux__)
            char c = getch();
        #elif __APPLE__
            char c = getchar();
        #endif
        if (actionFromChar(c)) {
            cursor_.set(35, 10);
            std::cout << "Save? (y/n)" << std::flush;
            #if defined(_WIN32) or defined(__linux__)
                char c = getch();
            #elif __APPLE__
                char c = getchar();
            #endif
            if (c == 'y') {
                saveOrganisms();
            }
            break;
        }
        sista::clearScreen();
    }
    field->reset();
}


bool actionFromChar(char c) {
    switch (c) {
        case 'w':
            coordinates->y < 1 ? coordinates->y = 29 : coordinates->y--;
            break;
        case 's':
            coordinates->y++;
            break;
        case 'a':
            coordinates->x < 1 ? coordinates->x = 49 : coordinates->x--;
            break;
        case 'd':
            coordinates->x++;
            break;
        case 'q':
            return true;
            break;
        case 'p': // Print Organism
            printOrganism();
            break;
        case 'e': // Edit Organism
            editOrganism();
            break;
        case 'r': // Remove Organism
            removeOrganism();
            break;
        case 'n': // New Organism
            newOrganism();
            break;
        case 'c': // Print coordinates
            cursor->set(35, 55);
            std::cout << "{" << coordinates->y << ", " << coordinates->x << "}" << std::flush;
            #if defined(_WIN32) or defined(__linux__)
                getch();
            #elif __APPLE__
                getchar();
            #endif
            break;
    }
    return false;
}


void printOrganism() {
    // We need to find organism on this coordinates
    Organism* organism = nullptr;
    for (Organism* organism_ : Organism::organisms) {
        if (organism_->getCoordinates() == *coordinates) {
            organism = organism_;
            break;
        }
    }
    if (organism == nullptr) {
        cursor->set(35, 10);
        std::cout << "No organism on this coordinates" << std::flush;
        #if defined(_WIN32) or defined(__linux__)
            getch();
        #elif __APPLE__
            getchar();
        #endif
        return;
    } else {
        ANSI::reset();
        cursor->set(5, 55);
        std::cout << "Organism: " << organism->id;
        cursor->set(6, 55);
        std::cout << "Symbol: ";
        organism->print();
        ANSI::reset();
        cursor->set(7, 55);
        std::cout << "Health: " << organism->health;
        cursor->set(8, 55);
        std::cout << "Age: " << organism->stats.age;
        cursor->set(9, 55);
        std::cout << "Left: " << organism->left;
        cursor->set(11, 55);
        for (Gene gene : genes) {
            std::cout << "\t";
            std::cout << gene_to_string[gene] << ": ";
            if (allele_to_string[gene].find(organism->dna->genes[gene]->value) == allele_to_string[gene].end())
                std::cout << organism->dna->genes[gene]->value;
            else
                std::cout << allele_to_string[gene][organism->dna->genes[gene]->value];
            cursor->set(11 + (int)gene, 55);
        }
        std::cout << std::flush;
        #if defined(_WIN32) or defined(__linux__)
            getch();
        #elif __APPLE__
            getchar();
        #endif
    }
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
    sklg_.open("organisms_set.sklg", std::ios::in);
    for (int i = 0; i < organisms_number; i++) {
        Organism* organism;
        // Format: id, symbol, foreground, background, ...
        int id, foreground, background;
        char symbol;
        sklg_ >> id >> symbol >> foreground >> background;
        // ..., y, x, age, left, health, ...
        sista::Coordinates coord;
        int age, left, health;
        sklg_ >> coord.y >> coord.x >> age >> left >> health;
        // ..., DNA, ...
        DNA* dna = new DNA(true);
        for (Gene gene : genes) {
            int value;
            sklg_ >> value;
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
        Organism::id_counter = id;
        sista::Pawn* pawn_ = (sista::Pawn*)(Entity*)organism;
        field->addPawn(pawn_);
    }
}