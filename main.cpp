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

#include "can_main.h"

namespace
{

	const size_t MAX_NBR_OF_THREADS = 4;

	struct ThreadData
	{
		size_t threadId;
		sched_param schedPara;
		__useconds_t sleepTime_us;
		std::string message;
	};




	pthread_mutex_t countMutex;
	pthread_cond_t countThreshold_cv;
	const auto COUNT_LIMIT = 10;
	size_t count;
	const auto MY_PROCESS_PID = 0;

	void countAndSignal ()
	{
	  pthread_mutex_lock (&countMutex);
	  count++;
	  printf("Count set to %d \n",count);
	  if (count >= COUNT_LIMIT)
	  {
	    pthread_cond_signal (&countThreshold_cv);
	  }
	  pthread_mutex_unlock (&countMutex);
	}
}



void* mainThread1(void *threadArg)
{
    auto data = static_cast<ThreadData*>(threadArg);

    sched_setscheduler(MY_PROCESS_PID, SCHED_FIFO, &data[0].schedPara);

    while(1)
    {
      pthread_mutex_lock (&countMutex);
      pthread_cond_wait(&countThreshold_cv, &countMutex);
      count = 0;
      pthread_mutex_unlock (&countMutex);
      printf("Thread PID: #%d!is alive\n", data->threadId);
      printf("Count is reset now\n");

    }

};

void* canThread(void *threadArg)
{
    auto data = static_cast<ThreadData*>(threadArg);

    sched_setscheduler(MY_PROCESS_PID, SCHED_FIFO, &data[0].schedPara);

    init_can();
    while(1)
    {
      printf("Thread PID: #%d!is alive\n", data->threadId);
      can_main();
      usleep(data->sleepTime_us);
    }
};



void* mainThread2(void *threadArg)
{
    auto data = static_cast<ThreadData*>(threadArg);

    sched_setscheduler(MY_PROCESS_PID, SCHED_FIFO, &data[0].schedPara);

    while(1)
    {
      printf("Thread PID: #%d!is alive\n", data->threadId);
      countAndSignal();
      usleep(data->sleepTime_us);
    }

};

void* mainThread0(void *threadArg)
{
    auto data = static_cast<ThreadData*>(threadArg);

    sched_setscheduler(MY_PROCESS_PID, SCHED_FIFO, &data[0].schedPara);

    while(1)
    {
      printf("Thread PID: #%d!is alive\n", data->threadId);
      countAndSignal();
      usleep(data->sleepTime_us);
    }

};



int main(int argc,char** argv)
{
   std::array<pthread_t, MAX_NBR_OF_THREADS> threads;

   std::array<ThreadData, MAX_NBR_OF_THREADS> threadData = {{
		   //Id,Prio, Sleep_us, MessageString
                   { 0, {49}, 10000000	, "first thread"},
		   { 1, {59}, 20000000	,"second thread"},
		   { 2, {69}, 30000000	,"third thread"	},
		   { 3, {39},  1000000	,"can thread"	},
   }};

   pthread_mutex_init(&countMutex, nullptr);
   pthread_cond_init(&countThreshold_cv, nullptr);

   // set thread prio and sheduling type
   pthread_create(&threads[1], nullptr, mainThread1, &threadData[1]);
   pthread_create(&threads[2], nullptr, mainThread2, &threadData[2]);
   pthread_create(&threads[3], nullptr, canThread  , &threadData[3]);

   sched_setscheduler(MY_PROCESS_PID, SCHED_FIFO, &threadData[0].schedPara);
   auto main_thread_data = threadData[0];
   while(1){

	  printf("Thread PID: #%d!is alive\n", main_thread_data.threadId);
	  countAndSignal();
	  usleep(main_thread_data.sleepTime_us);//3 Sek
   }
   pthread_exit(nullptr);
   return 0;
}
