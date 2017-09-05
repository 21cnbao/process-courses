#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>

void *thread_fun(void *param)
{
	printf("thread pid:%d, tid:%lu\n", getpid(), pthread_self());
	while (1) ;
	return NULL;
}

int main(void)
{
	pthread_t tid1, tid2;
	int ret;

	printf("main pid:%d, tid:%lu\n", getpid(), pthread_self());

	ret = pthread_create(&tid1, NULL, thread_fun, NULL);
	if (ret == -1) {
		perror("cannot create new thread");
		return 1;
	}

	ret = pthread_create(&tid2, NULL, thread_fun, NULL);
	if (ret == -1) {
		perror("cannot create new thread");
		return 1;
	}

	if (pthread_join(tid1, NULL) != 0) {
		perror("call pthread_join function fail");
		return 1;
	}

	if (pthread_join(tid2, NULL) != 0) {
		perror("call pthread_join function fail");
		return 1;
	}

	return 0;
}
