#ifndef TCP_SERVER_CONTROLLER_H
#define TCP_SERVER_CONTROLLER_H

#include <stdint.h>
#include <string>
#include <list>
#include <pthread.h>
#include <semaphore.h>

class TcpConnectionAcceptor;
class TcpClientServiceManager;
class TcpClientDbManager;
class TcpClient;

typedef struct TcpServerMsg_{
    void *data;
    sem_t *zero_sem;

}TcpServerMsg_t;

class TcpServerController
{
    private:
      
        pthread_rwlock_t rwlock;
        pthread_mutex_t mesgQMutex;
        pthread_cond_t mesgQCondition;
        pthread_t *serverControllerMsgQThread;
        TcpConnectionAcceptor   *tcpConnectionAcceptor;
        TcpClientServiceManager *tcpClientServiceManager;
        TcpClientDbManager      *tcpClientDbManager;

        std::list<TcpClient *> establishedClient;
        std::list<TcpClient *> connectPendingClients;

        std::list<TcpServerMsg_t *> msgQ;
        
        void ProcessMsgQMsg(TcpServerMsg_t *msg);

    public:
        
        uint32_t ip_addr;
        uint16_t port_no;
        std::string server_name;

        TcpServerController(std::string ip_addr, uint16_t port_no, std::string server_name);

        ~TcpServerController(void);

        void StartTcpServer(void);

        void StopTcpServer(void);

        void MsgQProcessingThreadInternal(void);

        void StartTcpConnectionAcceptorSrv(void);

        void StartTcpClientServiceManagerSrv(void);

        void StartTcpClientDbManagerSrv(void);

        void addClientToDB(TcpClient *tcpClient);

        void copyClientToList(std::list<TcpClient *> *listDb);

        void closeAllConnections(void);

        void displayList();
};
#endif //TCP_SERVER_CONTROLLER