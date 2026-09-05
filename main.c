#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/* FUNCTION DECLARATIONS AND LIST OF BUILTIN COMMANDS */

int dsh_cd(char **args);
int dsh_help(char **args);
int dsh_exit(char **args);

char *builtin_str[] = {
	"cd",
	"help",
	"exit"
};

int (*builtin_func[]) (char **) = {
	&dsh_cd,
	&dsh_help,
	&dsh_exit
};

int dsh_num_builtins() {
	return sizeof(builtin_str) / sizeof(char *);
}

int dsh_cd(char **args) {
	if (args[1] == NULL){
		fprintf(stderr, "dsh: Expected argument to \"cd\"\n");
	} else {
		if (chdir(args[1]) != 0){
			perror("dsh");
		}
	}
	return 1;
}

int dsh_help(char **args) {
	int i;

	printf("dsh Shell created following Brendan's Tutorials\n");
	printf("Type program names and arguments and hit enter\n");
	printf("The following are built in:\n");

	for (i=0; i < dsh_num_builtins(); i++){
		printf(" %s\n", builtin_str[i]);
	}

	printf("Use the MAN command for information on other programs\n");
	return 1;
}

int dsh_exit(char **args) {
	return 0;
}

#define DSH_RL_BUFSIZE 1024
char *dsh_read_line(void) {
	int bufsize = DSH_RL_BUFSIZE;
	int position = 0;
	char *buffer = malloc(sizeof(char) * bufsize);
	int character;

	if (!buffer) {
		fprintf(stderr, "dsh: allocations error\n");
		exit(EXIT_FAILURE);
	}

	while(1) {
		character = getchar();

		if (character == EOF || character == '\n') {
			buffer[position] = '\0';
			return buffer;
		} else {
			buffer[position] = character;
		}
		position++;

		if (position >= bufsize) {
			bufsize += DSH_RL_BUFSIZE;
			buffer = realloc(buffer, bufsize);
			if (!buffer) {
				fprintf(stderr, "dsh: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
	}
}

#define DSH_TOK_BUFSIZE 64
#define DSH_TOK_DELIM " \t\r\n\a"
char **dsh_split_line(char *line) {
	int bufsize = DSH_TOK_BUFSIZE, position = 0;
	char **tokens = malloc(sizeof(char*) * bufsize);
	char *token;

	if (!tokens){
		fprintf(stderr, "dsh: allocation error\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, DSH_TOK_DELIM);
	while(token != NULL) {
		tokens[position] = token;
		position++;

		if (position >= bufsize){
			bufsize += DSH_TOK_BUFSIZE;
			tokens = realloc(tokens, bufsize * sizeof(char*));
			if (!tokens){
				fprintf(stderr, "dsh: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
		token = strtok(NULL, DSH_TOK_DELIM);
	}
	tokens[position] = NULL;
	return tokens;
}

int dsh_lauch(char **args) {
	pid_t pid, wpid;
	int status;

	pid = fork();
	if (pid == 0){
		if (execvp(args[0], args) == -1){
			perror("dsh");
		}
		exit(EXIT_FAILURE);
	} else if (pid < 0){
		perror("dsh");
	} else {
		do {
			wpid = waitpid(pid, &status, WUNTRACED);
		} while(!WIFEXITED(status) && !WIFSIGNALED(status));
	}

	return 1;
}


int dsh_execute(char **args) {
	int i;

	if (args[0] == NULL){
		return 1;
	}

	for (i=0; i < dsh_num_builtins(); i++){
		if (strcmp(args[0], builtin_str[i]) == 0){
			return (*builtin_func[i])(args);
		}
	}

	return dsh_lauch(args);
}

void dsh_loop(void) {
	char *line;
	char **args;
	int status;

	do {
		printf("> ");
		line = dsh_read_line();
		args = dsh_split_line(line);
		status = dsh_execute(args);

		free(line);
		free(args);

	} while(status);
}

int main(int argc, char **argv) {
	
	dsh_loop();

	return EXIT_SUCCESS;
}


