# SAE 3.03 mbash
## Fonctionnalitées
- cd (fonctionne aussi avec ~ (remplace par /home/login))
- pwd 
- éxécute un fichier
pour éxécuter un fichier à partir du répértoire courant, il faut utiliser `./fichierExecutable`  
pour l'éxécuter depuis un chemin absolu il faut utiliser `. /usr/bin/fichierExecutable`
- & (fork le fichier)



`void cd()`  
Cette fonction parse avec un automate le parametre situé après la commande `cmd`, si c'est un chemin de fichier valide alors il s'y déplace. 


`void pwd()`
Cette fonction imprime le chemin du répertoire courant

`void execFichier()`
Cette fonction parse avec un automate le paramètre situé après le `.`, en premier elle verifie si le chemin est un fichier executable, si oui alors elle l'execute en passant en parametre les autres arguments de la commande, et deux variables d'environnement TERM et DISPLAY, pour pouvoir lancer des programmes graphiques

`char *isPathValidDir(char *path)`
Cette fonction verifie que le chemin en parametre est un dossier valide, et si il possède un `~` alors elle le remplace par le chemin absolu du home de l'utilisateur, elle renvoie donc le chemin corrigé si nécéssaire quand il est valide, et NULL si il n'est pas valide

`char *isPathValidFile(char *path)`
Cette fonction fait la meme chose que isPathValidDir mais pour les fichiers.

`void execCommandeInconnu(char *commandeInconnu)`
Cette fonction prend une commande en parametre et si elle trouve un fichier du meme nom dans les dossiers du PATH alors elle réarange la commande pour que `execFichier()` puisser l'éxécuter

`void intProcFils(int signum)`
Cette fonction est faite pour s'occuper d'interompre le processus en cours quand le programme reçoit `SIGINT`

