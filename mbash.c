//gcc -o mbash mbash.c -Wall -Wextra -Werror -pedantic
//compilé avec des options de warning suplémentaires pour éviter les erreurs a l'execution
//PINOT GAËTAN S3A
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MAXLI 2048
char cmd[MAXLI];
int pathidx;

void mbash();

void cd();

void pwd();

void execFichier();

char *isPathValid(char *path, int checkFile); 

void execCommandeInconnu(char *commandeInconnu);

void intProcFils(int signum);

int pidEnfant;

int indexCaract;
#define ETATDEPART 0
#define ETATALPHANUM 1
#define ETATEGALE 2
#define ETATESPACESAPRESALPHANUM 3
#define ETATPOINTEXEC 4
#define ETATFINI 5

int main() {
int ETAT; 
char caract; 
char commandeSwitch[MAXLI];
int templong;
int lancerParralelle;
printf("Bienvenue sur mbash, un programme fait par Gaëtan Pinot en S3A\nCe programme supporte les commandes cd et pwd par défaut\nIl supporte aussi l'éxécution de fichier\nComme par exemple ./helloworld.sh si le fichier est dans le répértoire courant\nOu . /usr/bin/ls pour un chemin absolu\nPour les commandes inconnu le programme essayera de trouver un chemin qui convient dans le PATH\nDonc ls fonctionnera, il serat précisé quand une commande est éxécuté via le PATH\nLes options fonctionne, donc ls -l ../ sera bien éxécuté\nVous pouvez aussi lancer des programmes en arrière plan avec &\nPar exemple emacs & lancera emacs sans bloquer le terminal\n");
	while (1) {
		signal(SIGINT, SIG_DFL);
		printf("mbash>");
		//fflush(stdout);
		char *status = fgets(cmd, MAXLI, stdin);
		if(status==NULL){
			printf("Erreur dans la lecture de votre commande\n");
		}else{
			//on enleve le charactère \n 
			cmd[strlen(cmd) - 1] = '\0';
			ETAT = 0;
			indexCaract = 0;
			caract = cmd[indexCaract];
			strcpy(commandeSwitch,"");
			lancerParralelle=0;
			while (caract != '\0') {
				switch (ETAT) {
					case ETATDEPART:
						switch (caract) {
							case '.':
								ETAT = ETATFINI;
								commandeSwitch[0] = caract;
								commandeSwitch[1] = '\0';
								break;
							case ' ':
								break;
							default:
								commandeSwitch[0] = caract;
								commandeSwitch[1] = '\0';
								ETAT = ETATALPHANUM;
								break;
						}
						break;
					case ETATALPHANUM:
						switch (caract) {
							case ' ':
								ETAT = ETATESPACESAPRESALPHANUM;
								break;

							case '=':
								commandeSwitch[0] = caract;
								commandeSwitch[1] = '\0';
								ETAT = ETATFINI;
								break;
							default:
								templong = strlen(commandeSwitch);
								commandeSwitch[templong] = caract;
								commandeSwitch[templong + 1] = '\0';
								break;
						}

						break;

					case ETATESPACESAPRESALPHANUM:
						switch (caract) {
							case '=':
								commandeSwitch[0] = caract;
								commandeSwitch[1] = '\0';
								ETAT = ETATFINI;
								break;
							case '&':

								lancerParralelle=1;

								break;
							case ' ':
								break;
							default:
								ETAT = ETATFINI;
								break;
						}
						break;
					case ETATFINI:
						caract = '\0';
						break;
					default:
						caract = '\0';
						break;
				}
				//printf("%c , ETAT %d\n",caract,ETAT);

				if (caract != '\0') {
					caract = cmd[++indexCaract];
				}
			}


			//printf("%s\n", commandeSwitch);
			//printf("%s\n",cmd);
			//strtok(cmd," ");
			int pid=0;	
			if(lancerParralelle){
				pid=fork();
			}

			lancerParralelle=0;
			if(!pid){
				if (!strcmp(commandeSwitch, "cd")) {
					cd();
				} else if (!strcmp(commandeSwitch, "pwd")) {
					pwd();
				} else if (!strcmp(commandeSwitch, ".")) {
					execFichier();
				} else if (!strcmp(commandeSwitch, "")) {
				} else {
					execCommandeInconnu(commandeSwitch);
					//printf("Commande non reconnue\n");
				}
			}


		}
	}
	return 0;
}

//cette fonction prend en parametre une commande et essaie de savoir si c'est un executable qui existe dans le path
//si il trouve que la commande est valide dans le path alors il la remet comme il faut dans cmd
// et puis il apelle execFichier() qui s'occupe du reste
void execCommandeInconnu(char *commandeInconnu) {
	char *envpath = strdup(getenv("PATH"));
	//printf("%s\n",envpath);
	char commandeReconstitue[MAXLI] = ". ";
	char pathActu[MAXLI];
	//strtok vas permettre de separer chaque chemin du PATH
	strcpy(pathActu, (strtok(envpath, ":")));
	char *pathValide = NULL;
	int etreDansBoucle = 1;

	//on boucle pour tout les chemin du PATH et on test si c'est un chemin valide

	while (etreDansBoucle) {
		strcat(pathActu, "/");
		strcat(pathActu, commandeInconnu);
		//printf("1 path est :%s\n",pathActu);
		char *debugPath = strdup(pathActu);
		if (isPathValid(debugPath,1) ) {
			//printf("path %s valide\n",pathActu);
			pathValide = strdup(pathActu);
			etreDansBoucle = 0;
		} else {
			char *tempstr = strtok(NULL, ":");
			if (tempstr) {
				strcpy(pathActu, tempstr);
			} else {
				etreDansBoucle = 0;
			}
		}
	}
	if (pathValide) {
		//printf("path %s valide\n",pathValide);
		int i;
		//on reconstitue la commande pour que si on ai reçu ça "ls -l /media/usbdrive"
		//ça donne ". /usr/bin/ls -l /media/usbdrive"
		//comme ça la fonction execFichier() peut s'occuper du parsing des paramètres

		for (i = 2; i < (int)strlen(pathValide) + 2; i++) {
			commandeReconstitue[i] = pathValide[i - 2];
		}

		int tempInd = indexCaract;
		if (cmd[tempInd] != '\0') {
			commandeReconstitue[i++] = ' ';

			char caractOptions = cmd[--tempInd];
			while (caractOptions != '\0') {
				//printf("charOpt %c\n",caractOptions);
				commandeReconstitue[i++] = caractOptions;
				caractOptions = cmd[++tempInd];;
			}
			commandeReconstitue[i] = caractOptions;
		}

		//printf("%s\n",commandeReconstitue);
		strcpy(cmd, commandeReconstitue);
		indexCaract = 1;
		printf("Execution d'une commande inconnu à mbash, mais trouvé dans le path\n");
		execFichier();
	} else {
		printf("%s n'est pas une commande valide\n", commandeInconnu);
	}
}

char *isPathValid(char *path, int checkFile) {
    char *ourPath = NULL;

    if (path[0] == '~') {
        char homeDir[MAXLI] = "/home/";
        strcat(homeDir, getlogin());
        strcat(homeDir, &path[1]);
        path = homeDir;
    }

    if (checkFile) {
        FILE *fi = fopen(path, "r");
        if (fi) {
            fclose(fi);
            ourPath = path;
        } else {
            //printf("chemin non valide\n");
        }
    } else {
        DIR *dir = opendir(path);
        if (dir) {
            closedir(dir);
            // The path is valid for a directory
            ourPath = path;
        } else {
            //printf("chemin non valide\n");
        }
    }

    return ourPath;
}



void pwd() {
	char chemin[MAXLI];
	char* status = getcwd(chemin, MAXLI);
	if(status){
		printf("Vous etes dans le repertoire: %s\n", chemin);
	}else{
		printf("Erreur dans la lecture du répertoire\n");
	}
}

#define CDDEPART 0
#define CDDANSPATH 1
#define CDESPACESAPRES 2
#define CDFIN 3

void cd() {
	//on récupère le premier argument après la commande cd
	//char* cdPath=strtok(NULL," ");
	char caractCD = cmd[--indexCaract];
	char cdPathValide[MAXLI];
	int ETAT = 0;
	int templong;
	while (caractCD != '\0') {
		switch (ETAT) {
			case CDDEPART:
				switch (caractCD) {
					case ' ':
						break;
					default:
						cdPathValide[0] = caractCD;
						cdPathValide[1] = '\0';
						ETAT = CDDANSPATH;
						break;
				}
				break;
			case CDDANSPATH:
				switch (caractCD) {
					case ' ':
						ETAT = CDESPACESAPRES;
						break;
					default:
						templong = strlen(cdPathValide);
						cdPathValide[templong] = caractCD;
						cdPathValide[templong + 1] = '\0';
						break;
				}

				break;

			case CDESPACESAPRES:
				switch (caractCD) {
					case ' ':
						break;
					default:
						ETAT = -1;
						break;
				}

				break;

			case CDFIN:

				break;
			default:
				caractCD = '\0';
				break;
		}

		caractCD = cmd[++indexCaract];
	}

	if (ETAT >= 0) {
		//on valide le chemin
		char* cdPathApresValid=isPathValid(cdPathValide,0);
		if (cdPathApresValid!= NULL) {

			if(-1==chdir(cdPathApresValid)){
				printf("Erreur dans le changement de repertoire\n");
			}
		} else {
			printf("Chemin: %s est non valide\n", cdPathValide);
		}
	} else {
		printf("Trop d'arguments\n");
	}
	/*
	   while(cdPath!=NULL){
	   printf("%s\n",cdPath);
	   cdPath=strtok(NULL," ");
	   }
	   */
}

#define EXDEPART 0
#define EXDANSPATH 1
#define EXESPACESAPRES 2
#define EXOPTIONS 3
#define EXDQUOTE 4
#define EXESCAPE 5
#define EXFIN 6

//parse le tableau cmd pour trouver un chemin de fichier à éxécuter et les arguments qui vont avec
void execFichier() {

	int longcmd = strlen(cmd);
	cmd[longcmd] = ' ';
	cmd[longcmd + 1] = '\0';
	char caractEX = cmd[indexCaract];
	char cdPathValide[MAXLI];
	char *options[20] = {"", NULL};
	int nbOptions = 1;
	char optionActuel[MAXLI];
	optionActuel[0] = '\0';
	int ETAT = 0;
	int templong;
	while (caractEX != '\0') {
		switch (ETAT) {
			case EXDEPART:
				switch (caractEX) {
					case ' ':
						break;
					case '/':
						if (cmd[indexCaract - 1] == '.') {

							cdPathValide[0] = '.';
							cdPathValide[1] = '/';
							cdPathValide[2] = '\0';
						} else {
							cdPathValide[0] = '/';
							cdPathValide[1] = '\0';
						}

						ETAT = EXDANSPATH;
						break;
					default:
						cdPathValide[0] = caractEX;
						cdPathValide[1] = '\0';
						ETAT = EXDANSPATH;
						break;
				}
				break;
			case EXDANSPATH:
				switch (caractEX) {
					case ' ':
						ETAT = EXESPACESAPRES;
						break;
					default:
						templong = strlen(cdPathValide);
						cdPathValide[templong] = caractEX;
						cdPathValide[templong + 1] = '\0';
						break;
				}

				break;

			case EXESPACESAPRES:
				switch (caractEX) {
					case ' ':
						break;
					case '"':
						if (strlen(optionActuel)) {
							options[nbOptions] = strdup(optionActuel);
							options[++nbOptions] = NULL;
							optionActuel[0] = '\0';
						}
						ETAT=EXDQUOTE;

						break;
					default:
						templong = strlen(optionActuel);
						optionActuel[templong] = caractEX;
						optionActuel[templong + 1] = '\0';
						ETAT = EXOPTIONS;
						break;
				}

				break;

			case EXOPTIONS:

				switch (caractEX) {
					case ' ':
						if (strlen(optionActuel)) {
							//printf("opti %d = %s\n",nbOptions,optionActuel);

							options[nbOptions] = strdup(optionActuel);
							//printf("%s\n",options[nbOptions]);
							//printf("%s\n",options[0]);
							options[++nbOptions] = NULL;
							optionActuel[0] = '\0';
						}
						break;
					case '"':
						if (strlen(optionActuel)) {
							options[nbOptions] = strdup(optionActuel);
							options[++nbOptions] = NULL;
							optionActuel[0] = '\0';
						}
						ETAT=EXDQUOTE;

						break;
					default:
						templong = strlen(optionActuel);
						optionActuel[templong] = caractEX;
						optionActuel[templong + 1] = '\0';
						break;

				}
				break;
			case EXDQUOTE:
				switch (caractEX){
					case '"':
						if (strlen(optionActuel)) {
							//printf("opti %d = %s\n",nbOptions,optionActuel);

							options[nbOptions] = strdup(optionActuel);
							//printf("%s\n",options[nbOptions]);
							//printf("%s\n",options[0]);
							options[++nbOptions] = NULL;
							optionActuel[0] = '\0';
						}
						ETAT=EXOPTIONS;

						break;
					case '\\':
						ETAT=EXESCAPE;
						break;
					default:
						templong = strlen(optionActuel);
						optionActuel[templong] = caractEX;
						optionActuel[templong + 1] = '\0';
						break;
				}

				break;
			case EXESCAPE:
				switch (caractEX){
					case 'n':
						templong = strlen(optionActuel);
						optionActuel[templong] = '\n';
						optionActuel[templong + 1] = '\0';
						break;
					case 't':
						templong = strlen(optionActuel);
						optionActuel[templong] = '\t';
						optionActuel[templong + 1] = '\0';
						break;
					default:
						templong = strlen(optionActuel);
						optionActuel[templong] = caractEX;
						optionActuel[templong + 1] = '\0';
						break;
				}
				ETAT=EXDQUOTE;
				break;


			case EXFIN:

				break;
			default:
				caractEX = '\0';
				break;
		}

		//printf("%c , ETAT %d\n",caractEX,ETAT);
		caractEX = cmd[++indexCaract];
	}
	/*
	   for(int i=0;i<nbOptions;i++){
	   printf("option %d %s\n",i,options[i]);
	   }
	   */


	//printf("%s\n",cdPathValide);
	char *execPathValide = isPathValid(cdPathValide,1);
	struct stat sb;
	if (execPathValide != NULL && stat(execPathValide, &sb) == 0 && sb.st_mode & S_IXUSR) {
		//	printf("%s\n",execPathValide);
		options[0] = strdup(execPathValide);
		int pid = fork();
		if (pid > 0) {
			pidEnfant = pid;
			signal(SIGINT, intProcFils);
			int status;
			wait(&status);
		} else {
			//printf("%d\n", getpid());
			//variables d'environnement pour pouvoir lancer des programmes visuels
			char temppath[MAXLI] = "HOME=/home/";
			strcat(temppath, getlogin());

			char *env[] = {"DISPLAY=:0","TERM=xterm-256color",temppath,NULL};
			int status = execve(execPathValide, options, env);
			if(status==-1){
				printf("Erreur d'execution du programme\n");
			}
			//printf("execute %s\nstatus=%d\n",execPathValide,status);
		}

	} else {
		printf("Chemin: %s est non valide\n", execPathValide);
	}
}

void intProcFils(int signum) {
	if (signum == SIGINT) {
		printf("Interruption de l'execution\n");
		kill(pidEnfant, SIGINT);
	}
}

