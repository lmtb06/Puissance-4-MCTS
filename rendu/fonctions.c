#include "fonctions.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

Etat *etat_initial(Joueur joueur_qui_commence)
{
    Etat *etat = (Etat *)malloc(sizeof(Etat));
    etat->joueur_actuel = joueur_qui_commence;

    // Initialisation du plateau
    for (int i = 0; i < NB_LIGNES; i++)
        for (int j = 0; j < NB_COLONNES; j++)
            etat->plateau[i][j] = VIDE;

    return etat;
}

Etat *copieEtat(Etat *src)
{
    Etat *etat = (Etat *)malloc(sizeof(Etat));
    etat->joueur_actuel = src->joueur_actuel;

    // Copie du plateau
    for (int i = 0; i < NB_LIGNES; i++)
        for (int j = 0; j < NB_COLONNES; j++)
            etat->plateau[i][j] = src->plateau[i][j];

    return etat;
}

int lignePremiereCaseLibre(Etat *etat, int colonne)
{
    for (int i = 0; i < NB_LIGNES; i++)
        if (etat->plateau[i][colonne] == VIDE)
            return i;
    return -1;
}

int jouerCoup(Etat *etat, Coup *coup)
{
    // Le symbole du joueur actuel est placé dans la première case vide de la colonne
    int ligne = lignePremiereCaseLibre(etat, coup->colonne);
    if (ligne != -1)
    {
        // On joue le coup et on change de joueur
        etat->plateau[ligne][coup->colonne] = SYMBOLE_JOUEUR(etat->joueur_actuel);
        etat->joueur_actuel = AUTRE_JOUEUR(etat->joueur_actuel);
        return 1;
    }

    return 0;
}

Coup *nouveauCoup(int colonne)
{
    Coup *coup = (Coup *)malloc(sizeof(Coup));
    coup->colonne = colonne;
    return coup;
}

Coup **coups_possibles(Etat *etat)
{
    Coup **coups = (Coup **)malloc((LARGEUR_MAX + 1) * sizeof(Coup *));
    int nb_coups = 0;

    // Nb de coups possibles = nb de colonnes non pleines
    for (int i = 0; i < NB_COLONNES; i++)
        if (lignePremiereCaseLibre(etat, i) != -1)
        { // Si la colonne n'est pas pleine
            coups[nb_coups] = nouveauCoup(i);
            nb_coups++;
        }

    coups[nb_coups] = NULL;
    return coups;
}

Noeud *nouveauNoeud(Noeud *parent, Coup *coup)
{
    Noeud *noeud = (Noeud *)malloc(sizeof(Noeud));

    if (parent != NULL && coup != NULL)
    {
        noeud->etat = copieEtat(parent->etat);
        jouerCoup(noeud->etat, coup);
        noeud->coup = coup;
        noeud->joueur_actuel = AUTRE_JOUEUR(parent->joueur_actuel);
    }
    else
    {
        noeud->etat = NULL;
        noeud->coup = NULL;
        noeud->joueur_actuel = JOUEUR1;
    }
    noeud->parent = parent;
    noeud->nb_enfants = 0;

    // POUR MCTS:
    noeud->nb_victoires = 0;
    noeud->nb_simus = 0;
    return noeud;
}

Noeud *ajouterEnfant(Noeud *parent, Coup *coup)
{
    Noeud *enfant = nouveauNoeud(parent, coup);
    parent->enfants[parent->nb_enfants] = enfant;
    parent->nb_enfants++;
    return enfant;
}

void freeNoeud(Noeud *noeud)
{
    if (noeud->etat != NULL)
        free(noeud->etat);
    if (noeud->coup != NULL)
        free(noeud->coup);
    for (int i = 0; i < noeud->nb_enfants; i++)
        freeNoeud(noeud->enfants[i]);

    free(noeud);
}

void freeListeCoupsPossibles(Coup **coups_possibles) {
    for (int i = 0; coups_possibles[i] != NULL; i++)
        free(coups_possibles[i]);
    free(coups_possibles);
}

int verifierAlignement4(Etat *etat, int ligne, int colonne, int deltaLigne, int deltaColonne)
{
    Case symboleCase = etat->plateau[ligne][colonne];
    if (symboleCase == VIDE)
        return 0;

    for (int k = 1; k < 4; k++)
    {
        int nouvelleLigne = ligne + deltaLigne * k;
        int nouvelleColonne = colonne + deltaColonne * k;
        if (
            (nouvelleLigne < 0) || (nouvelleLigne >= NB_LIGNES) ||        // On sort du plateau ligne ?
            (nouvelleColonne < 0) || (nouvelleColonne >= NB_COLONNES) ||  // On sort du plateau colonne ?
            etat->plateau[nouvelleLigne][nouvelleColonne] != symboleCase) // On a un symbole différent de celui de base ?
        {
            return 0;
        }
    }
    return 1;
}

FinDePartie testFin(Etat *etat)
{
    int existenceCaseVide = 0;
    // Vérification des alignements dans toutes les directions
    for (int i = 0; i < NB_LIGNES; i++)
    {
        for (int j = 0; j < NB_COLONNES; j++)
        {
            if (verifierAlignement4(etat, i, j, 0, 1) || // Horizontal
                verifierAlignement4(etat, i, j, 1, 0) || // Vertical
                verifierAlignement4(etat, i, j, 1, 1) || // Diagonale montante
                verifierAlignement4(etat, i, j, 1, -1))
            { // Diagonale descendante
                return etat->plateau[i][j] == SYMBOLE_JOUEUR(JOUEUR1) ? J1_GAGNE : J2_GAGNE;
            }
            existenceCaseVide = existenceCaseVide || (etat->plateau[i][j] == VIDE);
        }
    }

    // La partie continue s'il reste des cases vides
    if (existenceCaseVide)
        return NON;

    return MATCHNUL;
}

Noeud *selectionnerNoeud(Noeud *noeud, float c, Joueur joueurOrdi)
{
    if (noeud->nb_enfants == 0) // Si le noeud est terminal
        return noeud;


    Noeud *meilleurNoeud = NULL;
    float meilleurScore = -INFINITY;
    float bValeur;
    float mu;
    for (int i = 0; i < noeud->nb_enfants; i++)
    {
        if (noeud->enfants[i]->nb_simus == 0) // Si le fils à un noeud fils non développé on le sélectionne
            return noeud->enfants[i];
        Noeud *enfant = noeud->enfants[i];
        // Calcul de la B-valeur
        mu = (float)enfant->nb_victoires / enfant->nb_simus;
        mu = enfant->joueur_actuel == joueurOrdi ? mu : -mu;
        bValeur = mu + c * sqrt(log(noeud->nb_simus) / enfant->nb_simus);
        if (bValeur > meilleurScore)
        {
            meilleurScore = bValeur;
            meilleurNoeud = enfant;
        }
    }

    return selectionnerNoeud(meilleurNoeud, c, joueurOrdi);
}

CoupStatistiques *meilleurCoupMCTS(Etat *etat, fonctionSimulation simuler, fonctionChoixNoeud choixNoeud, float tempsMax, Joueur joueurOrdi, float c)
{
    clock_t debut;
    debut = clock();
    float temps;
    // Création de l'arbre de recherche
    Noeud *racine = nouveauNoeud(NULL, NULL);
    racine->etat = copieEtat(etat);
    
    srand(time(NULL));
    do // Tant qu'on n'a pas atteint le temps maximum
    {
        // Sélection du noeud à développer
        Noeud *noeud = selectionnerNoeud(racine, c, joueurOrdi);
        // Developpement du noeud
        Coup **coups = coups_possibles(noeud->etat);
        for (int i = 0; coups[i] != NULL; i++)
        {
            ajouterEnfant(noeud, coups[i]);
        }
        free(coups);
        // Selection d'un fils aléatoire parmi les fils du noeud non explorés ou choix du noeud lui même s'il est terminal
        Noeud *noeudSimu = noeud;
        if (noeud->nb_enfants > 0)
        {
            int nbEnfantsNonExplores = 0;
            Noeud **enfantsNonExplores = (Noeud **)malloc(noeud->nb_enfants * sizeof(Noeud *));
            for (int i = 0; i < noeud->nb_enfants; i++)
            {
                if (noeud->enfants[i]->nb_simus == 0)
                {
                    enfantsNonExplores[nbEnfantsNonExplores] = noeud->enfants[i];
                    nbEnfantsNonExplores++;
                }
            }
            // Choix aléatoire d'un fils non exploré
            int noEnfant = rand() % nbEnfantsNonExplores;
            noeudSimu = enfantsNonExplores[noEnfant];
            free(enfantsNonExplores);
        }
        // Simulation
        int resultat = simuler(noeudSimu->etat, joueurOrdi);
        // Mis à jour des statistiques
        while (noeudSimu != NULL)
        {
            noeudSimu->nb_simus++;
            if (resultat == 1)
                noeudSimu->nb_victoires++;
            noeudSimu = noeudSimu->parent;
        }
        temps = (clock() - debut) / (float)CLOCKS_PER_SEC;
    }

    while (temps < tempsMax);

    // Choix du meilleur coup
    Noeud *meilleurNoeud = choixNoeud(racine);
    CoupStatistiques *coupStatistiques = (CoupStatistiques *)malloc(sizeof(CoupStatistiques));
    coupStatistiques->coup = malloc(sizeof(Coup));
    coupStatistiques->coup->colonne = meilleurNoeud->coup->colonne;
    coupStatistiques->nb_simus = racine->nb_simus;
    coupStatistiques->proba_gagner = meilleurNoeud->nb_victoires/(float)meilleurNoeud->nb_simus;
    // Libération de la mémoire
    freeNoeud(racine);
    return coupStatistiques;
}

void freeCoupStatistiques(CoupStatistiques *coupStatistiques)
{
    free(coupStatistiques->coup);
    free(coupStatistiques);
}

int simulerAleatoire(Etat *etat, Joueur joueur)
{
    Etat *etatSimulation = copieEtat(etat);
    while (testFin(etatSimulation) == NON)
    {
        Coup **coups = coups_possibles(etatSimulation);
        int nbCoups = 0;
        for (int i = 0; coups[i] != NULL; i++)
        {
            nbCoups++;
        }
        Coup *coup = coups[rand() % nbCoups];
        // Marche aléatoire
        jouerCoup(etatSimulation, coup);
        // Libération de la mémoire
        freeListeCoupsPossibles(coups);
    }
    FinDePartie fin = testFin(etatSimulation);
    free(etatSimulation);
    return EST_GAGNANT(fin, joueur);
}

int simulerCoupGagnant(Etat *etat, Joueur joueur)
{
    Etat *etatSimulation = copieEtat(etat);
    while (testFin(etatSimulation) == NON)
    {
        Coup *coupGagnant = NULL;
        Coup **coups = coups_possibles(etatSimulation);
        int nbCoups = 0;
        for (int i = 0; coups[i] != NULL; i++)
        {
            nbCoups++;
            Etat *copieEtatSimulation = copieEtat(etat);
            jouerCoup(copieEtatSimulation, coups[i]);
            FinDePartie fin = testFin(copieEtatSimulation);
            // Si le coup est gagnant on le joue
            if (EST_GAGNANT(fin, joueur))
            {
                free(copieEtatSimulation);
                coupGagnant = coups[i];
                break;
            }
            free(copieEtatSimulation);
        }

        if (coupGagnant != NULL)
        {
            jouerCoup(etatSimulation, coupGagnant);
            freeListeCoupsPossibles(coups);
            break;
        }

        Coup *coup = coups[rand() % nbCoups];
        jouerCoup(etatSimulation, coup);

        freeListeCoupsPossibles(coups);
    }
    FinDePartie fin = testFin(etatSimulation);
    free(etatSimulation);
    return EST_GAGNANT(fin, joueur);
}

Noeud *choixMaxRecompense(Noeud *noeud)
{
    float maxRecompense = -INFINITY;
    Noeud *meilleurNoeud = NULL;
    for (int i = 0; i < noeud->nb_enfants; i++)
    {
        Noeud *enfant = noeud->enfants[i];
        float recompense = enfant->nb_victoires / (float)enfant->nb_simus;
        if (recompense > maxRecompense)
        {
            maxRecompense = recompense;
            meilleurNoeud = enfant;
        }
    }
    return meilleurNoeud;
}

Noeud *choixRobuste(Noeud *noeud)
{
    float maxSimulation = -INFINITY;
    Noeud *meilleurNoeud = NULL;
    for (int i = 0; i < noeud->nb_enfants; i++)
    {
        Noeud *enfant = noeud->enfants[i];
        if (enfant->nb_simus > maxSimulation)
        {
            maxSimulation = enfant->nb_simus;
            meilleurNoeud = enfant;
        }
    }
    return meilleurNoeud;
}

void afficheJeu(Etat *etat)
{

    int i, j;
    for (i = NB_LIGNES - 1; i >= 0; i--)
    {
        printf("----------------------------");
        printf("\n");
        printf("|");
        for (j = 0; j < NB_COLONNES; j++)
            printf(" %c |", etat->plateau[i][j]);
        printf("\n");
    }

    printf("----------------------------");
    printf("\n");
    printf("|");
    for (j = 0; j < NB_COLONNES; j++)
        printf(" %d |", j);
    printf("\n");
}

void afficheCoupStatistiques(CoupStatistiques *coupStatistiques)
{
    printf("Coup : %d, Nb simus : %d, Proba de victoire estimée : %.2f\n", coupStatistiques->coup->colonne, coupStatistiques->nb_simus, coupStatistiques->proba_gagner);
}

Coup *demanderCoup()
{
    int j;
    printf(" quelle colonne ? ");
    scanf("%d", &j);

    return nouveauCoup(j);
}

void humainVsIA(ParametresMCTS *parametresIA)
{
    FinDePartie fin;
    int joueur_qui_commence;
    Joueur joueurHumain;

    printf("Qui commence (0 : humain, 1 : ordinateur) ? ");
    while (scanf("%d", &joueur_qui_commence) != 1 || (joueur_qui_commence != 0 && joueur_qui_commence != 1))
    {
        printf("Entrée invalide. Veuillez entrer 0 pour humain ou 1 pour ordinateur: ");
        while (getchar() != '\n')
            ; // Nettoyer le buffer d'entrée
    }

    joueurHumain = (joueur_qui_commence == 0) ? JOUEUR1 : JOUEUR2;

    // Initialisation de l'état du jeu
    Etat *etat = etat_initial(JOUEUR1);

    // Boucle de jeu
    do
    {
        printf("\n");
        afficheJeu(etat);

        if (etat->joueur_actuel == joueurHumain)
        {
            // Tour de l'humain
            Coup *coup;
            do
            {
                coup = demanderCoup();
            } while (!jouerCoup(etat, coup));
            free(coup); // Libérer la mémoire après avoir joué le coup
        }
        else
        {
            // Tour de l'ordinateur
            CoupStatistiques *coupStat = meilleurCoupMCTS(etat, parametresIA->simuler, parametresIA->choixNoeud, parametresIA->temps_reflexion, AUTRE_JOUEUR(joueurHumain), parametresIA->c);
            afficheCoupStatistiques(coupStat);
            jouerCoup(etat, coupStat->coup);
            freeCoupStatistiques(coupStat);
        }

        fin = testFin(etat);
    } while (fin == NON);

    printf("\n");
    afficheJeu(etat);

    if (fin == MATCHNUL)
    {
        printf("Match nul !\n");
    }
    else if (EST_GAGNANT(fin, joueurHumain))
    {
        printf("** BRAVO, vous avez gagné **\n");
    }
    else
    {
        printf("** L'ordinateur a gagné **\n");
    }

    // Libération de la mémoire
    free(etat);
}

void humainVsHumain()
{
    FinDePartie fin;
    // Initialisation de l'état du jeu
    Etat *etat = etat_initial(JOUEUR1);
    // Boucle de jeu
    do
    {
        printf("\n");
        afficheJeu(etat);

        if (etat->joueur_actuel == JOUEUR1)
        {
            printf("Joueur 1, à vous de jouer\n");
        }
        else
        {
            printf("Joueur 2, à vous de jouer\n");
        }
        // Tour de l'humain
        Coup *coup;
        do
        {
            coup = demanderCoup();
        } while (!jouerCoup(etat, coup));
        free(coup); // Libérer la mémoire après avoir joué le coup

        fin = testFin(etat);
    } while (fin == NON);

    printf("\n");
    afficheJeu(etat);

    if (fin == MATCHNUL)
    {
        printf("Match nul !\n");
    }
    else if (EST_GAGNANT(fin, JOUEUR1))
    {
        printf("** BRAVO Joueur 1, vous avez gagné **\n");
    }
    else
    {
        printf("** BRAVO Joueur 2, vous avez gagné **\n");
    }

    // Libération de la mémoire
    free(etat);
}

FinDePartie IAVsIA(ParametresMCTS *parametres1, ParametresMCTS *parametres2)
{
    FinDePartie fin;
    // Initialisation de l'état du jeu
    Etat *etat = etat_initial(JOUEUR1);
    int statistiques_colonnes_joues_IA1[NB_COLONNES] = {0};
    int statistiques_colonnes_joues_IA2[NB_COLONNES] = {0};
    int nb_coups_IA1 = 0;
    int nb_coups_IA2 = 0;
    // Boucle de jeu
    do
    {
        // printf("\n");
        // afficheJeu(etat);

        if (etat->joueur_actuel == JOUEUR1)
        {
            // Tour de l'ordinateur 1
            // printf("Tour de l'ordinateur 1\n");
            CoupStatistiques *coupStat = meilleurCoupMCTS(etat, parametres1->simuler, parametres1->choixNoeud, parametres1->temps_reflexion, JOUEUR1, parametres1->c);
            // afficheCoupStatistiques(coupStat);
            jouerCoup(etat, coupStat->coup);
            statistiques_colonnes_joues_IA1[coupStat->coup->colonne]++;
            nb_coups_IA1++;
            freeCoupStatistiques(coupStat);
        }
        else
        {
            // Tour de l'ordinateur 2
            // printf("Tour de l'ordinateur 2\n");
            CoupStatistiques *coupStat = meilleurCoupMCTS(etat, parametres2->simuler, parametres2->choixNoeud, parametres2->temps_reflexion, JOUEUR2, parametres2->c);
            // afficheCoupStatistiques(coupStat);
            jouerCoup(etat, coupStat->coup);
            statistiques_colonnes_joues_IA2[coupStat->coup->colonne]++;
            nb_coups_IA2++;
            freeCoupStatistiques(coupStat);
        }

        fin = testFin(etat);
    } while (fin == NON);

    printf("\n");
    afficheJeu(etat);
    // Normalisation des statistiques
    float statistiques_normalisees_IA1[NB_COLONNES];
    float statistiques_normalisees_IA2[NB_COLONNES];
    for (int i = 0; i < NB_COLONNES; i++)
    {
        statistiques_normalisees_IA1[i] = statistiques_colonnes_joues_IA1[i] / (float)nb_coups_IA1;
        statistiques_normalisees_IA2[i] = statistiques_colonnes_joues_IA2[i] / (float)nb_coups_IA2;
    }

    printf("Statistiques des colonnes jouées par l'ordinateur 1:\n");
    for (int i = 0; i < NB_COLONNES; i++)
    {
        printf("Colonne %d : %.2f %% \n", i, statistiques_normalisees_IA1[i] * 100);
    }
    printf("Statistiques des colonnes jouées par l'ordinateur 2:\n");
    for (int i = 0; i < NB_COLONNES; i++)
    {
        printf("Colonne %d : %.2f %% \n", i, statistiques_normalisees_IA2[i] * 100);
    }


    // Libération de la mémoire
    free(etat);
    return fin;
}