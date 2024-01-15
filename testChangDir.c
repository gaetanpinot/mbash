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

#define ETATDEPART 0
#define ETATCMDSANSESPACESAPRES 1
#define ETATCMDESPACESAPRES 2
#define ETATPATHSANSESPACESAVANT 3
#define ETATESPACEENTREARGUMENT 4
#define ETATPARALLELE 5
#define ETATDASHPARAM 6
#define ETATPATHSESPACESAPRES 7
#define ETATFINI 8
int main(int argc, char** argv) {
	while(1){
		printf("mbash>");
		fgets(cmd,MAXLI,stdin);
		cmd[strlen(cmd)-1]='\0';
		int indexCaract=0;
		char caract=cmd[indexCaract];
		while(caract!='\0'){


			caract=cmd[++indexCaract];
		}
		//printf("%s\n",cmd);
		strtok(cmd," ");

		if(!strcmp(cmd,"cd")){
			cd();
		}else if(!strcmp(cmd,"pwd")){
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
		char temppath[5000]="/home/";
		strcat(temppath,getlogin());
		strcat(temppath,&path[1]);
		path=temppath;
	}
	DIR* dir = opendir(path);
	if (dir) {
		closedir(dir);
		//le chemin est valide, on peut le retourner
		ourPath=path;
	} else if (ENOENT == errno) {
	} else {
	}

	return(ourPath);
}

void pwd(){
	char* chemin;
	getcwd(chemin,5000);
	printf("vous etes dans le repertoire: %s\n",chemin);
}
void cd(){
	//on récupère le premier argument après la commande cd
	char* cdPath=strtok(NULL," ");
	char* cdPathValide;

	//on valide le chemin
	cdPathValide = isPathValid(cdPath);	
	if(cdPathValide!=NULL){

		chdir(cdPathValide);
	}
	else{
		printf("Chemin: %s est non valide\n",cdPath);
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


