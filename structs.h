/**
 * @file structs.h
 * @brief Fichier contient les définitions des différentes structures, constantes et énumérations utilisées dans le jeu.
 */

#pragma once

/*
 * PARAMETRES DU JEU *
 */

#define NB_COLONNES 7           // Nombre de colonnes de la grille de jeu
#define NB_LIGNES 6             // Nombre de lignes de la grille de jeu
#define LARGEUR_MAX NB_COLONNES // nb max de fils pour un noeud (= nb max de coups possibles) ici le nb de colonnes

/*
 * ENUMS *
 */

/**
 * @enum Case
 * @brief Représente les différentes cases de la grille de jeu.
 */
typedef enum
{
    VIDE = ' ',        /**< Case vide */
    JOUEUR1_SYM = 'X', /**< Symbole du joueur 1 */
    JOUEUR2_SYM = 'O'  /**< Symbole du joueur 2 */
} Case;

/**
 * @enum Joueur
 * @brief Représente les joueurs dans le jeu.
 */
typedef enum
{
    JOUEUR1, /**< Joueur 1 */
    JOUEUR2  /**< Joueur 2 */
} Joueur;

/**
 * @enum FinDePartie
 * @brief Représente les conditions de fin de partie.
 */
typedef enum
{
    NON,      /**< La partie n'est pas terminée */
    MATCHNUL, /**< La partie se termine par un match nul */
    J1_GAGNE, /**< Le joueur 1 gagne */
    J2_GAGNE  /**< Le joueur 2 gagne */
} FinDePartie;

/*
 * MACROS *
 */

#define AUTRE_JOUEUR(i) (i == JOUEUR1 ? JOUEUR2 : JOUEUR1)
#define SYMBOLE_JOUEUR(i) (i == JOUEUR1 ? JOUEUR1_SYM : JOUEUR2_SYM)
#define JOUEUR_SYMBOLE(c) (c == JOUEUR1_SYM ? JOUEUR1 : JOUEUR2)
#define EST_GAGNANT(fin, joueur) (joueur == JOUEUR1 ? fin == J1_GAGNE : fin == J2_GAGNE)
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) < (b) ? (b) : (a))

/*
 * STRUCTURES *
 */

/**
 * @struct EtatSt
 * @brief Représente l'état/position du jeu.
 */
typedef struct EtatSt
{
    Joueur joueur_actuel;                 /**< Joueur qui joue actuellement */
    Case plateau[NB_LIGNES][NB_COLONNES]; /**< Plateau de jeu : la première ligne est en bas du plateau et la première colonne est la plus à gauche */
} Etat;

/**
 * @struct Coup
 * @brief Représente un coup dans le jeu.
 */
typedef struct
{
    int colonne; /**< Numéro de colonne pour le coup */
} Coup;

/**
 * @struct NoeudSt
 * @brief Représente un nœud dans l'arbre de jeu pour l'ordinateur.
 */
typedef struct NoeudSt
{
    Joueur joueur_actuel; /**< Joueur qui a joué pour atteindre ce nœud */
    Coup *coup;           /**< Coup joué par le joueur pour atteindre ce nœud */

    Etat *etat; /**< État du jeu */

    struct NoeudSt *parent;               /**< Nœud parent */
    struct NoeudSt *enfants[LARGEUR_MAX]; /**< Liste des enfants : chaque enfant correspond à un coup possible */
    int nb_enfants;                       /**< Nombre d'enfants dans la liste */

    // FOR MCTS:
    int nb_victoires; /**< Nombre de victoires */
    int nb_simus;     /**< Nombre de simulations */
} Noeud;

/**
 * @struct CoupStatistiques
 * @brief Représente le choix d'un coup et diverses statistiques de l'ordinateur lors de la recherche de ce coup.
 */
typedef struct CoupStatistiques
{
    Coup *coup;         /**< Coup choisi */
    float proba_gagner; /**< La probabilité de gagner en jouant ce coup */
    int nb_simus;       /**< Nombre de simulations qui ont permis de calculer ce coup*/
} CoupStatistiques;

/**
 * @typedef fonctionSimulation
 * @brief Représente le type de fonctions de simulation. Elles renvoient un score pour le joueur donné à partir de l'état donné.
 * @param etat L'état du jeu.
 * @param joueur Le joueur pour lequel on calcule le score.
 * @return int Le score pour le joueur donné. (0 si match nul ou si le joueur a perdu, 1 si le joueur a gagné)
 */
typedef int (*fonctionSimulation)(Etat *etat, Joueur joueur);

/**
 * @typedef fonctionChoixNoeud
 * @brief Représente le type de fonctions de sélection de coup pour un nœud racine donné (les enfants du nœud doivent avoir leurs statistiques à jour). Elles renvoient le noeud du coup à jouer en fonction des statistiques des enfants du nœud.
 * @param noeud Le nœud racine.
 * @return Noeud* Le noeud contenant le coup à jouer.
 */
typedef Noeud *(*fonctionChoixNoeud)(Noeud *noeud);

/**
 * @struct ParametresMCTS
 * @brief Représente les paramètres pour l'algorithme MCTS.
 */
typedef struct ParametresMCTSSt
{
    fonctionSimulation simuler;    /**< Fonction de simulation */
    fonctionChoixNoeud choixNoeud; /**< Fonction de choix du meilleur noeud une fois les simulations terminées */
    float temps_reflexion;         /**< Temps de réflexion pour l'ordinateur */
    float c;                       /**< Paramètre d'exploration */
} ParametresMCTS;
