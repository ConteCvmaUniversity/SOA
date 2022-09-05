
//include
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <ctype.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <signal.h>


//define
#define CLEAN_SHELL "\033[2J\033[H"
#define TITLE       "\t\tMulti-flow device file\n"
#define SIZE        300
#define READSIZE    4

#define MAGIC_NUMBER 'p'//use in ioctl (best major fixed?)
#define SET_HIGH_PR _IO(MAGIC_NUMBER,0x00)
#define SET_LOW_PR _IO(MAGIC_NUMBER,0x01)
#define SET_OP_BLOCK _IO(MAGIC_NUMBER,0x02)
#define SET_OP_NONBLOCK _IO(MAGIC_NUMBER,0x03)
#define SET_TIMEOUT_BLOCK _IOW(MAGIC_NUMBER,0x04,int32_t*)

//global define
static volatile sig_atomic_t signo;
typedef struct sigaction sigaction_t;
#define min(X, Y) (((X) < (Y)) ? (X) : (Y))

char* opt[] = {
    "1) Write on device file ",
    "2) Read on device file",
    "3) Set high priority",
    "4) Set low priority",
    "5) Set operation blocking",
    "6) Set operation non blocking",
    "7) Set timer value",
    "8) Quit"
};


int num_opt = sizeof(opt) / sizeof(char *);

char opt_n[8]={'1','2', '3','4','5','6','7','8'};

char io_buf[SIZE];

void clean_stdin(){
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

//function
void menu();
int work(int,char*);
int multiChoice(char *domanda, char choices[], int num);
static void handler(int s);
char *getInput(unsigned int lung, char *stringa, bool hide);

int main(int argc, char** argv){
    
    int fd,ret;
    
    if(argc < 2){
        printf("Wrong usage, try: %s dev_path\n",argv[0]);
        return -1;
    }
    
    //try open device
    printf("Opening file at path %s\n",argv[1]);
    fd = open(argv[1],O_RDWR);
    if (fd == -1)
    {
        printf("open error on device %s\n",argv[1]);
		exit(-1);
    }
    printf("Complete opening file at path %s\n",argv[1]);

    sleep(1);

    ret = work(fd,argv[1]);

    close(fd);

    return ret;

}


int work(int fd,char* file_name){
    int opt;
    int ret=0;
    memset(io_buf,'\0',SIZE-1);
    
    //menu show and run command
    while (1)
    {
        //clean buffer
        memset(io_buf,0,strlen(io_buf));
        menu();
        opt = multiChoice("Select an option", opt_n, num_opt);
        //clean_stdin();

        //clean buffer for get opt argument
        memset(io_buf,0,strlen(io_buf));
        switch (opt)
        {
        case 1:
            printf("\tWRITE on device\n");
            printf("Insert data you want to write (maxsize:%d): ",SIZE);
            getInput(SIZE,io_buf,false);
            
            //write on device
            
            ret = write(fd,io_buf,min(strlen(io_buf), SIZE));
            if (ret == 0 || ret == -1){
                printf("Write operation failed\n");
                
            }else
            {
                printf("Write operation completed\n");
            }
            memset(io_buf, 0x0, strlen(io_buf));
            sleep(2);
            break;
        case 2:
            printf("\tREAD device\n");
            printf("Insert number of byte you want to read (maxsize:%d): ",SIZE);
            getInput(SIZE,io_buf,false);

            long data_len;
            data_len = strtol(io_buf, NULL, 10);
            if (errno == ERANGE)
            {
                fprintf(stderr, "Invalid conversion at %s:%d\n", __FILE__, __LINE__);
            }else
            {
                //do read
                memset(io_buf, 0x0, strlen(io_buf)); //clean io buf
                ret = read(fd, io_buf, min(data_len, SIZE));
                if (ret == 0 || ret == -1)
                    printf("\nRead return a 0 value or error \n");
                else{
                    printf("\nRead %d bytes: %s\n\n", ret,io_buf);
                    memset(io_buf, 0x0, strlen(io_buf));
                }
            }   
            printf("Press Enter to Continue");
            while( getchar() != '\n' );        
            break;
        case 3:
            printf("\tSET HIGH PRIORITY\n");
            printf("Setting priority HIGH to device %s\n",file_name);
            ioctl(fd, SET_HIGH_PR); 
            sleep(2);  
            break;
        case 4:
            printf("\tSET LOW PRIORITY\n");
            printf("Setting priority HIGH to device %s\n",file_name);
            ioctl(fd, SET_LOW_PR); 
            sleep(2);  
            break;
        case 5:
            printf("Setting operation blocking to device %s\n",file_name);
            ioctl(fd, SET_OP_BLOCK); 
            sleep(2);
            break;
        case 6:
            printf("\tSET non blocking operation\n");
            printf("Setting operation non blocking to device %s\n",file_name);
            ioctl(fd, SET_OP_NONBLOCK); 
            sleep(2);
            break;
        case 7:
            printf("\tSET timer\n");
            printf("Insert timer new value (max byte:%d): ",5);
            getInput(5,io_buf,false);

            long timer;
            timer = strtol(io_buf, NULL, 10);
            if (errno == ERANGE || timer < 0) 
            {
                fprintf(stderr, "Invalid conversion at %s:%d\n", __FILE__, __LINE__);
            }else
            {
                printf("Setting timeout for device %s to value %ld\n",file_name,timer);
                ioctl(fd, SET_TIMEOUT_BLOCK,(int32_t*) &timer); 
            }

            sleep(2);
            break;
        case 8:
            printf("\tEXIT program\n");
            goto EXIT; 
        }    
    }

EXIT: 
    return ret;
}


void menu(){
    printf(CLEAN_SHELL);
    printf(TITLE);
    for (int i = 0; i < num_opt; i++)
    {
        printf("%s\n",opt[i]);
    }
    
}

//choice a una sola cifra
int multiChoice(char *domanda, char choices[], int num)
{

	// Genera la stringa delle possibilit�
	char *possib = malloc(2 * num * sizeof(char));
	int i, j = 0;
	for(i = 0; i < num; i++) {
		possib[j++] = choices[i];
		possib[j++] = '/';
	}
	possib[j-1] = '\0'; // Per eliminare l'ultima '/'

	// Chiede la risposta
	while(true) {
		// Mostra la domanda
		printf("%s [%s]: ", domanda, possib);

        //valido fino a quando le possibilità sono a una sola cifra
		char c;
		getInput(1, &c, false);

		// Controlla se � un carattere valido
		for(i = 0; i < num; i++) {
			if(c == choices[i])
				return c - '0';
		}
	}
}


char *getInput(unsigned int lung, char *stringa, bool hide)
{
	char c;
	unsigned int i;

	// Dichiara le variabili necessarie ad un possibile mascheramento dell'input
	sigaction_t sa, savealrm, saveint, savehup, savequit, saveterm;
	sigaction_t savetstp, savettin, savettou;
	struct termios term, oterm;

	if(hide) {
		// Svuota il buffer
		(void) fflush(stdout);

		// Cattura i segnali che altrimenti potrebbero far terminare il programma, lasciando l'utente senza output sulla shell
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = SA_INTERRUPT; // Per non resettare le system call
		sa.sa_handler = handler;
		(void) sigaction(SIGALRM, &sa, &savealrm);
		(void) sigaction(SIGINT, &sa, &saveint);
		(void) sigaction(SIGHUP, &sa, &savehup);
		(void) sigaction(SIGQUIT, &sa, &savequit);
		(void) sigaction(SIGTERM, &sa, &saveterm);
		(void) sigaction(SIGTSTP, &sa, &savetstp);
		(void) sigaction(SIGTTIN, &sa, &savettin);
		(void) sigaction(SIGTTOU, &sa, &savettou);
	
		// Disattiva l'output su schermo
		if (tcgetattr(fileno(stdin), &oterm) == 0) {
			(void) memcpy(&term, &oterm, sizeof(struct termios));
			term.c_lflag &= ~(ECHO|ECHONL);
			(void) tcsetattr(fileno(stdin), TCSAFLUSH, &term);
		} else {
			(void) memset(&term, 0, sizeof(struct termios));
			(void) memset(&oterm, 0, sizeof(struct termios));
		}
	}
	
	// Acquisisce da tastiera al pi� lung - 1 caratteri
	for(i = 0; i < lung; i++) {
		(void) fread(&c, sizeof(char), 1, stdin);
		if(c == '\n') {
			stringa[i] = '\0';
			break;
		} else
			stringa[i] = c;

		// Gestisce gli asterischi
		if(hide) {
			if(c == '\b') // Backspace
				(void) write(fileno(stdout), &c, sizeof(char));
			else
				(void) write(fileno(stdout), "*", sizeof(char));
		}
	}
	
	// Controlla che il terminatore di stringa sia stato inserito
	if(i == lung - 1)
		stringa[i] = '\0';

	// Se sono stati digitati pi� caratteri, svuota il buffer della tastiera
	if(strlen(stringa) >= lung) {	
		// Svuota il buffer della tastiera
		do {
			c = getchar();
		} while (c != '\n'&& c != EOF);
	}

	if(hide) {
		//L'a capo dopo l'input
		(void) write(fileno(stdout), "\n", 1);

		// Ripristina le impostazioni precedenti dello schermo
		(void) tcsetattr(fileno(stdin), TCSAFLUSH, &oterm);

		// Ripristina la gestione dei segnali
		(void) sigaction(SIGALRM, &savealrm, NULL);
		(void) sigaction(SIGINT, &saveint, NULL);
		(void) sigaction(SIGHUP, &savehup, NULL);
		(void) sigaction(SIGQUIT, &savequit, NULL);
		(void) sigaction(SIGTERM, &saveterm, NULL);
		(void) sigaction(SIGTSTP, &savetstp, NULL);
		(void) sigaction(SIGTTIN, &savettin, NULL);
		(void) sigaction(SIGTTOU, &savettou, NULL);

		// Se era stato ricevuto un segnale viene rilanciato al processo stesso
		if(signo)
			(void) raise(signo);
	}
	
	return stringa;
}

// Per la gestione dei segnali
static void handler(int s) {
	signo = s;
}