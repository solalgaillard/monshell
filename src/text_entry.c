/* text_entry.c
 Permet de traiter le texte entré dans le terminal à l'aide de caractères de contrôles comme les flêches pour naviguer sur le texte, le backspace et le delete pour supprimer des caractères de la chaîne entrée mais surtout répond à la question de l'exercice, c'est-à-dire, produit l'auto-complétion avec la touche tab du texte en regard des fichiers ou répertoires présents sur le chemin du fichier ou répertoire représenté par la chaîne à l'écran.
*/
#include "sys.h"

#define ASCII_ESC 27
#define PROMPT "%s? ",cwd

/*Pour insérer un caractère dans une chaîne*/
void insert(char buffer[], int c, int place)
{
	int i ;
	for (i = strlen(buffer); i >= place; i--)
      		buffer[i+1] = buffer[i];
 
	buffer[place] = c;
	
	return ;
}

/*Pour supprimer un caractère dans une chaîne*/
void delete(char buffer[], int place)
{
	if (place < 0) return ;
	int i ;
	for (i = place ; i < strlen(buffer) ; i++)
      		buffer[i] = buffer[i+1];
      	return ;
}

/*Faire apparaître le buffer sur la ligne 
  tout en gardant le curseur au bon endroit*/
int refresh_bufferline(char buffer[], int num, int offset)
{
	if (num)
		printf("%c[%dD", ASCII_ESC, num);
	printf("%c[0K", ASCII_ESC);
	printf("%s", buffer);
	
	while (++num < strlen(buffer)+offset)
		printf("\b");
	return 0;
}


/*index de la dernière occurence du caractère 
  dans la chaîne, limite borné à 0 pour absence*/
int last_occur(char buffer[], int from_i , int key)
{
	while (buffer[from_i] != key) {
		if (from_i == 0)
			return 0;
		from_i-- ;
	}
	return from_i+1;	
}

/*index de la première occurence du caractère 
  dans la chaîne, limite borné à la longueur 
  de la chaîne pour absence*/
int next_occur(char buffer[], int from_i , int key)
{
	while (buffer[from_i] != key) {
		if (from_i == strlen(buffer))
			return strlen(buffer);
		from_i++ ;
	}
	return from_i;	
}

/*fonction qui trouve le nom de fichier ou de répertoire
grâce à opendir, readdir et la structure dirent*/

int find_directory(char chemin[], char * tab_dir[])
{
	char * lookedup_wd ; //mot comparé avec les noms des répertoires
	char tmp_dir[512] ;  //buffer
	char current_dir[512] ; //contient répertoire à ouvrir
	char * ptr_sub; //nécessaire pour valider présence du mot
	int cutoff, i ; //cutoff pour découper, i multi-usage et pour compter le nombre de mots
	DIR *dp, *dp_test;
	struct dirent *ep; 
	
	/*libère la mémoire si tableau a déjà été rempli.*/
	for (i = 0 ; tab_dir[i] != NULL ; i++)
		free(tab_dir[i]); 
		
	tab_dir[0] = NULL; //pour éviter d'essayer de libérer si erreur

	
	
		
	/* Obtenir le nom à chercher et modifier les chaînes, 
	   pour diviser entre le chemin et mot recherché. */
	cutoff = last_occur(chemin, strlen(chemin)-1, 47);
	
	/*si aucun slash est contenu dans la chaîne*/
	if (!cutoff) {
	
		/*le mot a cherché est l'équivalent de chemin*/
		lookedup_wd = chemin;
		strcpy(current_dir, "./"); //le répertoire est celui courant
	} else {
		lookedup_wd = chemin + cutoff; /*le mot cherché est à 
										partir du dernier slash*/
		i = *chemin ; //Chemin absolu?
		chemin[cutoff-1] = 0; /*on enlève le mot à cherché du 
								chemin et donc dernier slash*/
		if (i == '/')	//chemin absolu + remettre slash
			snprintf (current_dir, 512, "%s/", chemin); 
		else //chemin relatif + remettre slash
			snprintf (current_dir, 512, "./%s/", chemin); 
	}
		
	
	i = 0 ; //remettre le compteur de fichier à zéro
	
	
	dp = opendir(current_dir);
	 
	if (dp != NULL) {
		while ((ep = readdir(dp))) {
			/*si les dossiers sont "." ou "..", on oublie*/
			if (strcmp(".", ep->d_name) && strcmp("..", ep->d_name)){
				
				/*Valide si la 1ere chaîne est inclue dans la 
				  2eme depuis le début (arithmétique de pointeurs).*/
				ptr_sub = strstr(ep->d_name, lookedup_wd);
				if (ptr_sub && !(ptr_sub - ep->d_name)) {
									
					if (*current_dir == '/') 			
						snprintf (tmp_dir, 512, "%s%s", current_dir, ep->d_name);
					else //si chemin non absolu, on enlève l'appel sur le répertoire "./"
						snprintf (tmp_dir, 512, "%s%s", (current_dir+2), ep->d_name);
					
					/*Test si entrée trouvée est répertoire, 
					  si oui on ajoute le slash de fin*/
					if ((dp_test = opendir(tmp_dir))) {
						strcat (tmp_dir,"/");
						closedir(dp_test);
					}
								
					tab_dir[i] = strdup(tmp_dir);
					i++ ;
				}
		}	}
	
	tab_dir[i] = NULL; //fin de séquence
	closedir(dp);

	} else {
		perror (" opendir");
		return -1 ;
	}
	
	/*Renvoie le nombre de 
	  fichiers/répertoires trouvés*/
	return i; 
		
}

/*fonction exemple dans man 3 qsort 
  pour comparer des chaînes*/

static int cmpstringp(const void *p1, const void *p2)
{
    /* The actual arguments to this function are "pointers to
       pointers to char", but strcmp(3) arguments are "pointers
       to char", hence the following cast plus dereference */

   return strcmp(* (char * const *) p1, * (char * const *) p2);
}


/*fonction principale qui gère 
  le fonctionnement des touches claviers*/
int text_entry(char buffer[], char cwd[]) 
{	
	/* Met le terminal en mode canonique, 
	   enlève echo des caractères */
	struct termios original, changed;
	tcgetattr(0, &original);   //récupérer les parmètres io de stdin 
	changed = original;	//sauvegarde des paramètres originaux
	changed.c_lflag &= ~(ICANON | ECHO);  //avec flag mode canonique et plus de print
	tcsetattr (0, TCSANOW, &changed); //applique les paramètres

	char tmp[1024] ;
	char * tab_dir[1024];	
	int c, i = 0, y, tab_pres, found_dir, inner_it, idx_last, idx_next ;
	
	while (1) {
		c = getchar(); //En code ascii
		switch (c) {
			case 4 : //EOT
				//remet le terminal en mode original
				tcsetattr (0, TCSANOW, &original); 
				return 0;
			
			case 9 : //tab
				if(!tab_pres) { //si tab n'a pas déjà été pressé
							
					/*regarde où est placé le curseur*/
					if (i)
						idx_last = last_occur(buffer, i-1, 32);
					else
						idx_last = 0 ;
					idx_next = next_occur(buffer, i, 32);
				
					/*chaîne où est placé le curseur, 
					  divisé par des espaces, recopiée */
					strcpy(tmp, buffer+idx_last);
					tmp[idx_next-idx_last] = 0;
					
					/*cherche un fichier ou répertoire 
					  à partir de cette chaîne*/
					found_dir = find_directory(tmp, tab_dir);
					
					if (found_dir < 0) {
						tcsetattr (0, TCSANOW, &original); 
						return -1;
					}
	
				}
				/*Si tab a déjà été pressé, donner 
				  la liste des fichiers qui peuvent 
				  correspondre avec l'entrée en vigueur*/
				else {  
				
					/*trier par ordre alphabétique*/
					qsort(tab_dir, found_dir, sizeof(char *), cmpstringp) ;
					
					printf("\n"); //Move to next line
				
					/*Imprime par rang de 6*/
					for (inner_it = 0 ; tab_dir[inner_it] != NULL ; inner_it++) {
						
						/*cherche dernière instance d'un slash, 
					 	  mais exclure dernier caractère tant 
					 	  qu'il est slash (si répertoire) */
						
						y = strlen(tab_dir[inner_it]) - 1 ;
						while (tab_dir[inner_it][y] == '/')
							y--;
							
						idx_last = last_occur(tab_dir[inner_it], y, 47); 
					
						/*imprimer seulement la partie après slash*/				
						printf("%s\t", tab_dir[inner_it]+idx_last); 
						if (inner_it && inner_it % 6 == 0)
							printf("\n");
					}
				
					/*réimprime le buffer (ligne principale) à la ligne
					 et le prompt passé en argument*/
					printf("\n");
					printf(PROMPT);
					refresh_bufferline(buffer, 0, -i+1) ;
		
				}
			
				/*Si un fichier ou répertoire a été trouvé*/
				if (found_dir == 1) {				
				
					strcpy(tmp, buffer);	//copie le buffer
					tmp[idx_last] = 0;	/*enlève la partie sur laquelle 
					                     on a appelé found_dir et qui va 
					                     être remplacé*/
				
					/*recompose la chaîne du buffer*/
					sprintf(buffer, "%s%s%s", tmp, tab_dir[0], (tmp + idx_next));
				
					/*garde le curseur au bon endroit*/
					
					/*replace l'offset par la soustraction 
					  de l'index sur le mot où est curseur 
					  moins la nouvelle longueur du mot.*/
					y = i - idx_last + 1 - strlen(tab_dir[0]);
					
					/*réinitialise tab_pres et affiche la ligne*/
					tab_pres = refresh_bufferline(buffer, i, y) ;	
					/*met à jour i*/
					i = idx_last + strlen(tab_dir[0]);
		
				}
			    
			    /*si plusieurs entrées, 
			      on attend une deuxième touche tab*/
				else if (found_dir > 1)
					tab_pres = 1 ;
			
			
				break;
		
		
			case 126 : //delete
				/*supprime le caractère où se situe le curseur*/
				delete(buffer, i);
			
				tab_pres = refresh_bufferline(buffer, i, 1) ;
				break;
		
			case 127 : //backspace
				/*supprime le caractère précédant le curseur*/
				if (i > 0) {
					delete(buffer, i-1);

					tab_pres = refresh_bufferline(buffer, i, 2) ;
					i--;
				}
				break;
		
		
			case 27 : //Arrows
			
				if (getchar() == 91) {
					c = getchar() ;
				
					if (c == 68 && (i > 0)) { //gauche
						printf("\b"); //déplace à gauche
						i-- ;
					}
					
					else if (c == 67 && (i < strlen(buffer))) { //droite
						printf("\x1b[1C"); //déplace à droite
						i++ ;
					}
			
				tab_pres = 0 ;
			
				}
				break;
		
			case 10 : //newline
				printf("\n");
				tcsetattr (0, TCSANOW, &original); 
				return 1; //envoie la ligne à être traitée

			default :
			
				insert(buffer, c, i); //recopie le caractère dans le buffer
				tab_pres = refresh_bufferline(buffer, i, 0) ;
				i++; //déplace le curseur
		}
	}

	tcsetattr (0, TCSANOW, &original); 
	return -1;
}



# ifdef TEST
int main(int ac, char * av[])
{
	if (ac > 1) {
		fprintf(stderr, "usage: %s entrée de texte seulement, pas d'arguments\n", av[0]);
    	return 1;
	}
	
	char buffer[1024];
	buffer[0] = 0;
	
	printf("Ecrivez des chaînes, utiliser des caractères d'éditions comme delete, backspace, arrows mais aussi tab pour compléter les mots.\n? "); 
	
	while(text_entry(buffer, "")!=0) {
		printf("\nVous avez rentré la chaîne de caractère suivante: %s\n\n? ", buffer); 
		buffer[0] = 0;
	}

	return 0;
}
# endif
