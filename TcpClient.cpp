#include "TcpClient.h"
#include <sys/socket.h>


TcpClient::TcpClient(void)
{
    this->client_ip_addr = 0U;
    this->client_port_no = 0U;
    this->ref_count = 0U;
    this->sentBytes = 0U;
    this->receivedBytes = 0U;
    this->recvBufferReadPoint = 0U;
    this->recvBufferWritePoint = 0U;
}

TcpClient::TcpClient(TcpClient *tcpClient)
{
    this->client_ip_addr = tcpClient->client_ip_addr;
    this->client_port_no = tcpClient->client_port_no;
    this->client_fd = tcpClient->client_fd;
    this->ref_count = 0U;
    this->recvBufferReadPoint = 0U;
    this->recvBufferWritePoint = 0U;
}

TcpClient::TcpClient(uint32_t client_ip_addr, uint16_t client_port_no)
{
    this->sentBytes = 0U;
    this->receivedBytes = 0U;
    this->ref_count = 0U;
    this->recvBufferReadPoint = 0U;
    this->recvBufferWritePoint = 0U;

    this->client_ip_addr = client_ip_addr;
    this->client_port_no = client_port_no;
}

TcpClient::~TcpClient(void)
{
    
}

int TcpClient::SendMsg(char *msg, uint32_t msgSize)
{
    if(this->client_fd == 0U)
        return -1;
    int rc = sendto(this->client_fd, msg, msgSize, 0, NULL, 0);
    if(rc < 0){
        std::cout << "sendto client failed" << std::endl;
        return rc;
    }
    else{
        std::cout << "Welcome message sent to client" << std::endl;
    }
    this->sentBytes += (uint32_t)rc;
    return rc;
        
}

void TcpClient::setState(clientStateFlag_t stateFlag)
{
    this->stateFlag = stateFlag;
}