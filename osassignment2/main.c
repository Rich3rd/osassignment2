/*
 credits to Adam Rosenfield for circular buffer at 
 http://stackoverflow.com/questions/827691/how-do-you-implement-a-circular-buffer-in-c
 
Ng Ter Jing 
 0313766 
 OS assignment 2
 
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void* readerThread();
void* writerThread(); /*methods*/

#define buffsize 1024
char fileBuffer[buffsize]; /*file buffer*/

pthread_t threads[6];
pthread_mutex_t thread_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t thread_mutex1 = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t condition_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition_cond  = PTHREAD_COND_INITIALIZER;

FILE* fileReader;
FILE* filePtr2;

char tempString[1024];
char tempString2[1024];

int i;

typedef struct circular_buffer /*struct for ciruclar buffer */
{
    void *buffer; /*start of buffer*/
    void *buffer_end; /*end of whole buffer*/
    size_t capacity; /*maxmum capacity of buffer*/
    size_t count; /*how many items are inside the buffer*/
    size_t sz; /*size of each individual item in the buffer*/
    void *head; /*pointer to first item in buffer*/
    void *tail; /*pointer to the last item in the buffer*/
} circular_buffer;

circular_buffer CircleBuffer; /*declare circular buffer*/

void cb_init(circular_buffer *cb, size_t capacity, size_t sz) /*initilise circular buffer*/
{
    cb->buffer = malloc(capacity * sz); /*malloc the whole buffer */
    if(cb->buffer == NULL) /*if buffer malloc failed*/
    {
        printf("cb init error\n"); /*print error*/
    }
    cb->buffer_end = (char *)cb->buffer + capacity * sz; /*save the pointer to the end of the buffer*/
    cb->capacity = capacity; /*size of max capacity of buffer*/
    cb->count = 0;  /* the number of items current in the buffer*/
    cb->sz = sz;    /*the size of each item in the buffer */
    cb->head = cb->buffer; /* pointer to the head of the buffer*/
    cb->tail = cb->buffer; /*pointer to the tail of the buffer*/
}

void cb_free(circular_buffer *cb) /*free circular buffer*/
{
    free(cb->buffer);
}

void cb_push_back(circular_buffer *cb, const void *item) /*push items into circular buffer*/
{
    if(cb->count == cb->capacity)   /*if buffer is full*/
    {
        printf("push back error\n"); /*print error */
    }
    
    memcpy(cb->head, item, cb->sz); /*copy the pointer to 'item' to cb->head*/
    cb->head = (char*)cb->head + cb->sz; /*move the current head to the new head*/
    if(cb->head == cb->buffer_end) /*if the head is at the end of the whole buffer*/
        cb->head = cb->buffer; /*move the head to the buffer*/
    cb->count++; /*increase the count of numbers in the buffer*/
    
}

void cb_pop_front(circular_buffer *cb, void *item) /*pop items from circular buffer*/
{
    if(cb->count == 0) /*if buffer is empty*/
    {
        printf("pop front error\n"); /*print error*/
    }
    
    memcpy(item, cb->tail, cb->sz); /*copy the pointer of cb->tail to 'item'*/
    cb->tail = (char*)cb->tail + cb->sz; /*move cb->tail to new tail*/
    if(cb->tail == cb->buffer_end)/*if cb->tail is at the end of buffer*/
        cb->tail = cb->buffer; /*move cb->tail back to buffer*/
    cb->count--; /*minus the number of items inside the buffer*/
}

void *readerThread() /*reader thread to read from file*/
{
   pthread_mutex_lock(&thread_mutex); /*lock mutex for access to thread */
    while(fgets(fileBuffer,buffsize,fileReader)) /*gets items from file line by line until end */
    {
        pthread_mutex_lock(&condition_mutex); /*lock condition mutex */
        {
            while(CircleBuffer.count == CircleBuffer.capacity)  /*if circular buffer is full */
            {
                pthread_cond_wait(&condition_cond,&condition_mutex); /*wait for writer thread to write first */
            }
        }
        pthread_mutex_unlock(&condition_mutex); /*unlock condition mutex*/
        printf("ReaderThread :%s",fileBuffer);
        cb_push_back(&CircleBuffer,fileBuffer); /*push item read from file into circular buffer*/
    }
    pthread_mutex_unlock(&thread_mutex); /*unlock thread mutex */
    
    return 0;
}

void *writerThread() /*writer thread to write to file from shared buffer*/
{
    while(CircleBuffer.count != 0) /*if buffer is not empty*/
    {
        pthread_mutex_lock(&thread_mutex1); /*lock another thread mutex*/
        
        pthread_mutex_lock(&condition_mutex); /*lock the condition mutex*/
        if(CircleBuffer.count != CircleBuffer.capacity) /*if buffer is not full*/
        {
            pthread_cond_broadcast(&condition_cond); /*signal to readerThread to keep reading*/
        }
        pthread_mutex_unlock(&condition_mutex); /*unlock condition mutex*/
        
        cb_pop_front(&CircleBuffer,&tempString); /*take item out of circular buffer*/
        printf("WriterThread print out : %s",tempString);
        fprintf(filePtr2,"%s",tempString); /*print item into output file*/
        
        pthread_mutex_unlock(&thread_mutex1); /*unlock the thread mutex 1*/
    }
    return 0;
}


int main(int argc, char *argv[])
{
    /*
    fileReader = fopen("/home/0313766/OperatingSystems/Assignment2/input.txt","r");
    filePtr2 = fopen("output.txt","w");
    */
    
    fileReader = fopen(argv[1],"r"); /*file pointers*/
    filePtr2 = fopen(argv[2],"w");
    pthread_mutex_init(&thread_mutex, NULL); /*declare mutex*/
    
    memset(tempString, 0, 1024); /*set tempString to nothing*/
    CircleBuffer = *(circular_buffer*) malloc (sizeof(struct circular_buffer)); /*mallock circular buffer*/
    cb_init(&CircleBuffer,16,buffsize); /*initialise buffer*/

    pthread_create(&threads[0],NULL ,&readerThread,NULL); /*create read and write threads*/
    pthread_create(&threads[1],NULL ,&readerThread,NULL);
    pthread_create(&threads[4],NULL ,&writerThread,NULL);
    pthread_create(&threads[3],NULL ,&writerThread,NULL);
    pthread_create(&threads[2],NULL ,&readerThread,NULL);
    pthread_create(&threads[5],NULL ,&writerThread,NULL);
    
    pthread_join(threads[0], NULL); /*join read threads*/
    pthread_join(threads[1], NULL);
    pthread_join(threads[2], NULL);
    
    pthread_join(threads[3], NULL); /*join write threads*/
    pthread_join(threads[4], NULL);
    pthread_join(threads[5], NULL);

    cb_free(&CircleBuffer); /*free cirucular buffer*/
    pthread_mutex_destroy(&thread_mutex);
    pthread_mutex_destroy(&thread_mutex1); /*destroy mutexs*/
    
    fclose(filePtr2);
    fclose(fileReader); /*close file pointers*/
    
    pthread_exit(NULL);
    
}


