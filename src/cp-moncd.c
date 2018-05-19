/* cp-moncd.c
 Pour faire un chdir en interne
*/
#include "sys.h"


int moncd (int len, char * mot[])
{ 
	char * dir;
	int t;
	if (len < 2) { 
		dir = getenv("HOME");

	if (dir == 0)
                dir = "/tmp"; } 
	else if (len > 2) { 
		fprintf(stderr, "usage: %s [dir]\n", mot[0]);
            	return -1; 
	} 
	else dir = mot[1] ;
	
	t = chdir(dir);
	if (t < 0) { 
		perror(dir);
                return -1 ; 
	}

	return 0 ; 

}
                  
               


# ifdef TEST
int main(int argc, char * argv[])
{
	char cwd[1024];
	getcwd(cwd, sizeof(cwd)) ;
	printf("\nLe répertoire courant est: %s\n", cwd);
	moncd(argc, argv) ;
  	getcwd(cwd, sizeof(cwd)) ;
	printf("Après appel de moncd, il est: %s\n\n", cwd);
	return 0;
}
# endif
