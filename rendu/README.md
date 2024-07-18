# Guide d'utilisation:
- Pour compiler le programme, il suffit de lancer la commande `make` dans le terminal.
Si vous voulez supprimer les fichiers générés par la compilation, vous pouvez lancer la commande `make clean`.
- Pour compiler avec l'option d'optimisation `-O3`, il suffit de lancer la commande `make optim`.
- Pour lancer le programme, il suffit de lancer la commande `./jeu` dans le terminal.
- Le fichier `main.c` contient le programme principal. Et différentes fonctions de tests pour les différentes options de l'algo MCTS-UCT.
- Les fichiers .h contiennent les prototypes des fonctions et les structures de données utilisées. Avec les commentaires pour expliquer le rôle de chaque fonction.
- Les fichiers .c contiennent les implémentations des fonctions déclarées dans les fichiers .h correspondants.

# Réponses:
## 2. Testez différentes limites de temps pour l’ordinateur et comparez les résultats obtenus. A partir de quel temps de calcul l’ordinateur vous bat à tous les coups ?
Après test, on remarque que l'ordinateur commence à gagner à tous les coups à partir de 0.1 seconde. En dessous de 0.1 seconde, l'ordinateur ne gagne pas à tous les coups, mais il gagne la plupart du temps.

## 3. Implémentez l’amélioration des simulations consistant à toujours choisir un coup gagnant lorsque cela est possible. Comparez la qualité de jeu de cette nouvelle version avec la précédente et expliquez à quoi cela est dû.
Après test, on remarque que l'implémentation de l'amélioration des simulations consistant à toujours choisir un coup gagnant lorsque cela est possible, améliore la qualité de jeu de cette nouvelle version. Cela est dû au fait que plutôt que de choisir un coup aléatoire, on choisit un coup gagnant si cela est possible evitant ainsi de faire des coups inutiles, ce qui permet de faire plus de simulations et donc d'avoir une meilleure qualité de jeu.

## 4.  Si vous travaillez en C, quelle est l’utilité ici de compiler avec gcc -O3 plutôt qu’avec les options par défaut ? Donnez des exemples illustratifs.
L'utilité de compiler avec gcc -O3 plutôt qu’avec les options par défaut est que cela permet d'optimiser le programme en générant du code plus rapide et moins volumineux, notamment en optimisant les boucles, en utilisant des registres supplémentaires et en supprimant les instructions inutiles. Cela permet d'obtenir un programme plus rapide et plus efficace. Et ainsi permet de gagner du temps lors de l'exécution du programme. Par exemple, en faisant cela une execution qui permettait à l'IA de faire 30 simulation en 0.001 seconde, avec l'optimisation -O3, l'IA peut en faire 55.

## 5. Comparez les critères "max" et "robuste" pour choisir le coup à jouer en fin d’algorithme. Conduisent-ils souvent à des coups différents ? Lequel paraît donner la meilleure performance ?
Le critère "max" a des coups assez variés, comme on peut le voir dans les tests (au niveau des statistiques des colonnes jouées). Par contre, le critère "robuste" a des coups plus concentrés, ce qui est un avantage car cela permet de se concentrer sur les coups qui ont le plus de chance de gagner. Le critère "robuste" donne la meilleure performance car il permet de se concentrer sur les coups qui ont le plus de chance de gagner. (voir les statistiques des colonnes jouées dans les tests pour plus de détails)

## 6. Donnez une estimation du temps de calcul nécessaire pour jouer le premier coup avec l’algorithme Minimax (sans alpha-beta ni limitation de profondeur).
Ici avec l'algo de base on est en moyenne à 60000 simulations par seconde avec l'optimisation -O3 sur mon PC. Donc on peut estimer que pour une seule branche de l'arbre de jeu, il faudra environ 1/60000 = 0,00001666666 secondes. Or on a 7 branches possibles pour 6 de profondeur (7^6 possibilités), donc on peut estimer que pour jouer le premier coup avec l’algorithme Minimax (sans alpha-beta ni limitation de profondeur), il faudra environ 7^6 * 0,00001666666 = environ 1,96 secondes.