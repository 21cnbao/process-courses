#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>

struct thread_param {
	char info;
	int num;
};

void *thread_fun(void *param)
{
	printf("thread pid:%d, tid:%lu\n", getpid(), pthread_self());
	return NULL;
}

int main(void)
{
	pthread_t tid;
	int ret;

	printf("main pid:%d, tid:%lu\n", getpid(), pthread_self());

	ret = pthread_create(&tid, NULL, thread_fun, NULL);
	if (ret == -1) {
		perror("cannot create new thread");
		return -1;
	}

	if (pthread_join(tid, NULL) != 0) {
		perror("call pthread_join function fail");
		return -1;
	}

	return 0;
}
