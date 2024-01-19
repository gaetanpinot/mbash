#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>

int main() {
	char *isexe="/home/user/Documents/alphabet.c";
	struct stat sb;
	if(	stat(isexe, &sb) == 0 && sb.st_mode & S_IXUSR){ 
		printf("%s is%s executable.\n", isexe,  "" );
	}
   return 0;
}

