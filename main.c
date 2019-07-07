#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

#define min(a,b) ((a)>(b))?(b):(a)

struct Args
{
	int need;
	int number;
};

int products[5];
pthread_mutex_t m[5]={
	PTHREAD_MUTEX_INITIALIZER,
	PTHREAD_MUTEX_INITIALIZER,
	PTHREAD_MUTEX_INITIALIZER,
	PTHREAD_MUTEX_INITIALIZER,
	PTHREAD_MUTEX_INITIALIZER
};

void *Customer(void *ptr)
{
	struct Args *args=(struct Args*)ptr;
	int mn;

	while(1)
	{
		for (int i=0;i<5;i++)
		{
			if (pthread_mutex_trylock(&m[i])==0)
			{
				if (products[i]!=0)
				{
					printf("Customer %d needs %d \n",(*args).number,(*args).need);

					mn=min((*args).need,products[i]);
					(*args).need-=mn;
					products[i]-=mn;

					printf("Customer %d bought %d from shop %d and needs %d more\n",(*args).number,mn,i+1,(*args).need);

					pthread_mutex_unlock(&m[i]);

					if ((*args).need==0)
					{
						printf("Customer %d bought everything he wanted!\n",(*args).number);

						pthread_exit(0);
					}

					sleep(3);
				}
				else
				{
					printf("Customer %d couldn't buy products! Shop %d is empty!\n",(*args).number,i);
				}
			}
		}
	}
}

void *Mover(void *ptr)
{
	int add;

	while(1)
	{
		for (int i=0;i<5;i++)
		{
			if (pthread_mutex_trylock(&m[i])==0)
			{
				add=400+rand()%101;
				products[i]+=add;

				printf("Mover add %d to %d\n",add,i+1);

				pthread_mutex_unlock(&m[i]);
				sleep(1.5);
			}
		}
	}
}

int main()
{
	struct Args args[3];
	pthread_t tid[4];
	srand(time(NULL));
	int status;

	printf("Products:\n");

	for (int i=0;i<5;i++)
	{
		products[i]=900+rand()%201;
		printf("\t%d) %d\n",i+1,products[i]);
	}

	printf("\nCustomers need:\n");

	for (int i=0;i<3;i++)
	{
		args[i].need=9000+rand()%2001;
		args[i].number=i+1;
		printf("\t%d) %d\n",i+1,args[i].need);
	}

	printf("\n\n");

	for (int i=0;i<3;i++)
	{
		pthread_create(&tid[i],NULL,Customer,(void *)&args[i]);
	}

	pthread_create(&tid[3],NULL,Mover,NULL);

	for (int i=0;i<3;i++)
	{
		pthread_join(tid[i],(void *)&status);
	}


	printf("\n\nAll customers left!\n");
	pthread_cancel(tid[3]);
	printf("Mover done!\n");

	return 0;
}
