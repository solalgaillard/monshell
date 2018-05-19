/* co-main.c
Shell de base avec redirection, fonction interne moncd, exit, listing du repértoire courant à chaque prompt, et pipes
*/
# include "sys.h"

enum 
{
	MaxLigne = 1024,		// longueur max d'une ligne de commandes
	MaxPipe = MaxLigne / 8,		// nbre max de commandes liés par des pipes 
	MaxMot = MaxLigne / 2,		// nbre max de mot dans un pipe
	MaxDirs = 100,			// nbre max de repertoire dans PATH
	MaxPathLength = 512,		// longueur max d'un nom de fichier
};		

int text_entry(char [], char []);

int errorcheck(int, char *);

int decouper(char *, char *, char **, int); 

int moncd (int, char*[]) ;

int to_redirect(char *[]);

int init_moncd(char *[], int);

int init_pipe(int, int, int *) ;

int pipe_parent(int, int *) ;

void pipe_child(int, int, int *, int) ;

# define PROMPT "%s? ",cwd

int main(int argc, char * argv[])
{ 
	char cwd[1024];
	char ligne[MaxLigne];
	char pathname[MaxPathLength];
	char * pipe_seg[MaxPipe];
	char * mot[MaxMot];
	char * dirs[MaxDirs];
	int len_pipe, opened_pipe, fd[2], i_p, i, save_stdout, len_mot, is_amp, tmp ;
	int exit_prog = 0 ; //toggle pour exit

	/* Decouper UNE COPIE de PATH en repertoires */
	decouper(strdup(getenv("PATH")), ":", dirs, MaxDirs);
	
      	getcwd(cwd, sizeof(cwd)) ; //obtenir le répertoire courant
      	
	/* Lire et traiter chaque ligne de commande */
	for(printf(PROMPT); !exit_prog && text_entry(ligne, cwd) != 0 ; printf(PROMPT)) { 
		
		len_pipe = decouper(ligne, "|", pipe_seg, MaxPipe) - 1; //Découpe la ligne en différentes commandes liées par des pipes
							//Tant qu'il y a des commandes liées
		for(i_p = 0 ; i_p < len_pipe ; i_p++) { 
			
			opened_pipe = init_pipe(i_p+1, len_pipe, fd) ;	//Toggle pour pipe	
			if (opened_pipe<0) //erreur, on recommence
				continue ;
				
			/* découpe le segment pipe en commandes et arguments */
			len_mot = decouper(pipe_seg[i_p], " \t\n", mot, MaxMot) - 1;
            
			if (mot[0] == 0) 
            			continue;       	// ligne vide, on recommence


			is_amp = 0 ;
			for(i = 0; mot[i] != 0; i++) {	//lance seulement la commande avant &
				if (strcmp(mot[i], "&") == 0) { 
					mot[i] = NULL ;
	                        	is_amp = 1 ; 
	                        } 
			}
	                        			//commande interne moncd
			tmp = init_moncd(mot, len_mot) ;
			if (tmp > 0) {
				getcwd(cwd, sizeof(cwd));//Pour récupérer le nouveau répertoire courant
				continue ;
			} else if (tmp < 0) //erreur
				continue ;
				
			if (strcmp(mot[0], "exit") == 0) {//commande exit
				exit_prog = 1;	//Pour sortir de la boucle extérieur
				break ;
			} 
	
			tmp = fork();			// lancer le processus enfant
			if (errorcheck(tmp, "pipe"))
				continue ;
						
	
			if (tmp != 0) {			// parent : attendre la fin de l'enfant
				if (!is_amp)
					while(wait(0) != tmp) ;	
				
				/*avant de fermer tous les descripteurs du pipe, 
				  sauvegarde le pipe de lecture ou a été 
				  redirigé le stdout par l'enfant*/
				save_stdout = pipe_parent(opened_pipe, fd) ; 
	        		continue ; 
	        	}
	        					// Enfant :
	        					
	        					// Vérifie s'il y a des redirections et les lance
	        	if (to_redirect(mot)<0)
				exit(1); 		// Si il y a erreur, on arrête le tout et on relance le prompt. perror est dans la fonction to_redirect() pour plus de précision
            

			/*Fonction à effet, redirige stdout vers pipe d'écriture et ferme tous les descripteurs du pipe*/
	            	
	            	pipe_child(opened_pipe, i_p, fd, save_stdout); 
	            	
	            	
	            	
	            					//Pour executer dans le repertoire courant
			
			if (strchr(mot[0], '/') != NULL) {
    				execv(mot[0], mot) ;
    			}
 			else {				//Sinon dans Path
    				for(i = 0; dirs[i] != 0; i++) {
					snprintf(pathname, sizeof pathname, "%s/%s", dirs[i], mot[0]);
	                		execv(pathname, mot) ; 
    				}
    			
    			}
    			
		
							// aucun exec n'a fonctionne
			fprintf(stderr, "%s: not found\n", mot[0]);
			exit(1); 
		}
		
	/*remettre premier élément de ligne à 0 pour 
	  la considérer vide à la prochaine itération*/
	ligne[0] = 0;

	}
	
	printf("Bye\n");
	return 0; 
	
}

/*Afin de factoriser un peu le code car les codes erreurs ont toujours la même forme*/

int errorcheck(int val, char * S)
{
	if (val < 0) {
		perror(S);
		return -1 ;
	}
	return 0 ;
}


int init_moncd(char * mot[],int len_mot)
{
	if (strcmp(mot[0], "cd") == 0) { 
		if (moncd(len_mot, mot) < 0) { 
			fprintf(stderr, "cd a échoué.\n");
			return -1; 
		}
	        return 1 ; 
	}
	return 0 ;
}

int init_pipe(int iter, int len, int * p_tab)
{
	int tmp ;
	if(iter < len) { 	//Si il y a plus d'une commande restante, il faudra un pipe
		tmp = pipe(p_tab) ;	
		if (errorcheck(tmp, "pipe"))
			return -1 ;
		return 1 ;
	}
	return 0 ;
}

int pipe_parent(int opened_pipe, int * pipe)
{
	int save_stdout ;
	int tmp ;
	if (opened_pipe) { 	//Si commande est liée par un pipe
		tmp = close(pipe[1]); //ferme écriture, puisque pas besoin, c'est l'enfant qui s'en charge
	        if (errorcheck(tmp, "close"))
			return -1 ;

	        save_stdout = dup(pipe[0]); //Copie le pipe en lecture, déjà rempli par l'enfant
          	if (errorcheck(save_stdout, "dup"))
			return -1 ;
          	
          	tmp = close(pipe[0]); //ferme lecture, écriture déjà fermé
          	if (errorcheck(tmp, "close"))
			return -1 ;
		return save_stdout;
	}
	
	return 0 ;
}

void pipe_child(int opened_pipe, int it_pipe, int * pipe, int save_stdout)
{
	int tmp ;
	if (opened_pipe) { //toutes commandes sauf dernière
		tmp = close(pipe[0]); //ferme lecture car ouverte dans parent
	        if (errorcheck(tmp, "close"))
			exit(1) ;

	        tmp = dup2(pipe[1], STDOUT_FILENO); //Redirige le STDOUT sur pipe d'écriture
	        if (errorcheck(tmp, "dup2"))
			exit(1) ;
						
	        tmp = close(pipe[1]); //ferme ecriture car plus nécessaire
	        if (errorcheck(tmp, "close"))
			exit(1) ;
	}
	            	
	if(it_pipe>0) {  //toutes commandes sauf première
		tmp = dup2(save_stdout, STDIN_FILENO); //Redirige le pipe lect précédent sauvegardé par parnt sur STDIN
		if (errorcheck(tmp, "dup2"))
			exit(1) ;
	
		tmp = close(save_stdout);	//ferme le descripteur du pipe lect
		if (errorcheck(tmp, "close"))
			exit(1) ;
     	}
     	
     	return ;
}
