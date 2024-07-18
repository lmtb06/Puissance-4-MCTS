/*
	Canvas pour algorithmes de jeux à 2 joueurs
	
	joueur 0 : humain
	joueur 1 : ordinateur
			
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// Paramètres du jeu
#define LARGEUR_MAX 9 		// nb max de fils pour un noeud (= nb max de coups possibles)

#define TEMPS 1		// temps de calcul pour un coup avec MCTS (en secondes)

// macros
#define AUTRE_JOUEUR(i) (1-(i))
#define min(a, b)       ((a) < (b) ? (a) : (b))
#define max(a, b)       ((a) < (b) ? (b) : (a))

#define NB_COLONNES 7 // Nombre de colonnes de la grille de jeu
#define NB_LIGNES 6 // Nombre de lignes de la grille de jeu

// Critères de fin de partie
typedef enum {NON, MATCHNUL, ORDI_GAGNE, HUMAIN_GAGNE } FinDePartie;

// Definition du type Etat (état/position du jeu)
typedef struct EtatSt {

	int joueur; // à qui de jouer ? 

	// TODO: à compléter par la définition de l'état du jeu

	/* par exemple, pour morpion: */
	char plateau[NB_LIGNES][NB_COLONNES];	

} Etat;

// Definition du type Coup
typedef struct {
	// TODO: à compléter par la définition d'un coup 
	int colonne;

} Coup;

// Copier un état 
Etat * copieEtat( Etat * src ) {
	Etat * etat = (Etat *)malloc(sizeof(Etat));

	etat->joueur = src->joueur;
	
		
	// TODO: à compléter avec la copie de l'état src dans etat
	
	/* par exemple : */
	int i,j;	
	for (i=0; i< NB_LIGNES; i++)
		for ( j=0; j<NB_COLONNES; j++)
			etat->plateau[i][j] = src->plateau[i][j];
	

	
	return etat;
}

// Etat initial 
Etat * etat_initial( void ) {
	Etat * etat = (Etat *)malloc(sizeof(Etat));
	
	// TODO: à compléter avec la création de l'état initial
	
	/* par exemple : */
	int i,j;	
	for (i=0; i< NB_LIGNES; i++)
		for ( j=0; j<NB_COLONNES; j++)
			etat->plateau[i][j] = ' ';
	
	return etat;
}


void afficheJeu(Etat * etat) {

	// TODO: à compléter

	/* par exemple : */
	int i,j;
	printf("   |");
	for ( j = 0; j < NB_COLONNES; j++) 
		printf(" %d |", j);
	printf("\n");
	printf("--------------------------------");
	printf("\n");
	
	for(i=0; i < NB_LIGNES; i++) {
		printf(" %d |", i);
		for ( j = 0; j < NB_COLONNES; j++) 
			printf(" %c |", etat->plateau[i][j]);
		printf("\n");
		printf("--------------------------------");
		printf("\n");
	}
}


// Nouveau coup 
// TODO: adapter la liste de paramètres au jeu
Coup * nouveauCoup( int j ) {
	Coup * coup = (Coup *)malloc(sizeof(Coup));
	
	// TODO: à compléter avec la création d'un nouveau coup
	
	coup->colonne = j;
	
	return coup;
}

// Demander à l'humain quel coup jouer 
Coup * demanderCoup () {

	// TODO...

	/* par exemple : */
	int j;
	printf(" quelle colonne ? ") ;
	scanf("%d",&j); 
	
	return nouveauCoup(j);
}

// Modifier l'état en jouant un coup 
// retourne 0 si le coup n'est pas possible
int jouerCoup( Etat * etat, Coup * coup ) {

	// On joue le coup sur la première ligne vide de la colonne
	for (int i = NB_LIGNES - 1; i >= 0; i--) {
        if (etat->plateau[i][coup->colonne] == ' ') {
            etat->plateau[i][coup->colonne] = etat->joueur ? 'O' : 'X';
            etat->joueur = AUTRE_JOUEUR(etat->joueur);
            return 1;
        }
    }
    return 0; // colonne pleine	
}

// Retourne une liste de coups possibles à partir d'un etat 
// (tableau de pointeurs de coups se terminant par NULL)
Coup ** coups_possibles( Etat * etat ) {
	
	Coup ** coups = (Coup **) malloc((1+LARGEUR_MAX) * sizeof(Coup *) );
	
	int k = 0;
	
	// TODO: à compléter
	
	/* par exemple */
	int i,j;
	for (int j = 0; j < NB_COLONNES; j++) {
        if (etat->plateau[0][j] == ' ') { // si la colonne n'est pas pleine
            coups[k] = nouveauCoup(j);
            k++;
        }
    }
	/* fin de l'exemple */
	
	coups[k] = NULL;

	return coups;
}


// Definition du type Noeud 
typedef struct NoeudSt {
		
	int joueur; // joueur qui a joué pour arriver ici
	Coup * coup;   // coup joué par ce joueur pour arriver ici
	
	Etat * etat; // etat du jeu
			
	struct NoeudSt * parent; 
	struct NoeudSt * enfants[LARGEUR_MAX]; // liste d'enfants : chaque enfant correspond à un coup possible
	int nb_enfants;	// nb d'enfants présents dans la liste
	
	// POUR MCTS:
	int nb_victoires;
	int nb_simus;
	
} Noeud;


// Créer un nouveau noeud en jouant un coup à partir d'un parent 
// utiliser nouveauNoeud(NULL, NULL) pour créer la racine
Noeud * nouveauNoeud (Noeud * parent, Coup * coup ) {
	Noeud * noeud = (Noeud *)malloc(sizeof(Noeud));
	
	if ( parent != NULL && coup != NULL ) {
		noeud->etat = copieEtat ( parent->etat );
		jouerCoup ( noeud->etat, coup );
		noeud->coup = coup;			
		noeud->joueur = AUTRE_JOUEUR(parent->joueur);		
	}
	else {
		noeud->etat = NULL;
		noeud->coup = NULL;
		noeud->joueur = 0; 
	}
	noeud->parent = parent; 
	noeud->nb_enfants = 0; 
	
	// POUR MCTS:
	noeud->nb_victoires = 0;
	noeud->nb_simus = 0;	
	

	return noeud; 	
}

// Ajouter un enfant à un parent en jouant un coup
// retourne le pointeur sur l'enfant ajouté
Noeud * ajouterEnfant(Noeud * parent, Coup * coup) {
	Noeud * enfant = nouveauNoeud (parent, coup ) ;
	parent->enfants[parent->nb_enfants] = enfant;
	parent->nb_enfants++;
	return enfant;
}

void freeNoeud ( Noeud * noeud) {
	if ( noeud->etat != NULL)
		free (noeud->etat);
		
	while ( noeud->nb_enfants > 0 ) {
		freeNoeud(noeud->enfants[noeud->nb_enfants-1]);
		noeud->nb_enfants --;
	}
	if ( noeud->coup != NULL)
		free(noeud->coup); 

	free(noeud);
}
	

// Test si l'état est un état terminal 
// et retourne NON, MATCHNUL, ORDI_GAGNE ou HUMAIN_GAGNE
FinDePartie testFin( Etat * etat ) {

	// TODO...
	
	int i, j, k;
    char joueurActuel;

    // Vérifier les lignes pour un alignement de 4 jetons
    for (i = 0; i < 6; i++) {
        for (j = 0; j < 4; j++) { // 4 possibilités dans une ligne de 7 pour aligner 4 jetons
            joueurActuel = etat->plateau[i][j];
            if (joueurActuel != ' ' && joueurActuel == etat->plateau[i][j+1] && joueurActuel == etat->plateau[i][j+2] && joueurActuel == etat->plateau[i][j+3]) {
                return joueurActuel == 'O' ? ORDI_GAGNE : HUMAIN_GAGNE;
            }
        }
    }

    // Vérifier les colonnes pour un alignement de 4 jetons
    for (j = 0; j < 7; j++) {
        for (i = 0; i < 3; i++) { // 3 possibilités dans une colonne de 6 pour aligner 4 jetons
            joueurActuel = etat->plateau[i][j];
            if (joueurActuel != ' ' && joueurActuel == etat->plateau[i+1][j] && joueurActuel == etat->plateau[i+2][j] && joueurActuel == etat->plateau[i+3][j]) {
                return joueurActuel == 'O' ? ORDI_GAGNE : HUMAIN_GAGNE;
            }
        }
    }

    // Vérifier les diagonales (montantes et descendantes) pour un alignement de 4 jetons
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 4; j++) {
            joueurActuel = etat->plateau[i][j];
            if (joueurActuel != ' ' && joueurActuel == etat->plateau[i+1][j+1] && joueurActuel == etat->plateau[i+2][j+2] && joueurActuel == etat->plateau[i+3][j+3]) {
                return joueurActuel == 'O' ? ORDI_GAGNE : HUMAIN_GAGNE;
            }
        }
        for (j = 3; j < 7; j++) {
            joueurActuel = etat->plateau[i][j];
            if (joueurActuel != ' ' && joueurActuel == etat->plateau[i+1][j-1] && joueurActuel == etat->plateau[i+2][j-2] && joueurActuel == etat->plateau[i+3][j-3]) {
                return joueurActuel == 'O' ? ORDI_GAGNE : HUMAIN_GAGNE;
            }
        }
    }

    // Vérifier le match nul (si toutes les cases sont remplies)
    for (i = 0; i < 6; i++) {
        for (j = 0; j < 7; j++) {
            if (etat->plateau[i][j] == ' ') {
                return NON; // Le jeu continue
            }
        }
    }

    return MATCHNUL; // Toutes les cases sont remplies sans gagnant
}

/*---------------------*/
int estNonExplore(Noeud *noeud) {
    return noeud->nb_simus == 0;
}

int estTerminal(Etat *etat) {
    return testFin(etat) != NON;
}

Noeud *selectionnerEnfantUCT(Noeud *noeud, double c) {
    Noeud *meilleurEnfant = NULL;
    double meilleurBValeur = -INFINITY;
    double BValeur;
	double mu;
	double signe;
	// on calcule les B-valeur des ses enfants et on choisit le meilleur
    for (int i = 0; i < noeud->nb_enfants; i++) {
        Noeud *enfant = noeud->enfants[i];
		if (estNonExplore(enfant)) // Le premier enfant non exploré
		{
			return enfant;
		}
		
		// sinon on calcule la B-valeur et on choisit le meilleur
		mu = (double)enfant->nb_victoires / (double)enfant->nb_simus;
		signe = (double) enfant->joueur == 1 ? 1 : -1;
        BValeur = signe * mu 
		+ 
		c * sqrt(log((double)noeud->nb_simus) / (double)enfant->nb_simus);
        if (BValeur > meilleurBValeur) {
            meilleurEnfant = enfant;
            meilleurBValeur = BValeur;
        }
    }
    return meilleurEnfant;
}

int estDejaPresent(Noeud *noeud, Coup *coup) {
	for (int i = 0; i < noeud->nb_enfants; i++) {
		if (noeud->enfants[i]->coup->colonne == coup->colonne) {
			return 1;
		}
	}
	return 0;
}

void expand(Noeud *noeud) {
	Coup **coups = coups_possibles(noeud->etat);
	for (int i = 0; coups[i] != NULL; i++) {
		if (!estDejaPresent(noeud, coups[i])) {
			ajouterEnfant(noeud, coups[i]);
		}
	}
	free(coups);
}

double simuler(Noeud *noeud) {
    Etat *etat = copieEtat(noeud->etat);
    while (testFin(etat) == NON) {
        Coup **coups = coups_possibles(etat);
        int nb_coups = 0;
        while (coups[nb_coups] != NULL) nb_coups++;
        jouerCoup(etat, coups[rand() % nb_coups]);
        free(coups);
    }
    double resultat = (testFin(etat) == ORDI_GAGNE) ? 1.0 : 0.0;
    free(etat);
    return resultat;
}

void retropropager(Noeud *noeud, double resultat) {
    while (noeud != NULL) {
        noeud->nb_simus++;
        if (noeud->joueur == 1) { // Si le joueur est l'ordinateur, on ajoute le résultat de la simulation
            noeud->nb_victoires += resultat;
        }
        noeud = noeud->parent;
    }
}

Noeud *choisirMeilleurNoeudFils(Noeud *racine) {
    int maxSimus = -1;
    Noeud *meilleurNoeudFils = NULL;
    for (int i = 0; i < racine->nb_enfants; i++) {
        if (racine->enfants[i]->nb_simus > maxSimus) {
            meilleurNoeudFils = racine->enfants[i];
            maxSimus = racine->enfants[i]->nb_simus;
        }
    }
    return meilleurNoeudFils;
}

/*---------------------*/

// Calcule et joue un coup de l'ordinateur avec MCTS-UCT
// en tempsmax secondes
void ordijoue_mcts(Etat * etat, int tempsmax) {

	clock_t tic, toc;
	tic = clock();
	int temps;

	Coup ** coups;
	Coup * meilleur_coup ;
	double c = sqrt(2); // pour UCT
	
	// Créer l'arbre de recherche
	Noeud * racine = nouveauNoeud(NULL, NULL);	
	racine->etat = copieEtat(etat);
	int nb_simus = racine->nb_simus;

	// créer les premiers noeuds si la racine n'a pas d'enfants
	if (racine->nb_enfants == 0) {
		coups = coups_possibles(racine->etat); 
		int k = 0;
		Noeud * enfant;
		while ( coups[k] != NULL) {
			enfant = ajouterEnfant(racine, coups[k]);
			k++;
		}
		free (coups);
	}

	int iter = 0;
	do {
	
	
		Noeud *noeud = racine;
		int terminal;
        // Sélection
        while ( noeud->nb_enfants != 0 && !(terminal = estTerminal(noeud->etat))) {
            noeud = selectionnerEnfantUCT(noeud, c);
        }
        if (!terminal) {
        	// Expansion du noeud
            expand(noeud);
			// Selection d'un noeud à explorer parmi les noeuds non explorés
			Noeud** enfantsNonExplores = (Noeud**)malloc(noeud->nb_enfants * sizeof(Noeud*)); // tableau des enfants non explorés
			int nbEnfantsNonExplores = 0;
			// on ajoute les enfants non explorés au tableau
			for (int i = 0; i < noeud->nb_enfants; i++) {
				if (estNonExplore(noeud->enfants[i])) {
					enfantsNonExplores[nbEnfantsNonExplores] = noeud->enfants[i];
					nbEnfantsNonExplores++;
				}
			}
			
			noeud = enfantsNonExplores[rand() % nbEnfantsNonExplores]; // on choisit un enfant aléatoirement parmi les enfants non explorés
			free(enfantsNonExplores);
        }

        // Simulation
        double resultat = simuler(noeud);
        // Rétropropagation
        retropropager(noeud, resultat);

	
		toc = clock(); 
		temps = (int)( ((double) (toc - tic)) / CLOCKS_PER_SEC );
		iter ++;
	} while ( temps < tempsmax );

	// choisir le meilleur coup
	Noeud * noeudChoisi = choisirMeilleurNoeudFils(racine);
    meilleur_coup = noeudChoisi->coup;
	// Affichage le nombre de simulations réalisées pour calculer ce coup
	printf("Nombre de simulations réalisées : %d\n", racine->nb_simus - nb_simus);
	
	// Estimation de la probabilité de victoire pour l'ordinateur
	double proba_victoire = (double)noeudChoisi->nb_victoires / noeudChoisi->nb_simus;
	printf("Estimation de la probabilité de victoire pour l'ordinateur : %.2f%%\n", proba_victoire * 100);
	
	/* fin de l'algorithme  */ 
	
	// Jouer le meilleur premier coup
	jouerCoup(etat, meilleur_coup );
	
	// Penser à libérer la mémoire :
	freeNoeud(racine);
}

int main(void) {

	Coup * coup;
	FinDePartie fin;
	
	// initialisation
	Etat * etat = etat_initial(); 
	
	// Choisir qui commence : 
	printf("Qui commence (0 : humain, 1 : ordinateur) ? ");
	scanf("%d", &(etat->joueur) );
	
	// boucle de jeu
	do {
		printf("\n");
		afficheJeu(etat);
		
		if ( etat->joueur == 0 ) {
			// tour de l'humain
			
			do {
				coup = demanderCoup();
			} while ( !jouerCoup(etat, coup) );
									
		}
		else {
			// tour de l'Ordinateur
			
			ordijoue_mcts( etat, TEMPS );
			
		}
		
		fin = testFin( etat );
	}	while ( fin == NON ) ;

	printf("\n");
	afficheJeu(etat);
		
	if ( fin == ORDI_GAGNE )
		printf( "** L'ordinateur a gagné **\n");
	else if ( fin == MATCHNUL )
		printf(" Match nul !  \n");
	else
		printf( "** BRAVO, l'ordinateur a perdu  **\n");
	return 0;
}
