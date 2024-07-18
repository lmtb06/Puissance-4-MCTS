/**
 * @file fonctions.h
 * @brief Fichier contenant les des définitions des fonctions nécessaires pour le jeu.
 */
#pragma once

#include "structs.h"

/**
 * @brief Renvoie un état initialisé avec le joueur qui commence comme joueur actuel.
 *
 * @param joueur_qui_commence Le joueur qui commence la partie.
 * @return Etat* Un pointeur vers l'état initialisé.
 */
Etat *etat_initial(Joueur joueur_qui_commence);

/**
 * @brief Renvoie une copie allouée dynamiquement de l'état source.
 *
 * @param src L'état source à copier.
 * @return Etat* Un pointeur vers la copie de l'état source.
 */
Etat *copieEtat(Etat *src);

/**
 * @brief Renvoie la ligne de la première case libre dans la colonne donnée, ou -1 si la colonne est pleine.
 *
 * @param etat L'état du jeu.
 * @param colonne Le numéro de la colonne.
 * @return int La ligne de la première case libre ou -1 si la colonne est pleine.
 */
int lignePremiereCaseLibre(Etat *etat, int colonne);

/**
 * @brief Joue un coup pour le joueur actuel dans l'état donné.
 * Le joueur actuel est modifié dans l'état une fois le coup joué.
 *
 * @param etat L'état du jeu.
 * @param coup Le coup à jouer.
 * @return int 0 si le coup n'est pas possible, 1 sinon.
 */
int jouerCoup(Etat *etat, Coup *coup);

/**
 * @brief Renvoie un coup initialisé avec le numéro de colonne.
 *
 * @param colonne Le numéro de la colonne.
 * @return Coup* Un pointeur vers le coup initialisé.
 */
Coup *nouveauCoup(int colonne);

/**
 * @brief Renvoie un tableau de pointeurs de coups possibles à partir de l'état donné (le dernier pointeur est NULL).
 *
 * @param etat L'état du jeu.
 * @return Coup** Un tableau de pointeurs de coups possibles.
 */
Coup **coups_possibles(Etat *etat);

/**
 * @brief Renvoie un nouveau noeud obtenu à partir du parent en jouant le coup donné. Utiliser nouveauNoeud(NULL, NULL) pour créer la racine.
 *
 * @param parent Le parent du noeud.
 * @param coup Le coup à jouer.
 * @return Noeud* Un pointeur vers le nouveau noeud.
 */
Noeud *nouveauNoeud(Noeud *parent, Coup *coup);

/**
 * @brief Ajoute un enfant au parent et renvoie l'enfant créé.
 *
 * @param parent Le parent du noeud.
 * @param coup Le coup à jouer.
 * @return Noeud* Un pointeur vers le nouvel enfant.
 */
Noeud *ajouterEnfant(Noeud *parent, Coup *coup);

/**
 * @brief Libère la mémoire associée à un noeud et tous ses enfants.
 *
 * @param noeud Le noeud à libérer.
 */
void freeNoeud(Noeud *noeud);

/**
 * @brief Libère la mémoire à une liste de coups possibles produite par coups_possibles.
 * @param coups_possibles La liste de coups à libérer.
 */
void freeListeCoupsPossibles(Coup **coups_possibles);

/**
 * @brief Vérifie un alignement de 4 pions de même symbole à partir d'une position donnée.
 *
 * @param etat L'état du jeu.
 * @param ligne La ligne de départ.
 * @param colonne La colonne de départ.
 * @param deltaLigne Le déplacement de ligne.
 * @param deltaColonne Le déplacement de colonne.
 * @return int 1 si un alignement est trouvé, 0 sinon.
 */
int verifierAlignement4(Etat *etat, int ligne, int colonne, int deltaLigne, int deltaColonne);

/**
 * @brief Teste si l'état est un état terminal.
 *
 * @param etat L'état du jeu.
 * @return FinDePartie La valeur correspondant à l'état terminal (NON, MATCHNUL, J1_GAGNE ou J2_GAGNE).
 */
FinDePartie testFin(Etat *etat);

/**
 * @brief Selectionne récursivement à partir du nœud de base, le nœud avec la plus grande B-valeur jusqu’à un nœud terminal ou un avec un fils non développé
 * @param noeud Le nœud de départ
 * @param c Le paramètre d'exploration UCT
 * @param joueurOrdi Le joueur pour lequel l'ordinateur doit jouer.
 */
Noeud *selectionnerNoeud(Noeud *noeud, float c, Joueur joueurOrdi);

/**
 * @brief Calcul et renvoie le CoupStatistiques correspondant au meilleur coup à jouer en utilisant l'algorithme MCTS-UCT
 *
 * @param etat L'état du jeu.
 * @param simuler La fonction de simulation.
 * @param choixCoups La fonction de choix du meilleur coup une fois les simulations terminées.
 * @param tempsMax Le temps de calcul en secondes.
 * @param joueurOrdi Le joueur pour lequel l'ordinateur doit jouer.
 * @param c Le paramètre d'exploration UCT.
 * @return CoupStatistiques* Un pointeur vers le CoupStatistiques correspondant au meilleur coup.
 */
CoupStatistiques *meilleurCoupMCTS(Etat *etat, fonctionSimulation simuler, fonctionChoixNoeud choixCoups, float tempsMax, Joueur joueurOrdi, float c);

/**
 * @brief Libère la mémoire associée à un CoupStatistiques.
 * @param coupStatistiques Le CoupStatistiques à libérer.
 */
void freeCoupStatistiques(CoupStatistiques *coupStatistiques);

/**
 * @brief Simule une partie à partir de l'état donné et renvoie le score pour le joueur donné. à chaque étape, un coup est choisi aléatoirement parmi les coups possibles.
 *
 * @param etat L'état du jeu.
 * @param joueur Le joueur pour lequel on calcule le score.
 * @return int Le score pour le joueur donné. (0 si match nul ou si le joueur a perdu, 1 si le joueur a gagné)
 */
int simulerAleatoire(Etat *etat, Joueur joueur);

/**
 * @brief Simule une partie à partir de l'état donné et renvoie le score pour le joueur donné. à chaque étape, on choisit le coup gagnant si possible, sinon on choisit aléatoirement parmi les coups possibles.
 */
int simulerCoupGagnant(Etat *etat, Joueur joueur);

/**
 * @brief Renvoie le nœud contenant le coup à jouer en fonction des statistiques des enfants du nœud.
 * Ici on choisit le nœud avec le plus de victoires.
 * @param noeud Le nœud racine.
 * @return Noeud* Le nœud contenant le coup à jouer.
 */
Noeud *choixMaxRecompense(Noeud *noeud);

/**
 * @brief Renvoie le nœud contenant le coup à jouer en fonction des statistiques des enfants du nœud.
 * Ici on choisit le nœud avec le plus de simulations.
 * @param noeud Le nœud racine.
 * @return Noeud* Le nœud contenant le coup à jouer.
 */
Noeud *choixRobuste(Noeud *noeud);

/**
 * @brief Affiche le plateau de jeu.
 *
 * @param etat L'état du jeu.
 */
void afficheJeu(Etat *etat);

/**
 * @brief Affiche les statistiques d'un coup.
 * @param coupStatistiques Le CoupStatistiques à afficher.
 */
void afficheCoupStatistiques(CoupStatistiques *coupStatistiques);

/**
 * @brief Demande un coup à jouer à l'utilisateur.
 * @return Coup* Le coup choisi.
 */
Coup *demanderCoup();

/**
 * @brief Lance une partie entre un humain et l'ordinateur.
 * @param parametresIA Les paramètres pour l'ordinateur.
 */
void humainVsIA(ParametresMCTS *parametresIA);

/**
 * @brief Lance une partie entre deux humains.
 */
void humainVsHumain();

/**
 * @brief Lance une partie entre deux ordinateurs. et renvoie le résultat de la partie.
 * @param parametres1 Les paramètres pour le premier ordinateur.
 * @param parametres2 Les paramètres pour le deuxième ordinateur.
 * @return FinDePartie La valeur correspondant à l'état terminal (NON, MATCHNUL, J1_GAGNE ou J2_GAGNE).
 */
FinDePartie IAVsIA(ParametresMCTS *parametres1, ParametresMCTS *parametres2);