#include "organism.cpp"


std::ofstream dna_stats("dna-stats.txt", std::fstream::out | std::fstream::app);
std::ofstream organisms_count("organisms-count.txt", std::fstream::out | std::fstream::app);

std::ofstream speed_stats("speed-stats.txt", std::fstream::out | std::fstream::app);
std::ofstream strength_stats("strength-stats.txt", std::fstream::out | std::fstream::app);
std::ofstream fertility_stats("fertility-stats.txt", std::fstream::out | std::fstream::app);
std::ofstream nature_stats("nature-stats.txt", std::fstream::out | std::fstream::app);
std::ofstream lifespan_stats("lifespan-stats.txt", std::fstream::out | std::fstream::app);
std::ofstream attack_stats("attack-stats.txt", std::fstream::out | std::fstream::app);
std::ofstream defense_stats("defense-stats.txt", std::fstream::out | std::fstream::app);
std::ofstream vision_stats("vision-stats.txt", std::fstream::out | std::fstream::app);
std::ofstream breath_stats("breath-stats.txt", std::fstream::out | std::fstream::app);
std::unordered_map<Gene, std::ofstream*> gene_stats = {
    {Gene::SPEED, &speed_stats},
    {Gene::STRENGTH, &strength_stats},
    {Gene::FERTILITY, &fertility_stats},
    {Gene::NATURE, &nature_stats},
    {Gene::LIFESPAN, &lifespan_stats},
    {Gene::ATTACK, &attack_stats},
    {Gene::DEFENSE, &defense_stats},
    {Gene::VISION, &vision_stats},
    {Gene::BREATH, &breath_stats}
};
std::ofstream speed_alleles_stats("speed-alleles-stats.txt", std::fstream::out | std::fstream::app);
std::ofstream strength_alleles_stats("strength-alleles-stats.txt", std::fstream::out | std::fstream::app);
std::ofstream fertility_alleles_stats("fertility-alleles-stats.txt", std::fstream::out | std::fstream::app);
std::ofstream nature_alleles_stats("nature-alleles-stats.txt", std::fstream::out | std::fstream::app);
std::ofstream lifespan_alleles_stats("lifespan-alleles-stats.txt", std::fstream::out | std::fstream::app);
std::ofstream attack_alleles_stats("attack-alleles-stats.txt", std::fstream::out | std::fstream::app);
std::ofstream defense_alleles_stats("defense-alleles-stats.txt", std::fstream::out | std::fstream::app);
std::ofstream vision_alleles_stats("vision-alleles-stats.txt", std::fstream::out | std::fstream::app);
std::ofstream breath_alleles_stats("breath-alleles-stats.txt", std::fstream::out | std::fstream::app);
std::unordered_map<Gene, std::ofstream*> alleles_stats = {
    {Gene::SPEED, &speed_alleles_stats},
    {Gene::STRENGTH, &strength_alleles_stats},
    {Gene::FERTILITY, &fertility_alleles_stats},
    {Gene::NATURE, &nature_alleles_stats},
    {Gene::LIFESPAN, &lifespan_alleles_stats},
    {Gene::ATTACK, &attack_alleles_stats},
    {Gene::DEFENSE, &defense_alleles_stats},
    {Gene::VISION, &vision_alleles_stats},
    {Gene::BREATH, &breath_alleles_stats}
};
std::ofstream atmosphere_stats("atmosphere-stats.txt", std::fstream::out | std::fstream::app);


void dumpStats(int _) {
    organisms_count << Organism::organisms.size() << std::endl;
    // I use map<Gene, ...> and not vector<...> just for the purpose of being explicit
    std::unordered_map<Gene, int> sum;
    std::unordered_map<Gene, std::vector<int>> values_found; // values_found[gene] = all values found for that gene
    std::unordered_map<Gene, std::unordered_map<int, int>> values; // values[gene][allele] = counter of that allele
    for (Gene gene : genes) {
        sum.insert({gene, 0});
        values_found.insert({gene, std::vector<int>()});
        values.insert({gene, std::unordered_map<int, int>()});
    }
    for (Organism* organism : Organism::organisms) {
        DNA* dna = organism->dna;
        for (Allele* allele : dna->alleles) {
            if (std::find(values_found[allele->name].begin(), values_found[allele->name].end(), allele->value) == values_found[allele->name].end()) {
                values[allele->name][allele->value] = 1;
                values_found[allele->name].push_back(allele->value);
            } else {
                values[allele->name][allele->value]++;
            }
            sum[allele->name] += allele->value;
        }
    }
    std::unordered_map<Gene, float> averages;
    for (Gene gene : genes) {
        averages.insert({gene, (float)sum[gene]/(float)Organism::organisms.size()});
    }
    // Output the statistics to dna-stats.txt
    dna_stats << "== DNA stats at " << _ << " decaframes from start ==\n\n";
    dna_stats << "Allele's distribution:\n";
    for (Gene gene : genes) {
        dna_stats << "- " << gene_to_string[gene] << " (average: " << averages[gene] << ")\n";
        *gene_stats[gene] << averages[gene] << std::endl;
        std::sort(values_found[gene].begin(), values_found[gene].end());
        for (int value : values_found[gene]) {
            if (std::find(possible_random_allele_values[gene].begin(), possible_random_allele_values[gene].end(), value) != possible_random_allele_values[gene].end()) {
                dna_stats << "\t" << allele_to_string[gene][value] << " : " << values[gene][value] << "\n";
                *alleles_stats[gene] << allele_to_string[gene][value] << ":" << values[gene][value] << ",";
            } else {
                dna_stats << "\t" << value << " : " << values[gene][value] << "\n";
                *alleles_stats[gene] << value << ":" << values[gene][value] << ",";
            }
        }
        *alleles_stats[gene] << std::endl;
    }
    dna_stats << std::endl;
    // Output the statistics to atmosphere-stats.txt
    atmosphere_stats << globals::oxygen << ',' << globals::carbon_dioxide << std::endl;
}