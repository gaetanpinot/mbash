
#include <stdio.h>
#include <fcntl.h>
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
char *isPathValidDir(char* path);
char *isPathValidFile(char* path);
void execCommandeInconnu(char* commandeInconnu);

int indexCaract;
#define ETATDEPART 0
#define ETATALPHANUM 1
#define ETATEGALE 2
#define ETATESPACESAPRESALPHANUM 3
#define ETATPOINTEXEC 4
#define ETATFINI 5
int main(int argc, char** argv) {
	execCommandeInconnu("yes");
	
	return 0;
}

void execCommandeInconnu(char* commandeInconnu){
	char *envpath=getenv("PATH");
	//printf("%s\n",envpath);
	char *pathActu=strdup(strtok(envpath,":"));
	char *pathValide=NULL;

	while(pathActu){
		strcat(pathActu,"/");
		strcat(pathActu,commandeInconnu);
		//printf("1 path est :%s\n",pathActu);
		if(isPathValidFile(pathActu)){
			//printf("path %s valide\n",pathActu);
			pathValide=strdup(pathActu);
			pathActu=NULL;
		}else{
			pathActu=strtok(NULL,":");
			if(pathActu){
				pathActu=strdup(pathActu);
			}
		}
	}
	if(pathValide){
		printf("path %s valide\n",pathValide);
		execFichier(pathValide);
	}else{
		printf("%s n'est pas une commande valide\n",commandeInconnu);
	}
}

//verifie la validité d'un chemin
//renvoie NULL si le chemin n'est pas valide
//adapte le chemin si il commence par un ~
char *isPathValidDir(char* path){

	char* ourPath=NULL;
	if(path[0]=='~'){
		char temppath[MAXLI]="/home/";
		strcat(temppath,getlogin());
		strcat(temppath,&path[1]);
		path=temppath;
	}
	DIR* dir =-1;
	dir = opendir(path);
	printf("%s\n",path);
	if (dir) {
		closedir(dir);
		//le chemin est valide, on peut le retourner
		ourPath=path;
	} else{
		printf("chemin non valide\n");
	}
	return(ourPath);
}


char *isPathValidFile(char* path){

	char* ourPath=NULL;
	if(path[0]=='~'){
		char temppath[MAXLI]="/home/";
		strcat(temppath,getlogin());
		strcat(temppath,&path[1]);
		path=temppath;
	}
	FILE* fi=-1;
	fi=fopen(path,"r");
	if(fi){
		fclose(fi);
		ourPath=path;
	}else{
		//printf("chemin non valide\n");
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
		strcpy(cdPathValide,isPathValidDir(cdPathValide));
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
#define EXDEPART 0
#define EXDANSPATH 1
#define EXESPACESAPRES 2
#define EXOPTIONS 3
#define EXFIN 4

void execFichier(){
	int longcmd=strlen(cmd);
	cmd[longcmd]=' ';
	cmd[longcmd+1]='\0';
	char caractEX=cmd[indexCaract];
	char cdPathValide[MAXLI];
	char *options[20]={"",NULL};
	int nbOptions=1;
	char optionActuel[200];
	optionActuel[0]='\0';
	int ETAT=0;
	while(caractEX!='\0'){
		switch(ETAT){
			case EXDEPART:
				switch(caractEX){
					case ' ':
						break;
					case '/':
						if(cmd[indexCaract-1]=='.'){

							cdPathValide[0]='.';
							cdPathValide[1]='/';
							cdPathValide[2]='\0';
						}else{
							cdPathValide[0]='/';
							cdPathValide[1]='\0';
						}
	
						ETAT=EXDANSPATH;
						break;
					default:
						cdPathValide[0]=caractEX;
						cdPathValide[1]='\0';
						ETAT=EXDANSPATH;
						break;
				}
				break;
			case EXDANSPATH:
				switch(caractEX){
					case ' ':
						ETAT=EXESPACESAPRES;
						break;
					default:
						int templong=strlen(cdPathValide);
						cdPathValide[templong]=caractEX;
						cdPathValide[templong+1]='\0';
						break;
				}

				break;

			case EXESPACESAPRES:
				switch(caractEX){
					case ' ':
						break;
					default:
						int templong=strlen(optionActuel);
						optionActuel[templong]=caractEX;
						optionActuel[templong+1]='\0';
						ETAT=EXOPTIONS;
						break;
				}

				break;

			case EXOPTIONS:

				switch(caractEX){
					case ' ':
						if(strlen(optionActuel)){
							//printf("opti %d = %s\n",nbOptions,optionActuel);
							
							options[nbOptions]=strdup(optionActuel);
							//printf("%s\n",options[nbOptions]);
							//printf("%s\n",options[0]);
							options[++nbOptions]=NULL;
							optionActuel[0]='\0';
						}
						break;
					default:
						int templong=strlen(optionActuel);
						optionActuel[templong]=caractEX;
						optionActuel[templong+1]='\0';
						break;
						
				}
				break;
			case EXFIN:

				break;
			default:
				caractEX='\0';
				break;
		}

		//printf("%c , ETAT %d\n",caractEX,ETAT);
		caractEX=cmd[++indexCaract];
	}

	/*
	for(int i=0;i<nbOptions;i++){
		printf("option %d %s\n",i,options[i]);
		printf("option %d %p\n",i,options[i]);
	}
	*/


	//printf("%s\n",cdPathValide);
	char* execPathValide=isPathValidFile(cdPathValide);
	if(execPathValide!=NULL){
	//	printf("%s\n",execPathValide);
		options[0]=strdup(execPathValide);
		int pid=fork();
		if(pid>0){
			int status;
			wait(&status);
		}else{
			char *args[]={execPathValide,NULL};
			char *env[]={NULL};
			int status=execve(execPathValide,options,env);
			printf("execute %s\nstatus=%d\n",execPathValide,status);
		}

	}else{
		printf("Chemin: %s est non valide\n",execPathValide);
	}
}

