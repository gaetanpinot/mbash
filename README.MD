## Fonctionnalitées
- cd (fonctionne aussi avec ~ (remplace par /home/login))
- pwd 
- éxécute un fichier
pour éxécuter un fichier à partir du répértoire courant, il faut utiliser `./fichierExecutable`  
pour l'éxécuter depuis un chemin absolu il faut utiliser `. /usr/bin/fichierExecutable`
- & dans la commande pour lancer la commande en arrière plan
- recherche dans le PATH quand on tape une commande non reconnue le programme cherche dans le PATH, si il trouve une commande qui correspond alors il l'execute


Voici un commentaire sur les fonctions que j'ai crées, quelque commentaires suplémentaires sont disponible dans le code.

`void cd()`  
Cette fonction parse avec un automate le parametre situé après la commande `cmd`, si c'est un chemin de fichier valide alors il s'y déplace. 

`void pwd()`
Cette fonction imprime le chemin du répertoire courant

`void execFichier()`
Cette fonction parse avec un automate le paramètre situé après le `.`, en premier elle verifie si le chemin est un fichier executable, si oui alors elle l'execute en passant en parametre les autres arguments de la commande, et trois variables d'environnement TERM, DISPLAY et HOME, pour pouvoir lancer des programmes graphiques.
L'automate de la fonction prend en compte les paramètre entre `"` ou ceux qui sont séparé par un espace. Exemple:
`git commit -m "ceci est un message de commit, \"cette partie est entouré de double quote\""`
Pour ce prompt la fonction va parser les options et va trouver ça:
- Option 1 = commit
- Option 2 = -m
- Option 3 = ceci est un message de commit, "cette partie est entouré de double quote"
A noter que les seuls charactère echapé par le programme sont `\"`, `\n` et `\t`, les autres seront ignoré, donc si on veut avoir un `*` échapé, pour un grep par exemple, il faudra taper `\\*`.

`char *isPathValid(char *path,int checkFile)`
Cette fonction verifie que le chemin en parametre est un dossier ou fichier (en fonction de checkFile) valide, et si il possède un `~` alors elle le remplace par le chemin absolu du home de l'utilisateur, elle renvoie donc le chemin corrigé si nécéssaire quand il est valide, et NULL si il n'est pas valide

`void execCommandeInconnu(char *commandeInconnu)`
Cette fonction prend une commande en parametre et si elle trouve un fichier du meme nom dans les dossiers du PATH alors elle réarange la commande pour que `execFichier()` puisser l'éxécuter

`void intProcFils(int signum)`
Cette fonction est faite pour s'occuper d'interompre le processus en cours quand le programme reçoit `SIGINT`

`int main()`
Le main est une boucle infini qui demande un prompt puis qui parcours le prompt avec un automate pour savoir quel fonction il doit lancer

