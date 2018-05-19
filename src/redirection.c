/* redirection.c
 Quatre petites fonction pour effectuer un nombre illimité de redirections des descripteurs d'entrées et sorties standard.
*/
# include "sys.h"

/*Fait appel à freopen pour rediriger les entrées et sorties*/

int redirection(char * fichier, char * mode, _IO_FILE * descripteur)
{
	FILE * rdctn = freopen(fichier, mode, descripteur);
	if (!rdctn)
		return -1 ; //que si le fichier ne peut être ouvert. Attention, freopen ne renvoie pas NULL avec un fichier NULL.
        return 0 ;		
}

/*La fonction qui teste la présence d'un signe. A envisager comme le mapping d'un switch*/

int is_it_symbol(char * mot)
{
	if (strcmp(mot, ">") == 0) 
		return 1 ; 
	else if (strcmp(mot, ">>") == 0)
		return 2 ;
	else if (strcmp(mot, "<") == 0)
		return 3 ;
	return 0 ;
}

/*Essentiel pour réajuster le tableau de deux crans vers la gauche en cas de présence d'autres arguments après le chevron. Cette fonction ne peut pas échouer logiquement donc ne retourne pas de valeurs*/

void new_tab(char * tab_mot[], int i)
{ 	//Tant qu'il y a des arguments dans le vecteur après le nom de fichier, on les décale
	for(i=i ; tab_mot[i+2] != NULL ; i++) 
		tab_mot[i] = tab_mot[i+2] ; 
	tab_mot[i] = NULL ; //Ou on met là ou était le chevron à NULL, ou le mot suivant le dernier de l'itération précédente.
	return ;
}

int to_redirect(char * tab_mot[])
{
	int i, is_sym, is_rd_ok ;
	for(i = 0; tab_mot[i] != 0; i++) { 
		is_sym = is_it_symbol(tab_mot[i]) ; 
		switch (is_sym) {
			case 1 :
				is_rd_ok = redirection(tab_mot[i + 1], "w", stdout) ;
				break ;
			case 2 :
				is_rd_ok = redirection(tab_mot[i + 1], "a", stdout) ;
				break ;
			case 3 :
				is_rd_ok = redirection(tab_mot[i + 1], "r", stdin) ;
				break ;
			default :
				continue ; //recommence avec la prochaine itération		
		}
					//Ne sera appelé que si switch valide un des cas.	
		if (tab_mot[i + 1] == NULL) { //Si le chevron n'est pas suivi d'un argument car rediction() ne renvoit pas d'erreur
			fprintf(stderr, "redirection: you need an argument for the redirection.\n") ;
			exit(-1) ;
		}
						//erreur sur redirection
		if (is_rd_ok < 0) {
			fprintf(stderr, "%s: ", tab_mot[i + 1]);
			perror("") ;
			exit(-1) ;
		}
						//sinon décale le tableau et recommence
		new_tab(tab_mot, i) ; 
			
		return 1 + to_redirect(tab_mot); //appel récursif + compteur pour connaître le nombre de redirections effectués si désiré
		
	}
	return 0 ;
}



# ifdef TEST
int
main(int ac, char * av[]){
  char * out[] = {"Ecrire","dans", "le fichier_out", "au", "lieu", "de", ">", "./exercices/6.6/fichier_out", "stdout", "puis", "prendre", "ce", "fichier", "à la place du stdin et le rediriger encore ailleurs: fichier_out_2"};
  char * in[] = {">>", "./exercices/6.6/fichier_out_2", "<", "./exercices/6.6/fichier_out"};
  int i;

  if (ac > 1){
    fprintf(stderr, "usage: %s, Les tableaux sont déjà renseignés\n", av[0]);
    return 1;
  }

  printf("\nOn redirige les éléments contenus dans le tableau out, ici, sur la sortie standard:\n\n");
  for (i = 0; out[i] != NULL ; i++)
  	printf("%s ", out[i]);
  printf("\n\n");
  
  to_redirect(out);
  for (i = 0; out[i] != NULL ; i++)
  	printf("%s ", out[i]);
  
  to_redirect(in);
  for (i = 0; in[i] != NULL ; i++)
  	printf("%s ", in[i]);

  return 0;
}
# endif
