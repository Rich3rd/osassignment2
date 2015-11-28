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

void* readerThread(void *arg);
void* writerThread(void *arg);

#define buffsize 1024
char fileBuffer[buffsize];

pthread_t threads[6];
pthread_mutex_t mutex;

FILE* fileReader;
FILE* filePtr2;


void *readerThread()
{
    pthread_mutex_lock(&mutex);
    while(fgets(fileBuffer,buffsize,fileReader))
    {
        printf("%s",fileBuffer);
    }
    pthread_mutex_unlock(&mutex);
    return 0;
}


int main()
{
    fileReader = fopen("/Users/Richard/Documents/Xcode/osassignment2/input.txt","r");
    filePtr2 = fopen("/Users/Richard/Documents/Xcode/osassignment2/output.txt","w");
    
    pthread_mutex_init(&mutex, NULL);
    
    for (int i = 0; i<3; i++)
    {
        
    pthread_create(&threads[i],NULL ,&readerThread,NULL);
    }
    
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
    pthread_join(threads[2], NULL);
    
    fclose(filePtr2);
    fclose(fileReader);
    
    pthread_mutex_destroy(&mutex);
    pthread_exit(NULL);
    
    
    
   
    
}


