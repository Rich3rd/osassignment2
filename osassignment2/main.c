//
//  main.c
//  osassignment2
//
//  Created by Richard Ng on 28/11/2015.
//  Copyright (c) 2015 Richard Ng. All rights reserved.
//

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



void* readerThread(void *arg);
void* writerThread(void *arg);

#define buffsize 1024
char fileBuffer[buffsize];

pthread_t threads[6];
pthread_mutex_t mutex;

FILE* fileReader;
FILE* filePtr2;

char tempString[1024];
char tempString2[1024];

int i;
int numberOfLines;

typedef struct circular_buffer
{
    void *buffer;     // data buffer
    void *buffer_end; // end of data buffer
    size_t capacity;  // maximum number of items in the buffer
    size_t count;     // number of items in the buffer
    size_t sz;        // size of each item in the buffer
    void *head;       // pointer to head
    void *tail;       // pointer to tail
} circular_buffer;

circular_buffer CircleBuffer;

void cb_init(circular_buffer *cb, size_t capacity, size_t sz)
{
    cb->buffer = malloc(capacity * sz);
    if(cb->buffer == NULL)
    {
        printf("cb init error\n");
    }
    
    else{// handle error
    cb->buffer_end = (char *)cb->buffer + capacity * sz;
    cb->capacity = capacity;
    cb->count = 0;
    cb->sz = sz;
    cb->head = cb->buffer;
    cb->tail = cb->buffer;
    }
}

void cb_free(circular_buffer *cb)
{
    free(cb->buffer);
    // clear out other fields too, just to be safe
}

void cb_push_back(circular_buffer *cb, const void *item)
{
    if(cb->count == cb->capacity)
    {
        printf("push back error\n");
    }
    
    // handle error
    else{
    memcpy(cb->head, item, cb->sz);
    cb->head = (char*)cb->head + cb->sz;
    if(cb->head == cb->buffer_end)
        cb->head = cb->buffer;
    cb->count++;
    }
}

void cb_pop_front(circular_buffer *cb, void *item)
{
    if(cb->count == 0)
    {
        printf("pop front error\n");
    }
    // handle error
    else{
    memcpy(item, cb->tail, cb->sz);
    cb->tail = (char*)cb->tail + cb->sz;
    if(cb->tail == cb->buffer_end)
        cb->tail = cb->buffer;
    cb->count--;
    }
}

void *readerThread()
{
    pthread_mutex_lock(&mutex);
    while(fgets(fileBuffer,buffsize,fileReader))
    {
//        printf("%s",fileBuffer);
        cb_push_back(&CircleBuffer,fileBuffer);
        numberOfLines ++;
    }
    pthread_mutex_unlock(&mutex);
    return 0;
}

void *writerThread()
{
    
    pthread_mutex_lock(&mutex);
    for (i=0; i<numberOfLines;i++)
    {
        cb_pop_front(&CircleBuffer,&tempString);
        printf("%s",tempString);
        fprintf(filePtr2,"%s",tempString);
        numberOfLines--;
    
    }
    
    pthread_mutex_unlock(&mutex);
    
    
    return 0;
}


int main()
{
    fileReader = fopen("/Users/Richard/Documents/Xcode/osassignment2/input.txt","r");
    filePtr2 = fopen("/Users/Richard/Documents/Xcode/osassignment2/output.txt","w");
    
    pthread_mutex_init(&mutex, NULL);
    
    memset(tempString, 0, 1024);
    CircleBuffer = *(circular_buffer*) malloc (sizeof(struct circular_buffer));
    cb_init(&CircleBuffer,16,buffsize);
    
    pthread_create(&threads[0],NULL ,&readerThread,NULL);
    pthread_create(&threads[1],NULL ,&readerThread,NULL);
    pthread_create(&threads[2],NULL ,&readerThread,NULL);
    
    pthread_create(&threads[3],NULL ,&writerThread,NULL);
    pthread_create(&threads[4],NULL ,&writerThread,NULL);
    pthread_create(&threads[5],NULL ,&writerThread,NULL);
    
    
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
    pthread_join(threads[2], NULL);
    
    pthread_join(threads[3], NULL);
    pthread_join(threads[4], NULL);
    pthread_join(threads[5], NULL);

    
    

    
    fclose(filePtr2);
    fclose(fileReader);
    
    pthread_mutex_destroy(&mutex);
    pthread_exit(NULL);
    
}


