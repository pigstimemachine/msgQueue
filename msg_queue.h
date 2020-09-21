#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#ifndef __MSG_QUEUE_H
#define __MSG_QUEUE_H

#define printd(format,args...)  printf(format, ##args)
#define MAX_NAME_LEN 32
struct msg_queue {
	    void** buffer; 
	    int size; 
	    int lget; 
	    int lput; 
	    int ndata;
	    int nfullthread; 
	    int nemptythread; 
	    int init_done;
	    pthread_mutex_t mux;
	    pthread_cond_t cond_get, cond_put;
	    char name[MAX_NAME_LEN+1];
};
 
static inline void* get_queue(struct msg_queue *q,int timeo)
{
	    void* data = NULL;
	    struct timeval now;
	    struct timespec outtime;
	    int ret;
	    
	    pthread_mutex_lock(&q->mux);
	    while(q->lget == q->lput && 0 == q->ndata){
	    		q->nemptythread++;
	    		gettimeofday(&now, NULL);
			outtime.tv_sec = now.tv_sec + timeo/1000;
			outtime.tv_nsec = (now.tv_usec +(timeo%1000)*1000)* 1000;
			//usleep(1000);
			//printf("");
		       ret = pthread_cond_timedwait(&q->cond_get, &q->mux,&outtime);
		       q->nemptythread--;
		       if(ret){
				pthread_mutex_unlock(&q->mux);
				return NULL;
		       }
	    }
	    
	 //   printd("get data! lget:%d", q->lget);

	    data = (q->buffer)[q->lget++];
	    if(q->lget == q->size){
	        	q->lget = 0;
	    }
	    q->ndata--;

	//    printd(" nData:%d\n", q->ndata);

	    if(q->nfullthread){
	        	pthread_cond_signal(&q->cond_put);
	    }
	    
	    pthread_mutex_unlock(&q->mux);
	    return data;
}
 
static inline void put_queue(struct msg_queue *q, void* data)
{
	    pthread_mutex_lock(&q->mux);
	    while(q->lget == q->lput && q->ndata){
			printf("WARNNING!!!!!%s is full\n",q->name);
		        q->nfullthread++;
		        pthread_cond_wait(&q->cond_put, &q->mux);
		        q->nfullthread--;
	    }

	//    printd("put data! lput:%d", q->lput);

	    (q->buffer)[q->lput++] = data;
		    if(q->lput == q->size){
		        q->lput = 0;
	    }
	    q->ndata++;

	//    printd(" nData:%d\n", q->ndata);

	    if(q->nemptythread){
	        	pthread_cond_signal(&q->cond_get);
	    }
	    pthread_mutex_unlock(&q->mux);
}

static inline void init_queue(struct msg_queue *q,int queue_len,char *name)
{
	if(q->init_done)
		return ;
	memset((void *)q,0,sizeof(struct msg_queue));
	pthread_mutex_init(&q->mux, 0);
    	pthread_cond_init(&q->cond_get, 0);
    	pthread_cond_init(&q->cond_put, 0);
    	q->size = queue_len;
    	q->buffer = malloc(q->size * sizeof(void*));
    	if(q->buffer == NULL){
        	printf("malloc failed!\n");
        	exit(-1);
    	}
    	q->init_done = 1;
	if(strlen(name)<MAX_NAME_LEN)
	{
		strncpy(q->name,name,strlen(name));
	}
	else
		strncpy(q->name,name,MAX_NAME_LEN-1);
    	return;
}

static inline void destroy_queue(struct msg_queue *q)
{
	if(!q->init_done)
		return;
	
	free(q->buffer);
	pthread_mutex_destroy(&q->mux);
	pthread_cond_destroy(&q->cond_get);
	pthread_cond_destroy(&q->cond_put);
	q->init_done = 0;
	return;
}



struct msg_queue AS4_upload_log_queue;


#endif
/*


  * @brief   	how to use it ,and where to use it
  * @Name	 	int main Name ( void sth)
  * @param   	None
  * @retval  	None
  * @Author  	ZCJ
  * @Date 	 	2020/06/ Create
  * @Version 	1.0 2020/07/02  
  *		     	1.1 2020/07/ change sth
  * @Note 
  
  init_queue(&AS4_upload_log_queue,10000,"AS4_upload_log_queue");
  int put_queue_test_Lv(void)
  int get_queue_test_Lv(void)

先初始化，再将需要的数据放入队列，再取出队列
  
 init_queue(&AS4_upload_log_queue,10000,"AS4_upload_log_queue");


typedef struct 
{
	int test1;
	int test2 ;
	int test3;
	
}justtest;
 int put_queue_test_Lv(void)
{
	 justtest *msg = NULL;
	static cnt;
	msg = malloc(sizeof( justtest));
	if(NULL == msg)
	{
		printx(" %s %d malloc failure\n",__FUNCTION__, __LINE__);
		return -1;
	}
		cnt++;
	memset(msg,0,sizeof( justtest));
	msg->test1 = cnt;
	msg->test2 =cnt+ 2;
	msg->test3 = cnt+11;

	put_queue(&AS4_upload_log_queue, (void *)msg);	
	return 0;
}
 int get_queue_test_Lv(void)
{
	 justtest *msg;
	int finaltestcnt1,finaltestcnt2,finaltestcnt3;
	msg = ( justtest *)get_queue(&AS4_upload_log_queue, 10000);
	finaltestcnt1 = msg->test1;
	finaltestcnt2 = msg->test2;
	finaltestcnt3 = msg->test3 ; 
	PX("finaltestcnt1=%d finaltestcnt2=%d finaltestcnt3=%d\n" ,finaltestcnt1,finaltestcnt2,finaltestcnt3);
	free(msg);
}


*/





