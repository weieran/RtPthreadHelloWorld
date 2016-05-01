/**
 *  Hello World for a RT-Programm under linux
 *  compile using  "gcc -o swave swave.cpp -lrt -Wall"
 *  execute with admin rights: with su ./swave 90 50000 for a RT example.
 *
 *
 *  code copied from:
 *  https://rt.wiki.kernel.org/index.php/Squarewave-example
 *  */

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <array>

const size_t NBR_OF_THREADS = 5;


void* mainThread1(void *threadId)
{
	__useconds_t sleeepTime_us = 1 * 1000000;//1sek

  while(1)
  {
	  printf("Thread PID: #%d!is alive\n", (uint32_t)threadId);
	  usleep(sleeepTime_us);
  }

};

void* mainThread2(void *threadId)
{
	__useconds_t sleeepTime_us = 2 * 1000000;// 2sek

  while(1)
  {
	  printf("Thread PID: #%d! is alive\n", (uint32_t)threadId);
	  usleep(sleeepTime_us);
  }

};




int main(int argc,char** argv)
{
   std::array<pthread_t, NBR_OF_THREADS> threads;


   pthread_create(&threads[0], nullptr, mainThread1, (void*)0);
   pthread_create(&threads[1], nullptr, mainThread1, (void*)1);


   while(1){

	  printf("Main Thread alive \n");
	  sleep(3);//3 Sek

   }
   pthread_exit(nullptr);
   return 0;
}
