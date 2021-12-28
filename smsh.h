define YES 1
#define NO 0

    char *
    next_cmd();
char **splitline(char *);
void freelist(char **);
void *emalloc(size_t);
void *erealloc(void *, size_t);
int execute(char **);
void fatal(char *, char *, int);
char *get_username();
char *get_prompt(char *, char *);
char *update_location(ino_t, char *, int);
ino_t get_inode(char *);
void exit_shell(char **);
void cd(char **);
char **trim_array(char **, char **, int);
int get_args_num(char **);
void handle_child(int);

int process();
