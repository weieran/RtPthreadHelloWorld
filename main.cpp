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

namespace
{

	const size_t MAX_NBR_OF_THREADS = 5;

	struct ThreadData
	{
		size_t threadId;
		size_t prio;
		__useconds_t sleepTime_us;
		std::string message;
	};

	pthread_mutex_t countMutex;
	pthread_cond_t countThreshold_cv;
	const auto COUNT_LIMIT = 10;
	size_t count;

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

void* mainThread2(void *threadArg)
{
	auto data = static_cast<ThreadData*>(threadArg);

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

   const ThreadData data = { 1, 50, 1000000, "first thread"};
   std::array<ThreadData, MAX_NBR_OF_THREADS> threadData = {{
		   //Id,Prio, Sleep_us, MessageString
                   { 1, 50, 1000000, "first thread"},
		   { 2, 50, 2000000,"second thread"},
		   { 3, 50, 3000000,"third thread"},
		   { 4, 50, 4000000,"fourth thread"},
		   { 5, 50, 5000000,"fifth thread"}
   }};


   pthread_mutex_init(&countMutex, nullptr);
   pthread_cond_init(&countThreshold_cv, nullptr);

   pthread_create(&threads[0], nullptr, mainThread1, &threadData[0]);
   pthread_create(&threads[1], nullptr, mainThread2, &threadData[1]);


   while(1){

	  printf("Thread PID: #0 alive \n");
	  countAndSignal();
	  sleep(3);//3 Sek

   }
   pthread_exit(nullptr);
   return 0;
}
