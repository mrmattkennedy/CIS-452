#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>

#define SIZE 16

int main (int argc, char** argv)
{
	int status;
	long int i, loop, temp, *shmPtr;
	int shmId;
	pid_t pid;

	if (argc != 2){
		perror("need 2 arguments\n");
		exit(1);
	}
	
	loop = atoi(argv[1]);
	printf("%lu\n", loop);

	if ((shmId = shmget (IPC_PRIVATE, SIZE, IPC_CREAT|S_IRUSR|S_IWUSR)) < 0) {
		perror ("i can't get no..\n");
		exit (1);
	}
	if ((shmPtr = shmat (shmId, 0, 0)) == (void*) -1) {
		perror ("can't attach\n");
		exit (1);
	}

	shmPtr[0] = 0;
	shmPtr[1] = 1;

	if (!(pid = fork())) {
		for (i=0; i<loop; i++) {
			//a=5, b=10, a+b=15, b=a-b=5, a=a-b=10
			shmPtr[0] += shmPtr[1];
			shmPtr[1] = shmPtr[0]-shmPtr[1];
			shmPtr[0] = shmPtr[0]-shmPtr[1];
		}
		if (shmdt (shmPtr) < 0) {
			perror ("just can't let go\n");
			exit (1);
		}
		exit(0);
	}
	else
		for (i=0; i<loop; i++) {
			shmPtr[0] += shmPtr[1];
			shmPtr[1] = shmPtr[0]-shmPtr[1];
			shmPtr[0] = shmPtr[0]-shmPtr[1];
		}

	wait (&status);
	printf ("values: %li\t%li\n", shmPtr[0], shmPtr[1]);

	if (shmdt (shmPtr) < 0) {
		perror ("just can't let go\n");
		exit (1);
	}
	if (shmctl (shmId, IPC_RMID, 0) < 0) {
		perror ("can't deallocate\n");
		exit(1);
	}

	return 0;
}
