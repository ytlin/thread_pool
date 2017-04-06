#include <stdlib.h>  /* qsort() */
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>  /* memcpy()  */
#include <sys/time.h>  /* gettimeofday()  */
#include <assert.h>     /* assert */

struct params {
	int left;
	int right;
};

int cmp(const void *a, const void *b);

void swap(int *a, int *b);

void *quick_sort();

void bubble_sort(int *array, int left, int right);

/** HW4 */
struct job
{
	void* (*function)(void *arg);
	void *arg;
	struct job *next;     //job queue list
};

struct threadpool
{
	int thread_num;
	int queue_max_num;
	int queue_cur_num;
	int queue_close;
	int pool_close;
	struct job *head;  //job queue head ptr	
	struct job *tail;  //job queue tail ptr	
	pthread_t *threads;
	pthread_mutex_t mut;
	pthread_cond_t queue_empty;
	pthread_cond_t queue_not_empty;
	pthread_cond_t queue_not_full;
};

struct threadpool* threadpool_init(int thread_num, int queue_max_num);

void* threadpool_function(void* arg);

int threadpool_add_job(struct threadpool* pool, void* (*function)(void *arg), void *arg);

int threadpool_destroy(struct threadpool *pool);

void* work1(void *arg);

/** end of HW4*/

/* global var  */
int array[1200000];
struct params thread_params[16];
sem_t mutex[15];   /* 0 for id setting, 1~14 for Tasks */
sem_t mutex_id;
sem_t event_id;
int thread_id=1;
int global_id;
int task_id[16];
/* end of dealcare*/


int main()
{
	/*  var declare */
	int num_array;
	int i;
	/*  init thread[]  */
	for(i=0;i<=15;i++)
	{
		thread_params[i].left = -100;
		thread_params[i].right = -100;
		task_id[i] = i;
	}
	FILE* pfile_in;
	FILE* pfile_out_S;
	FILE* pfile_out_pool[8];
	struct timeval start, end;
	int sec;
	int usec;
	/* get the value of array*/
	//fgets(input_name, 100, stdin);
	//if(input_name[strlen(input_name)-1] == '\n')
	//	input_name[strlen(input_name)-1] = '\0';
	pfile_in = fopen("input.txt", "r");
//	pfile_out_S = fopen("outputS.txt", "w");
	pfile_out_pool[0] = fopen("output_1.txt", "w");
	pfile_out_pool[1] = fopen("output_2.txt", "w");
	pfile_out_pool[2] = fopen("output_3.txt", "w");
	pfile_out_pool[3] = fopen("output_4.txt", "w");
	pfile_out_pool[4] = fopen("output_5.txt", "w");
	pfile_out_pool[5] = fopen("output_6.txt", "w");
	pfile_out_pool[6] = fopen("output_7.txt", "w");
	pfile_out_pool[7] = fopen("output_8.txt", "w");
	fscanf(pfile_in, "%d", &num_array);
	for(i=0;i<num_array;i++){
		fscanf(pfile_in, "%d", &array[i]);
	}
	/* copy to array2  */
	int *array2;
	array2 = malloc(sizeof(int)*num_array);
	memcpy(array2, array, num_array*sizeof(int));
	
	sem_init(&mutex_id, 0, 1);
	sem_init(&event_id, 0, 0);
	for(i=1;i<=8;i++){
		/* init global id*/
		global_id = 1;
		memcpy(array, array2, num_array*sizeof(int));
		/* pool init  */	
		struct threadpool *pool = threadpool_init(i, 18);
		/* master thread add first job */
		thread_params[0].left = 0;
		thread_params[0].right = num_array-1;
		gettimeofday(&start, 0);	/* timer stop */
	    	threadpool_add_job(pool, work1, &task_id[global_id]);
		int k;
	        for(k=0;k<7;k++)
		{
			sem_wait(&event_id);  //wake up by task done
	    				threadpool_add_job(pool, work1, &task_id[global_id*2]);
	    				threadpool_add_job(pool, work1, &task_id[global_id*2+1]);
			sem_post(&mutex_id);
		}
		/* destroy pool  */
		threadpool_destroy(pool);
		gettimeofday(&end, 0);	/* timer stop */
		sec = end.tv_sec - start.tv_sec;
		usec = end.tv_usec - start.tv_usec;
		printf("%d threads elapsed time: %f s \n", i, sec+(usec/1000000.0));
		for(k=0;k<num_array;k++){
			fprintf(pfile_out_pool[i-1], "%d ", array[k]);
		}
	}
//	/* init the thread_params[0] */
//	thread_params[0].left = 0;
//	thread_params[0].right = num_array-1;
//	gettimeofday(&start, 0);  /* timer start */
//	/* create 15 threads  */
//	sem_init(&mutex[0], 0, 1);	/*init value 1 ,let first thread go into*/
//	for(i=1;i<15;i++)		/* for events */
//		sem_init(&mutex[i], 0, 0);		
//	for(i=0;i<15;i++)
//	{
//		rc = pthread_create(&threads[i], NULL, quick_sort, NULL);
//		if (rc){
//			printf("ERROR; return code from pthread_create() is %d\n", rc);
//			exit(-1);
//		}
//	}
//
//	void *ret;
//	for(i=0;i<15;i++)
//		pthread_join(threads[i], &ret);
//
//	gettimeofday(&end, 0);	/* timer stop */
//	int sec = end.tv_sec - start.tv_sec;
//	int usec = end.tv_usec - start.tv_usec;
//
//	//	printf("return value from thread1 = %d\n",*(int *)ret);	
//
//	/*  multi thread wirte to output2.txt  */
//	printf("Multi-threads elapsed time: %f s \n", sec+(usec/1000000.0));

//	/* single thread wirte to output1.txt */
//	gettimeofday(&start, 0);  /* timer start */
//	//qsort(array2, num_array, sizeof(int), cmp);
//	bubble_sort(array2, 0, num_array-1);
//	gettimeofday(&end, 0);	/* timer stop */
//	sec = end.tv_sec - start.tv_sec;
//	usec = end.tv_usec - start.tv_usec;
//	printf("Single thread elapsed time: %f s \n", sec+(usec/1000000.0));
//	for(i=0;i<num_array;i++){
//		fprintf(pfile_out_S, "%d ", array2[i]);
//	}

	/*  destroy sem  */
	for(i=0;i<15;i++)
		sem_destroy(&mutex[i]); 
	/* free array2  */
	free(array2);
	/*  close files */
	fclose(pfile_in);	
//	fclose(pfile_out_S);	
	fclose(pfile_out_pool[0]);	
	fclose(pfile_out_pool[1]);	
	fclose(pfile_out_pool[2]);	
	fclose(pfile_out_pool[3]);	
	fclose(pfile_out_pool[4]);	
	fclose(pfile_out_pool[5]);	
	fclose(pfile_out_pool[6]);	
	fclose(pfile_out_pool[7]);	
}	

/****  ==============function implements===================  *****/

int cmp(const void *a, const void *b)
{
	return (*(int*)a - *(int*)b);
}

void swap(int *a, int *b)
{
	int tmp = *a;
	*a = *b;
	*b = tmp;
}


void *quick_sort()
{
	int id;
	int left, right;
	int k;    
	sem_wait(&mutex[0]);   //start 
	id = thread_id++;
	sem_post(&mutex[0]);   // end
	/*  wait until array ready  */

	if(id != 1)sem_wait(&mutex[id-1]);   //start 
	left = thread_params[id-1].left;
	right = thread_params[id-1].right;
	if(id <8){
		if (left < right)
		{
			int pivot = array[(left+right)/2];  
			int i = left - 1, j = right + 1;
			while (i < j)
			{
				do ++i; while (array[i] < pivot);
				do --j; while (array[j] > pivot);
				if (i < j) swap(&array[i], &array[j]);
			}
			thread_params[id*2-1].left = left;
			thread_params[id*2-1].right = i-1;
			thread_params[id*2].left = j+1;
			thread_params[id*2].right = right;
		}else   /* fill 0 , in order start up the redundent Tn */
		{
			thread_params[id*2-1].left = 0;
			thread_params[id*2-1].right = 0;
			thread_params[id*2].left = 0;
			thread_params[id*2].right = 0;	
		}
		sem_post(&mutex[id*2-1]);
		sem_post(&mutex[id*2]);
		if(id != 1)sem_post(&mutex[id-1]);
	}else{
		//qsort(array+left, right-left+1, sizeof(int), cmp);  /* first arg:array+left!! */
		bubble_sort(array, left, right);
	}

	pthread_exit(0);

}

void bubble_sort(int *array, int left, int right)
{
	int i,j,tmp;
	array += left;
	int n = right -left +1;

	for (i = 0 ; i < ( n - 1 ); i++)
	{
		for (j = 0 ; j < n - i - 1; j++)
		{
			if (array[j] > array[j+1]) /* For decreasing order use < */
			{
				tmp       = array[j];
				array[j]   = array[j+1];
				array[j+1] = tmp;
			}
		}
	}	
}

//============= HW4 implement  =================
struct threadpool* threadpool_init(int thread_num, int queue_max_num)
{
	struct threadpool *pool;
	pool = malloc(sizeof(struct threadpool));
	pool->thread_num = thread_num;
	pool->queue_max_num = queue_max_num;
	pool->queue_cur_num = 0;
	pool->head = NULL;
	pool->tail = NULL;
	/** init semaphore */
	pthread_mutex_init(&(pool->mut), NULL);
	pthread_cond_init(&(pool->queue_empty), NULL);
	pthread_cond_init(&(pool->queue_not_empty), NULL);
	pthread_cond_init(&(pool->queue_not_full), NULL);
	/** malloc pthread_t  */
	pool->threads = malloc(sizeof(pthread_t)*thread_num);
	/** open queue, thread pool*/
	pool->queue_close = 0;
	pool->pool_close = 0;

	/** create my thread wrapper function */
	int i;
	for(i=0;i<thread_num;i++)
		pthread_create(&(pool->threads[i]), NULL, threadpool_function, (void*)pool);

	return pool;

}

void* threadpool_function(void* arg)
{
	struct threadpool *pool = (struct threadpool*)arg;
	struct job *thread_job;	
	while(1){
		pthread_mutex_lock(&(pool->mut));
		while(!(pool->pool_close) && pool->queue_cur_num == 0)
			pthread_cond_wait(&(pool->queue_not_empty), &(pool->mut));//wake up by job add functi
		if(pool->pool_close){    // queue closed, so just exit

			pthread_mutex_unlock(&(pool->mut));
			pthread_exit(NULL);
		}
		//queue not empty and queue not close, go below (mut is still locked !)
		// take one job from queue
		pool->queue_cur_num--;
		thread_job = pool->head;
		//adjust the queue
		if(pool->queue_cur_num ==0){
			pool->head = NULL;
			pool->tail = NULL;

		}else{
			pool->head = pool->head->next;
		}
		//signal event
		if(pool->queue_cur_num == 0){
			pthread_cond_signal(&(pool->queue_empty));  //signal empty event, if destroy is waiting, wake up it
		}
		if(pool->queue_cur_num == pool->queue_max_num -1){
			pthread_cond_broadcast(&(pool->queue_not_full));  //wake up all **
		}
		pthread_mutex_unlock(&(pool->mut));
		
		//execute the real function
		//printf("do %d\n", *((int *)(thread_job->arg)));
		fflush(stdout);
		(*(thread_job->function))(thread_job->arg);
		//printf("%d , done\n", *((int *)(thread_job->arg)));
		fflush(stdout);
		if(*((int *)thread_job->arg) < 8)
		{
			sem_wait(&mutex_id);
			global_id = *((int *)thread_job->arg); 
			sem_post(&event_id);
		}
		free(thread_job);
	}

}

int threadpool_add_job(struct threadpool* pool, void* (*function)(void *arg), void *arg)
{
	// ensure the function paramater is not NULL
	assert(pool != NULL);
	assert(function != NULL);
	//assert(arg != NULL);

	//go into queue, first lock mut
	pthread_mutex_lock(&(pool->mut));
	//if full, wait
	while(!(pool->queue_close||pool->pool_close) && pool->queue_cur_num==pool->queue_max_num)
	{
		pthread_cond_wait(&(pool->queue_not_full), &(pool->mut));
	}
	if(pool->queue_close || pool->pool_close)
	{
		pthread_mutex_unlock(&(pool->mut));
		return -1;
	}
	// start to add job to the queue
	struct job *thread_job = (struct job*)malloc(sizeof(struct job));
	thread_job->function = function;
	thread_job->arg = arg;
	thread_job->next = NULL;
	
	if(pool->head == NULL)
	{
		pool->head = thread_job;
		pool->tail = thread_job;
		pthread_cond_broadcast(&(pool->queue_not_empty));
	}else{
		pool->tail->next = thread_job;
		pool->tail =thread_job;
	}
	pool->queue_cur_num++;
	pthread_mutex_unlock(&(pool->mut));
}

int threadpool_destroy(struct threadpool *pool)
{
	assert(pool!=NULL);
	pthread_mutex_lock(&(pool->mut));
	
	//already close, return
	if(pool->queue_close || pool->pool_close){
		pthread_mutex_unlock(&(pool->mut));
		return -1;
	}
	
	//close queue
	pool->queue_close = 1;
	// if current not empty yet, wait the empty event
	while(pool->queue_cur_num != 0)
		pthread_cond_wait(&(pool->queue_empty), &(pool->mut));

	pool->pool_close = 1;
	pthread_mutex_unlock(&(pool->mut));
	pthread_cond_broadcast(&(pool->queue_not_empty)); //wake up all, they will notice that queue close
	pthread_cond_broadcast(&(pool->queue_not_full)); //wake up all, they will notice that queue close
	
	int i;
	// collect the body
	for(i=0;i<pool->thread_num;i++){
		pthread_join(pool->threads[i], NULL);
	}

	//clean semaphore
	pthread_mutex_destroy(&(pool->mut));
	pthread_cond_destroy(&(pool->queue_empty));
    	pthread_cond_destroy(&(pool->queue_not_empty));   
    	pthread_cond_destroy(&(pool->queue_not_full));
	free(pool->threads);
        struct job *p;
        while (pool->head != NULL)
        {
        	p = pool->head;
        	pool->head = p->next;
        	free(p);
        }
    	free(pool);
}

void* work1(void *arg)
{
	int id = *((int *)arg);
	int left=thread_params[id-1].left, right=thread_params[id-1].right;
	int k; 
	if(id<8){   
		if (left < right)
		{
			int pivot = array[(left+right)/2];  
			int i = left - 1, j = right + 1;
			while (i < j)
			{
				do ++i; while (array[i] < pivot);
				do --j; while (array[j] > pivot);
				if (i < j) swap(&array[i], &array[j]);
			}
			thread_params[id*2-1].left = left;
			thread_params[id*2-1].right = i-1;
			thread_params[id*2].left = j+1;
			thread_params[id*2].right = right;
		}else   /* fill 0 , in order start up the redundent Tn */
		{
			thread_params[id*2-1].left = 0;
			thread_params[id*2-1].right = 0;
			thread_params[id*2].left = 0;
			thread_params[id*2].right = 0;	
		}
	}else{
		//qsort(array+left, right-left+1, sizeof(int), cmp);  /* first arg:array+left!! */
		bubble_sort(array, left, right);
	}
}
