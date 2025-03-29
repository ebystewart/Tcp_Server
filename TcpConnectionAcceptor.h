#ifndef TCP_CONNECTION_ACCEPTOR_H
#define TCP_CONNECTION_ACCEPTOR_H

#include <pthread.h>
#include <semaphore.h>

class TcpServerController;
class TcpClient;

class TcpConnectionAcceptor
{
    private:

        int accept_fd;
        bool connAcceptorFlag;
        pthread_t *connAcceptorThread;
        sem_t semConnAcceptor;
        pthread_rwlock_t rwlock;

    public:

        TcpServerController *tcp_ctrlr;

        TcpConnectionAcceptor(TcpServerController *tcpServerController);

        ~TcpConnectionAcceptor(void);

        void startTcpConnAcceptorThread(void);

        void stopTcpConnAcceptorThread(void);

        void tcpConnAcceptorThreadHandler(void);

        void setConnAcceptorFlag(bool status);

};
void startServer(void);
#endif