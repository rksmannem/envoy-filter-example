#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <iostream>


#define SHM_KEY 0x1234

const int SHM_ACCESS = 0666;
const int SHM_CREATE = SHM_ACCESS|IPC_CREAT;
const int SHM_ATTACH = SHM_ACCESS;
const int MAX_LENGTH = 1024;
const char QUIT[] = "quit";

using namespace std;

//simple data structure shared by both processes
struct shared_data {
   char str[MAX_LENGTH];
   int signal;
};

//variables common to writer and reader process
int shmid;
struct shared_data *shared_data;
struct shmid_ds shm_stats;


//Create and/or attach to shared memory
int shm_allocate(int shm_mode){
   shmid = shmget(SHM_KEY, sizeof(struct shared_data), shm_mode);
   if (shmid == -1){
      cout << "shmget error:" << errno << endl << endl << ", error: " << strerror(errno) << endl;
      return 0;
   }

   void *ret = shmat(shmid, NULL, 0);
   shared_data = reinterpret_cast<struct shared_data*>(ret);
   if ( shared_data == NULL ) {
        cout << "shmat error:" << errno << endl << endl<< ", error: " << strerror(errno) << endl;
        return 0;
   }

   return 1; //success
}

//Detach from and deallocate shared memory
int shm_release(){
   int dtRet = shmdt(shared_data);
   if ( dtRet == -1) {
      cout << endl << "shmdt Return Value: " << dtRet << ", error: "<< strerror(errno) << endl;
      return 0;
   }

   int ctlRet = shmctl(shmid, IPC_RMID, 0);
   if (ctlRet == -1) {
      cout << endl << "shmctl Return Value: " << ctlRet  << ", error: "<< strerror(errno) << endl;
      return 0;
   }
   
   return 1; //success
}


int shm_detach(){
   int dtRet = shmdt(shared_data);
   if ( dtRet == -1) {
      cout << endl << "shmdt Return Value: " << dtRet << ", error: "<< strerror(errno) << endl;
      return 0;
   }
   cout << endl << "detached shared memory: " << dtRet << endl;
   return 1; //success
}

void print_shm_info(){
   if(shmctl(shmid, IPC_STAT, &shm_stats) == 0) {
      cout << endl << "Size of segment in bytes: " << shm_stats.shm_segsz << endl;
      cout << "Processes attached: " << shm_stats.shm_nattch << endl << endl; 
   }
   else
      cout << endl << "Error fetching shared memory info" << endl << endl << ", error: "<< strerror(errno) << endl;
}
