#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#define MAX 5
int buf[MAX];
int in=0;
int out =0;
int count=0;
pthread_mutex_t mutex;
pthread_cond_t nof;
pthread_cond_t noe;

//生产
void* prod(void*arg)
{
    int id=*(int*)arg;
    for(int i=0;i<10;i++)
    {
        int data=i+100*id;//生产数据
        pthread_mutex_lock(&mutex);
         while(count==MAX)
         {
            printf("生产者%d等待，缓冲区满\n",id);
            pthread_cond_wait(&nof,&mutex);//

         }
         buf[in]=data;
            in=(in+1)%MAX;
            count++;
            printf("生产者%d生产了%d，缓冲区数量%d\n",id,data,count);
            pthread_cond_signal(&noe);//唤醒消费者
         pthread_mutex_unlock(&mutex);//
            sleep(1);//为什么sleep不在解锁前面？
    } 
}
//消费
void *cons(void*arg)
{
   int id=*(int*)arg;
   for(int i=0;i<10;i++)
   {
         pthread_mutex_lock(&mutex);
         while(count==0)
         {
            printf("消费者%d等待，缓冲区空\n",id);
            pthread_cond_wait(&noe,&mutex);

         }
         int data=buf[out];
         out=(out+1)%MAX;//(max==buf.size)
         count--;

         pthread_cond_signal(&nof);
         printf("消费者%d消费了%d，缓冲区数量%d\n",id,data,count);
         pthread_mutex_unlock(&mutex);
         sleep(1);

   }
   return NULL;
}
int main()
{
   pthread_t p1,p2,c1,c2;
   int id1=1,id2=2;

   pthread_mutex_init(&mutex,NULL);
   pthread_cond_init(&nof,NULL);
   pthread_cond_init(&noe,NULL);

   pthread_create(&p1,NULL,prod,&id1);
   pthread_create(&p2,NULL,prod,&id2);
   pthread_create(&c1,NULL,cons,&id1);
   pthread_create(&c2,NULL,cons,&id2);

   pthread_join(p1,NULL);
   pthread_join(p2,NULL);
   pthread_join(c1,NULL);
   pthread_join(c2,NULL);

   pthread_mutex_destroy(&mutex);
   pthread_cond_destroy(&noe);
   pthread_cond_destroy(&nof);

   return 0;
}
