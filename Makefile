CC=g++
CFLAGS=-g -Wall -Wextra -ggdb
TARGET:testapp.exe
LIBS=-lpthread
OBJS=TcpClientDbManager.o 		 		\
			TcpClientServiceManager.o   \
			TcpConnectionAcceptor.o 	\
			TcpServerController.o 	    \
			network_utils.o	            \
			TcpClient.o				

testapp.exe:testapp.o ${OBJS}
	${CC} ${CFLAGS} ${OBJS} testapp.o -o testapp.exe ${LIBS}

testapp.o:testapp.cpp
	${CC} ${CFLAGS} -c testapp.cpp -o testapp.o

TcpClientDBManager.o:TcpClientDBManager.cpp
	${CC} ${CFLAGS} -c TcpClientDBManager.cpp -o TcpClientDBManager.o

TcpClientServiceManager.o:TcpClientServiceManager.cpp
	${CC} ${CFLAGS} -c TcpClientServiceManager.cpp -o TcpClientServiceManager.o

TcpNewConnectionAcceptor.o:TcpNewConnectionAcceptor.cpp
	${CC} ${CFLAGS} -c TcpNewConnectionAcceptor.cpp -o TcpNewConnectionAcceptor.o

TcpServerController.o:TcpServerController.cpp
	${CC} ${CFLAGS} -c TcpServerController.cpp -o TcpServerController.o

network_utils.o:network_utils.cpp
	${CC} ${CFLAGS} -c network_utils.cpp -o network_utils.o

TcpClient.o:TcpClient.cpp
	${CC} ${CFLAGS} -c TcpClient.cpp -o TcpClient.o

clean:
	rm -f *.o
	rm -f *exe
