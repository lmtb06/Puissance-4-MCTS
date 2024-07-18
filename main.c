#include "fonctions.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "structs.h"

/**
 * @brief lance une partie entre un joueur humain et un joueur IA. Avec une configuration par défaut de l'IA.
*/
void jouerAvecIA() {
    ParametresMCTS parametres = {
        .simuler = simulerAleatoire,
        .choixNoeud = choixMaxRecompense,
        .c = sqrt(2),
        .temps_reflexion = 2.0};
    
    humainVsIA(&parametres);
    printf("Fin de la partie\n");
}

/**
 * @brief Lance plusieurs parties entre deux IA avec comme différence l'amélioration de simulation.
*/
void comparaisonAmeliorationSimulation(int nb_simulations)
{
    int resultats[3] = {0, 0, 0}; // 0: match nul, 1: victoire joueur 1(simulation aléatoire), 2: victoire joueur 2 (simulation coup gagnant)
    float temps_reflexion = 0.09;
    ParametresMCTS parametres1 = {
        .simuler = simulerCoupGagnant,
        .choixNoeud = choixMaxRecompense,
        .c = sqrt(2),
        .temps_reflexion = temps_reflexion};

    ParametresMCTS parametres2 = {
        .simuler = simulerCoupGagnant,
        .choixNoeud = choixMaxRecompense,
        .c = sqrt(2),
        .temps_reflexion = temps_reflexion};

    for (int i = 0; i < nb_simulations; i++)
    {
        FinDePartie fin = IAVsIA(&parametres1, &parametres2);
        resultats[fin - 1]++;
    }
    printf("###### Simulation aléatoire vs simulation coup gagnant ######\n");
    printf("Résultats de %d simulations:\n", nb_simulations);
    printf("Matchs nuls: %d\n", resultats[0]);
    printf("Victoires simulation aléatoire: %d\n", resultats[1]);
    printf("Victoires simulation choix coup gagnant: %d\n", resultats[2]);
}

/**
 * @brief Lance plusieurs parties entre deux IA avec comme différence le critère de max et le critère robuste.
*/
void comparaisonCritereMaxRobuste(int nb_simulations)
{
    int resultats[3] = {0, 0, 0}; // 0: match nul, 1: victoire joueur 1(critère max), 2: victoire joueur 2 (critère robuste)
    float temps_reflexion = 0.09;
    ParametresMCTS parametres1 = {
        .simuler = simulerAleatoire,
        .choixNoeud = choixMaxRecompense,
        .c = sqrt(2),
        .temps_reflexion = temps_reflexion};

    ParametresMCTS parametres2 = {
        .simuler = simulerAleatoire,
        .choixNoeud = choixRobuste,
        .c = sqrt(2),
        .temps_reflexion = temps_reflexion};

    for (int i = 0; i < nb_simulations; i++)
    {
        FinDePartie fin = IAVsIA(&parametres1, &parametres2);
        resultats[fin - 1]++;
    }
    printf("###### Critère max vs critère robuste ######\n");
    printf("Résultats de %d simulations:\n", nb_simulations);
    printf("Matchs nuls: %d\n", resultats[0]);
    printf("Victoires critères max: %d\n", resultats[1]);
    printf("Victoires critères robustes: %d\n", resultats[2]);
}

int main(void)
{
    jouerAvecIA();
    // comparaisonAmeliorationSimulation(10);
    // comparaisonCritereMaxRobuste(10);

    return 0;
}