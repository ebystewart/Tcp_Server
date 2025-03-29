#include <iostream>
#include "TcpServerController.h"
#include "TcpConnectionAcceptor.h"
#include "TcpClient.h"
#include "network_utils.h"
#include <signal.h>

TcpServerController *tcpServerController;

void signal_handler(int sig_num);

void signal_handler(int sig_num)
{
   if(sig_num == SIGINT || sig_num == SIGKILL || sig_num == SIGABRT || sig_num == SIGTERM)
   {
      delete tcpServerController;
   }
   exit(0);
}

int main(int argc, char **argv)
{
   tcpServerController = new TcpServerController("127.0.0.1", 4000U, "ServerTry");
   tcpServerController->StartTcpConnectionAcceptorSrv();
   tcpServerController->StartTcpClientServiceManagerSrv();
   if(signal(SIGINT, signal_handler) == SIG_ERR)
       std::cout << "SIGINT signal handler couldn't be configured" << std::endl;
   if(signal(SIGKILL, signal_handler) == SIG_ERR)
       std::cout << "SIGINT signal handler couldn't be configured" << std::endl;
   if(signal(SIGABRT, signal_handler) == SIG_ERR)
       std::cout << "SIGINT signal handler couldn't be configured" << std::endl;
   if(signal(SIGTERM, signal_handler) == SIG_ERR)
       std::cout << "SIGINT signal handler couldn't be configured" << std::endl;            
   while(1);
   return 0;
}