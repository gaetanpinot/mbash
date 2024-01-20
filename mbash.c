#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#define MAXLI 2048
char cmd[MAXLI];
char path[MAXLI];
int pathidx;
void mbash();
int main(int argc, char** argv) {
	char *testSTR;
//	fgets(testSTR,4000,stdin);

	testSTR="hello\b bonjour\b\b salut \b";
	int taille=strlen(testSTR);
	printf("%s\n",testSTR);
	for(int i=0;i<taille;i++){
		printf("char:'%c' val:%d\n",testSTR[i],testSTR[i]);
	}
	return 0;
}

void mbash() {
  printf("Execute: %s", cmd);
  system(cmd);
}

