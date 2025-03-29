#ifndef TCP_CLIENT_SERVICE_MANAGER_H
#define TCP_CLIENT_SERVICE_MANAGER_H

#include <pthread.h>
#include <list>
#include <semaphore.h>

class TcpServerController;
class TcpClient;

class TcpClientServiceManager
{
    private:

        std::list<TcpClient *>tcpClientDb;
        pthread_rwlock_t rwlock;
        pthread_t *serviceMgrThread;
        sem_t semServiceMgr;
        int client_fd;
        int max_fd;
        fd_set active_fd_set;
        fd_set backup_fd_set;

    public:

        TcpServerController *tcp_ctrlr;

        TcpClientServiceManager(TcpServerController *tcpServerController);

        ~TcpClientServiceManager(void);  
        
        void startTcpClientServiceManagerThread(void);

        void startTcpClientServiceManagerThreadHandler(void);

        void copyClientFdToFdSet(fd_set *fd_set);

        void closeAllConnections(void);

        int findMaxFd(void);

};

#endif