# monshell
Shell For Linux distros

MONSHELL(1)  User Manual  MONSHELL(1)

NAME
  monshell - monshell est un shell élémentaire.

SYNOPSIS
  monshell COMMAND

DESCRIPTION
  monshell est un programme permettant l'appel d'autres programmes 
  suivis par leurs arguments.
  Il possède quelques rares commandes internes comme "cd" ou "exit" et 
  plusieurs fonctionnalités comme un traitement de texte basique pour 
  la ligne de commande, la redirection à l'aide d'opérateurs spéciaux, 
  l'appel de programmes en arrière plan ou encore le piping d'une 
  commande vers une autre.
  L'utilisateur, après avoir appelé le programme, se voit proposé un
  prompt stipulant le répertoire courant. Il peut ensuite entrer la
  commande qu'il désire. Celle-ci sera recherché dans les répertoires
  du path ou à partir d'un chemin absolu ou relatif. Celle-ci peut être
  suivi d'arguments à destination du programmes ou encore d'options 
  décrites ci-dessous.
  L'édition de texte permet de se déplacer sur la ligne avec les flêches
  du clavier, la compléter avec l'injection de caractères ou encore la 
  suppression à l'aide des touches del ou bcksp. Appuyé sur la touche
  tab permet d'obtenir l'auto-complétion du mot sur lequel le curseur
  se trouve. Dans le cas où le répertoire courant contient plusieurs
  mots correspondant à celui tapé, alors en appuyant une deuxième fois
  sur tab nous obtenons cette liste à la ligne.

OPTIONS
  cd		- Une commande interne qui permet de changer le répertoire
  		courant par celui renseigné en argument 2. En absence 
  		d'arguments, il s'agit du répertoire "home" (ou "/tmp" 
  		en son absence). 
  		Les chemins d'accès peuvent être relatifs ou absolus.
  		
  exit		- Une commande interne qui permet de quitter le programme.
  
  COMMAND &	- Une option qui délimite la fin de la ligne de commande 
  		et demande d'effectuer la commande renseigné en arrière 
  		plan sans mettre le shell en état d'attente de la fin de 
  		celle-ci.
  		Attention cette option ne peut pas être placé n'importe 
  		où sur la ligne de commande car elle délimite sa fin.
  		La mettre avant les arguments d'une commande reviendrait 
  		à s'en débarasser.
  
  > file	- option qui peut-être placé n'importe où sur la ligne
  		et effectue une redirection du descripteur stdout vers
  		le fichier renseigné. Cette redirection crée ou écrase
  		le fichier renseigné.
  		
  >> file	- option qui peut-être placé n'importe où sur la ligne
  		et effectue une redirection du descripteur stdout vers
  		le fichier renseigné. Cette redirection crée ou joint
  		à la suite du fichier existant le contenu du stdout.
  
  < file	- option qui peut-être placé n'importe où sur la ligne
  		et effectue une redirection du fichier renseigné vers
  		le stdin de la commande.
  	
  COMMAND |	- option "pipe" qui permet de rediriger le stdout de la
  COMMAND |...	première commande sur le stdin de la deuxième. Ainsi,
  		le résultat d'une commande est transmise à la deuxième.
  		128 commandes peuvent être mises à la suite. 
  
EXAMPLES
  
  - cd
  	/home/solalgaillard/Desktop/systeme d'ex/Chapitre 11? cd ../
  	/home/solalgaillard/Desktop/systeme d'ex?
  
  	/home/solalgaillard/Desktop/systeme d'ex/Chapitre 11? cd src/
  	/home/solalgaillard/Desktop/systeme d'ex/Chapitre 11/src? 
  	
  - exit
  	? exit
	? Bye
	
  -command &
  	? sleep 10 &
	? echo meme pas 10 secondes
	meme pas 10 secondes
	? 
	
  - > file
    >> file
    < file
	? cat < ./src/redirection.c > toto
	? echo une fois > toto
	? echo deux fois >> toto
	
  COMMAND |	
  COMMAND |...	
	? cat < ./src/co-main.c | wc
  	  233     880    6336
  	? cat < ./src/co-main.c | uniq | wc
   	  229     880    6304
   	  
   press key tab
   	? src/t
	text_entry	text_entry.c	
	? s src/t s s
	text_entry	text_entry.c	
	? s src/t src/ s
	cn-decouper.c	co-main.c	cp-moncd.c	monshell	redirection.c	sys.h	text_entry	
	text_entry.c	


SEE ALSO
  bash(1), sh(1), csh(1)
  
BUGS
  Le plus grand problème de ce shell est qu'il sectionne les espaces 
  des chaînes même entre guillemets ou avec des escape characters. 
  Ainsi certains noms de fichiers ou dossiers seront inaccessibles et 
  on ne pourra pas passé certains arguments qui nécessitent d'être 
  divisés par des espaces mais considéré comme un seul élément.

AUTHOR
  Solal Gaillard

COPYRIGHT
  This is free and unencumbered software released into the public domain.

  Anyone is free to copy, modify, publish, use, compile, sell, or
  distribute this software, either in source code form or as a compiled
  binary, for any purpose, commercial or non-commercial, and by any
  means.

  In jurisdictions that recognize copyright laws, the author or authors
  of this software dedicate any and all copyright interest in the
  software to the public domain. We make this dedication for the benefit
  of the public at large and to the detriment of our heirs and
  successors. We intend this dedication to be an overt act of
  relinquishment in perpetuity of all present and future rights to this
  software under copyright law.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
  OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
  OTHER DEALINGS IN THE SOFTWARE.

  For more information, please refer to <http://unlicense.org>
