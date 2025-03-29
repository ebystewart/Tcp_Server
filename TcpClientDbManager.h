#ifndef TCP_CLIENT_DB_MANAGER_H
#define TCP_CLIENT_DB_MANAGER_H
#include <iostream>
#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>
#include <list>

class TcpServerController;
class TcpConnectionAcceptor;
class TcpClient;

class TcpClientDbManager
{
    private:

        std::list<TcpClient *>tcpClientDb;
        pthread_rwlock_t rwlock;

    public:
     
        TcpServerController *tcpServerController;

        TcpClientDbManager(TcpServerController *tcpServerController);

        ~TcpClientDbManager(void);

        TcpClient *LookUpClientDb(uint32_t ipAddr, uint16_t portNum);

        void AddClientToDb(TcpClient *);

        void RemoveClientFromDb(TcpClient *);

        void UpdateClient(TcpClient *);

        void copyClientToList(std::list<TcpClient *> *listDb);

        void clearList(void);

        void displayList(void);

        void closeAllConnections(void);

        void incrementClientRefCount(TcpClient *);

        void decrementClientRefCount(TcpClient *);
        
};

#endif