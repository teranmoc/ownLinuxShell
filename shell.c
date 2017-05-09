/* TP6 Interpréteur de commande
Jérémie Décome
L3 INFORMATIQUE - TD5
jeudi 14 janvier 2015
Exercice 5 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdbool.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>

#define DIM 256 // taille totale du buffer d'entrée
#define MAX 10 // nbre de "mots" maximum


int main(int argc, char *argv[])
{
	char *commande = malloc(sizeof(char) * DIM);
	char *tab[MAX];
	char *prog2[MAX];
	const char *sep = " ";
	char *p = NULL;
	int c, i = 0, taille, p2 = -1;
	int j, k, l;
	int fichier = -1; // contiends le descripteur du fichier
	int save = -1;
	int z; // index temporaire pour essai
	bool background = false, out = false, in = false, tube = false;
	memset(tab, 0, sizeof(tab));
	memset(prog2, 0, sizeof(prog2));
	printf("Bash interpréteur de commande simple\n");
	printf("Attention : ne gère pas les commandes internes\n");
	do
	{
		printf("> ");
		fgets(commande, DIM, stdin);
		commande[strlen(commande) - 1] = 0; // retire le retour chariot de la chaine (nécessaire pour détecter le exit)
		if(strcmp(commande, "exit") == 0) // controle que l'ordre d'arret n'est pas transmis
			break;
		
		// analyse de la ligne
		p = strtok(commande, sep); // première partie
		while(p != NULL)
		{
			if(i < MAX) // nbre limite d'argument
				tab[i] = strdup(p);
			else
				break;
			i++;
			p = strtok(NULL, sep); // on travaille toujours sur la chaine commande
		}
		tab[i] = NULL; // à la fin, i contient le nbre d'arguments effectif

		for(j = 0; j < i; j++)
		{
			k = j; // sauvegarde de j
			k++; // k pointe maintenant sur le fichier
			if(strcmp(tab[j], ">") == 0) // redirection du flux de sortie (réécriture complet d'un fichier)
			{
				fichier = open(tab[k], O_CREAT | O_TRUNC | O_WRONLY, 0664);
				out = true;
				break; // on sort de la boucle, pas besoin d'aller voir plus loin
			}
			if(strcmp(tab[j], ">>") == 0) // redirection du flux de sortie (écriture en fin de fichier)
			{
				fichier = open(tab[k], O_CREAT | O_APPEND | O_WRONLY, 0664);
				out = true;
				break;
			}
			if(strcmp(tab[j], "<") == 0) // redirection du flux d'entrée
			{
				fichier = open(tab[k], O_RDONLY);
				in = true;
				break;
			}
			if(strcmp(tab[j], "|") == 0) // pipe
			{
				tube = true; // active la gestion du pipe
				p2 = j + 1; // index sur le programme 2 (la suite est les arguments du programme)
				tab[j] = NULL;
				break;
			}
		}
		// détecte le & final (toujours à la fin)
		if(strcmp(tab[i - 1], "&") == 0)
		{
			tab[i - 1] = NULL;
			background = true; // l'exécution se passe en arrière plan
		}
		if(tube) // gestion du pipe (premier pipe)
		{
			int m, arg = 1; // index
			prog2[0] = tab[p2];
			tab[p2] = NULL;
			p2++;
			// lecture des arguments du 2ème programme
			for(m = p2; m < i; m++)
			{
				prog2[arg] = tab[m];
				arg++;
				tab[m] = NULL;
			}
			prog2[m] = NULL;
		}
		
		// lancement du programme
		pid_t prog = fork();
		if(prog == 0) // fils : exécute toute la chaine
		{
			if(fichier != -1)
			{
				tab[j] = NULL;
				tab[k] = NULL; // désactivation du fichier dans le tableau d'argument
				if(out)
					close(1); // on ferme la sortie standard
				if(in)
					close(0); // on ferme l'entrée standard
				dup(fichier);
				close(fichier); // ferme le descripteur du fichier (dupliqué dans 1 ou 0)
			}
			if(tube)
			{
				int t[2];
				pipe(t);
				pid_t exec = fork();
				if(exec == 0) // exécute le 1ere programme
				{
					close(1);
					dup(t[1]);
					close(t[0]);
					close(t[1]);
					execvp(tab[0], tab);
				}
				else // le père exécute le 2e sans l'attendre
				{
					close(0);
					dup(t[0]);
					close(t[0]);
					close(t[1]);
					execvp(prog2[0], prog2);
				}
			}
			else
			{
				execvp(tab[0], tab);
			}
			
		}
		else // père
		{
			if(background) // le père affiche le numéro du PID et rend la main pour faire autre chose
				printf("[BG] %d\n", prog);
			else // sinon le père attends que le fils d'exécution se finisse
			{
				wait(NULL);
			}
		}
		// réinitialise les variables
		memset(tab, 0, sizeof(tab));
		memset(prog2, 0, sizeof(prog2));
		i = 0; // repositionne l'index
		in = false;
		out = false;
		tube = false;
		p2 = -1;
		fichier = -1;
	}
	while(1);
	free(commande);
	return 0;
}
