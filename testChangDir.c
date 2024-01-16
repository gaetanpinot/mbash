#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#define MAXLI 2048
char cmd[MAXLI];
char path[MAXLI];
int pathidx;
void mbash();
void cd();
void pwd();
void execFichier();
char *isPathValid(char* path);

int indexCaract;
#define ETATDEPART 0
#define ETATALPHANUM 1
#define ETATEGALE 2
#define ETATESPACESAPRESALPHANUM 3
#define ETATPOINTEXEC 4
#define ETATFINI 5
int main(int argc, char** argv) {
	while(1){
		printf("mbash>");
		fgets(cmd,MAXLI,stdin);
		cmd[strlen(cmd)-1]='\0';
		int ETAT=0;
		indexCaract=0;
		char caract=cmd[indexCaract];
		char commandeSwitch[MAXLI];
		while(caract!='\0'){
			switch(ETAT){
				case ETATDEPART:
					switch(caract){
						case '.':
							ETAT=ETATFINI;
							commandeSwitch[0]=caract;
							commandeSwitch[1]='\0';
							break;
						case ' ':
							break;
						default:
							commandeSwitch[0]=caract;
							commandeSwitch[1]='\0';
							ETAT=ETATALPHANUM;
							break;
					}
					break;
				case ETATALPHANUM:
					switch(caract){
						case ' ':
							ETAT=ETATESPACESAPRESALPHANUM;
							break;

						case '=':
							commandeSwitch[0]=caract;
							commandeSwitch[1]='\0';
							ETAT=ETATFINI;
							break;
						default:
							int templong=strlen(commandeSwitch);
							commandeSwitch[templong]=caract;
							commandeSwitch[templong+1]='\0';
							break;
					}

					break;

				case ETATESPACESAPRESALPHANUM:
					switch(caract){
						case '=':
							commandeSwitch[0]=caract;
							commandeSwitch[1]='\0';
							ETAT=ETATFINI;
							break;
						case ' ':
							break;
						default:
							ETAT=ETATFINI;
							break;
					}
					break;
				case ETATFINI:
					caract='\0';
					break;
				default:
					caract='\0';
					break;
			}
		//printf("%c , ETAT %d\n",caract,ETAT);

		if(caract!='\0'){
			caract=cmd[++indexCaract];
		}
		}


	
	printf("commandeSwitch  %s\n",commandeSwitch);
	//printf("%s\n",cmd);
	//strtok(cmd," ");

	if(!strcmp(commandeSwitch,"cd")){
		cd();
	}else if(!strcmp(commandeSwitch,"pwd")){
		pwd();
	}else if(!strcmp(cmd,"./")){
		execFichier();
	}else{
		printf("Commande non reconnue\n");
	}
}
return 0;
}
//verifie la validité d'un chemin
//renvoie NULL si le chemin n'est pas valide
//adapte le chemin si il commence par un ~
char *isPathValid(char* path){

	char* ourPath;
	if(path[0]=='~'){
		char temppath[MAXLI]="/home/";
		strcat(temppath,getlogin());
		strcat(temppath,&path[1]);
		path=temppath;
	}
	DIR* dir =-1;
	dir = opendir(path);
	printf("%s\n",path);
	if (dir!=-1) {
		closedir(dir);
		//le chemin est valide, on peut le retourner
		ourPath=path;
	} else {
	}

	return(ourPath);
}

void pwd(){
	char chemin[MAXLI];
	getcwd(chemin,MAXLI);
	printf("Vous etes dans le repertoire: %s\n",chemin);
}
#define CDDEPART 0
#define CDDANSPATH 1
#define CDESPACESAPRES 2
#define CDFIN 3
void cd(){
	//on récupère le premier argument après la commande cd
	//char* cdPath=strtok(NULL," ");
	char caractCD=cmd[--indexCaract];
	char cdPathValide[MAXLI];
	int ETAT=0;
	while(caractCD!='\0'){
		switch(ETAT){
			case CDDEPART:
				switch(caractCD){
					case ' ':
						break;
					default:
						cdPathValide[0]=caractCD;
						cdPathValide[1]='\0';
						ETAT=CDDANSPATH;
						break;
				}
				break;
			case CDDANSPATH:
				switch(caractCD){
					case ' ':
						ETAT=CDESPACESAPRES;
						break;
					default:
						int templong=strlen(cdPathValide);
						cdPathValide[templong]=caractCD;
						cdPathValide[templong+1]='\0';
						break;
				}

				break;

			case CDESPACESAPRES:
				switch(caractCD){
					case ' ':
						break;
					default:
						ETAT=-1;
						break;
				}

				break;

			case CDFIN:

				break;
			default:
				caractCD='\0';
				break;
		}

		caractCD=cmd[++indexCaract];
	}

	if(ETAT>=0){
		//on valide le chemin
		strcpy(cdPathValide,isPathValid(cdPathValide));
		if(cdPathValide!=NULL){

			chdir(cdPathValide);
		}
		else{
			printf("Chemin: %s est non valide\n",cdPathValide);
		}
	}else{
		printf("Trop d'arguments\n");
	}
	/*
	   while(cdPath!=NULL){
	   printf("%s\n",cdPath);
	   cdPath=strtok(NULL," ");
	   }
	   */
}
void execFichier(){
	char* execPath=strtok(NULL," ");
	char* execPathValide=isPathValid(execPath);
	if(execPathValide!=NULL){
		int pid=fork();
		if(pid>0){
			int status;
			wait(&status);
		}else{
			printf("execute %s\n",execPathValide);
		}

	}else{
		printf("Chemin: %s est non valide\n",execPath);
	}
}


