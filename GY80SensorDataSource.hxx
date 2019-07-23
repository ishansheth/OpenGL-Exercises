#ifndef GY80SENSORDATA_HXX_
#define GY80SENSORDATA_HXX_

#include<unistd.h> 
#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>
//#include<string.h>
#include<string>

#define PORT 8080
#define PI 3.14159265

const char* serverIP = "169.254.133.250";

static void fillAddr(const std::string& address,unsigned short port,sockaddr_in& addr){

  memset(&addr,0,sizeof(addr));
  addr.sin_family = AF_INET;

  hostent* host;

  if((host = gethostbyname(address.c_str())) == NULL){
    std::cerr<<"Failed to resolve hostname"<<std::endl;
  }

  addr.sin_addr.s_addr = *((unsigned long*) host->h_addr_list[0]);

  addr.sin_port = htons(port);
}

class SensorSocket{

protected:
  int m_sockDesc;      //socket descriptor

public:

  SensorSocket(int type,int protocol){
    if((m_sockDesc = socket(PF_INET,type,protocol)) < 0){
      std::cerr<<"Socket can not be created"<<std::endl;
    }
  }
  
  SensorSocket(int newConnSD){
    m_sockDesc = newConnSD;
  }

  ~SensorSocket(){
    ::close(m_sockDesc);
    m_sockDesc = -1;
  }

  void connect(const std::string& foreignAddress,unsigned short foreignPort){
    sockaddr_in destAddr;
    unsigned int addr_len = sizeof(destAddr);

    fillAddr(foreignAddress,foreignPort,destAddr);

    if(::connect(m_sockDesc,(sockaddr*)&destAddr,addr_len)<0){
      std::cerr<<"Could not connect to foreign host"<<std::endl;
    }
  }

  void send(const void* buffer,int bufferlen){
    if(::send(m_sockDesc,(void*)buffer,bufferlen,0)<0){
      std::cerr<<"could not send buffer data to foreign host"<<std::endl;
    }
  }

  int recv(void* buffer,int bufferlen){
    int returnVal;
    if((returnVal = ::recv(m_sockDesc,(void*)buffer,bufferlen,0)) < 0){
      std::cerr<<"could not receive buffer data to foreign host"<<std::endl;      
    }
    return returnVal;
  }

  std::string getLocalAddress(){
    sockaddr_in Addr;

    unsigned int addr_len = sizeof(Addr);

    if(getsockname(m_sockDesc,(sockaddr*)&Addr,(socklen_t*)&addr_len)<0){
      std::cerr<<"Can not get local address"<<std::endl;
    }

    return inet_ntoa(Addr.sin_addr);
  }

  unsigned short getLocalPort(){
    sockaddr_in Addr;

    unsigned int addr_len = sizeof(Addr);

    if(getsockname(m_sockDesc,(sockaddr*)&Addr,(socklen_t*)&addr_len)<0){
      std::cerr<<"Can not get local address"<<std::endl;
    }

    return ntohs(Addr.sin_port);
  }
  

  void setLocalPort(unsigned short port){
    sockaddr_in localaddr;

    memset(&localaddr,0,sizeof(localaddr));

    localaddr.sin_family = AF_INET;
    localaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localaddr.sin_port = port;

    if(bind(m_sockDesc,(sockaddr*)&localaddr,sizeof(sockaddr_in))<0){
      std::cerr<<"can not bind socket to port"<<std::endl;
    }
  }

  void setLocalAddressAndPort(const std::string& localAddr,unsigned short localPort){
    sockaddr_in localaddr;
    fillAddr(localAddr,localPort,localaddr);

    if(bind(m_sockDesc,(sockaddr*)&localaddr,sizeof(sockaddr_in))<0){
      std::cerr<<"can not bind socket to port"<<std::endl;
    }
  }

  std::string getForeignAddress(){
    sockaddr_in Addr;
    
    unsigned int addr_len = sizeof(Addr);
    
    if(getpeername(m_sockDesc,(sockaddr*)&Addr,(socklen_t*)&addr_len)<0){
      std::cerr<<"Can not get foreign address"<<std::endl;
    }
    return inet_ntoa(Addr.sin_addr);
  }

  unsigned short getForeignPort(){
    sockaddr_in Addr;
    
    unsigned int addr_len = sizeof(Addr);
    
    if(getpeername(m_sockDesc,(sockaddr*)&Addr,(socklen_t*)&addr_len)<0){
      std::cerr<<"Can not get foreign address"<<std::endl;
    }
    
    return ntohs(Addr.sin_port);
  }
  
};


/**
class SensorServerSocketTCP:public SensorSocket{

public:
  SensorServerSocketTCP(unsigned short localPort,int queuelen):SensorSocket(SOCK_STREAM,IPPROTO_TCP){
    setLocalPort(localPort);
    setListen(queuelen);
  }
  
  SensorServerSocketTCP(const std::string& localAddress,unsigned short localPort,int queuelen):SensorSocket(SOCK_STREAM,IPPROTO_TCP){
    setLocalAddressAndPort(localAddress,localPort);
    setListen(queuelen);
  }

  ~SensorServerSocketTCP(){

  }

  void setListen(){
    if(listen(m_sockDesc,queuelen)<0){
      std::cerr<<"can not listen for new connection"<<std::endl;      
    }
  }

  SensorSocket* accept(){
    int newSD;
    if((newSD = accept(m_sockDesc,NULL,0))<0){
      std::cerr<<"can not accept new connection"<<std::endl;
    }
    return new SensorSocket(newSD);
  }

};


class SensorServerSocketUDP:public SensorSocket{

public:
  SensorServerSocketUDP(unsigned short localPort,int queuelen):SensorSocket(SOCK_STREAM,IPPROTO_UDP){
    setLocalPort(localPort);
    setListen(queuelen);
  }
  
  SensorServerSocketUDP(const std::string& localAddress,unsigned short localPort,int queuelen):SensorSocket(SOCK_STREAM,IPPROTO_TCP){
    setLocalAddressAndPort(localAddress,localPort);
    setListen(queuelen);
  }

  ~SensorServerSocketUDP(){}

  void setListen(){
    if(listen(m_sockDesc,queuelen)<0){
      std::cerr<<"can not listen for new connection"<<std::endl;      
    }
  }

  SensorSocket* accept(){
    int newSD;
    if((newSD = accept(m_sockDesc,NULL,0))<0){
      std::cerr<<"can not accept new connection"<<std::endl;
    }
    return new SensorSocket(newSD);
  }

};
**/

struct GyroData{
    float xData;
    float yData;
    float zData;
};


class SensorDataSource{
  
  SensorSocket m_sensorConnSocket;
  
  std::thread m_clientThread;
  
  float* m_sensorData;

  bool m_connectionStatus;

  bool m_dataLoaded;
  
  static constexpr int CHUNKSIZE = 30;
  
  std::queue<GyroData> dataBufferQueue;

  std::mutex m_bufferSyncMutex;
  
  std::condition_variable m_bufferSyncVar;

  void restartGyroModule(){
    // TODO: start the whole gyro stack here when server breaks the connection
    int connres= -1;
    int handshakeRes = -1;
    while(connres < 0){
      connres =  connectToServer();
      std::cout<<"Trying to connect................"<<std::endl;
      std::this_thread::sleep_for(std::chrono::seconds(2));
    }    
    handshakeRes = startGyroProtocol();
    if(handshakeRes > 0)
      std::cout<<"Connection established again!!"<<std::endl;
    else
      std::cout<<"Connection cant be established"<<std::endl;
  }


  int readBytesQueue(){
    int readbytes = 0;
    int receivedBytesChunk = 0;
    
    if(!m_connectionStatus){
      std::cout<<"Connection is not yet established"<<std::endl;
      return -1;
    }
    
    const char* sendCmd = "SEND";
    m_sensorConnSocket.send(sendCmd,strlen(sendCmd));      

    memset(m_sensorData,0,CHUNKSIZE*sizeof(float));
    
    while(readbytes != CHUNKSIZE*sizeof(float)){
      /**
	 The 'read' is told to read 'CHUNKSIZE*sizeof(float)' bytes from the socket, but it is possible that it will read
	 less bytes than the reuqested and return the number of bytes read. So there is a need of loop which will keep track
	 of number of total bytes requested and total bytes read from the socket. And when both of them are equal, then 
	 leave the loop with full buffer. If this loop is not used, then it will give garbage value in the provided buffer
      **/
      receivedBytesChunk = m_sensorConnSocket.recv(m_sensorData+readbytes,CHUNKSIZE*sizeof(float)-readbytes);
      if(receivedBytesChunk > 0)
	readbytes += receivedBytesChunk;
      else if(receivedBytesChunk == 0){
	std::cout<<"Server closed the connection"<<std::endl;
	return -1;
      }      
    }
      
    for(int i = 0;i<CHUNKSIZE;i = i+3){
      // creating in place structure
      dataBufferQueue.push({m_sensorData[i],m_sensorData[i+1],m_sensorData[i+2]});      
    }    
    return 1;
  }


  void clientWorkerThread(){
    while(1){
      std::unique_lock<std::mutex> lk(m_bufferSyncMutex);
      m_bufferSyncVar.wait(lk,[this](){return dataBufferQueue.size() == 0;});
      if(readBytesQueue()<0){
	std::cout<<"Bytes can not be read from server,terminating thread"<<std::endl;
	break;
      }
      lk.unlock();
      m_bufferSyncVar.notify_one();
    }
  }

public:
  // ctor
  SensorDataSource():m_sensorData(new float[CHUNKSIZE])
		    ,m_connectionStatus(false),m_sensorConnSocket(SOCK_STREAM,0),m_dataLoaded(false){}

  //dtor, joining thread, deleting the receiver buffer
  ~SensorDataSource(){
    // always check if thread is joinable, then join, otherwise it will crash
    if(m_clientThread.joinable())
      m_clientThread.join();
    delete[] m_sensorData;
  }

  void startClientThread(){
    m_clientThread = std::thread(&SensorDataSource::clientWorkerThread,this);
  }

  GyroData getGyroData(){

    std::unique_lock<std::mutex> lk(m_bufferSyncMutex);

    m_bufferSyncVar.wait(lk,[this]{return dataBufferQueue.size() > 0;});

    GyroData x;
    
    x = dataBufferQueue.front();
    dataBufferQueue.pop();

    lk.unlock();
    m_bufferSyncVar.notify_all();
    return x;
  }
  
  
  int connectToServer(){
    m_sensorConnSocket.connect(serverIP,PORT);
    std::cout<<"Local Address:----->"<<m_sensorConnSocket.getLocalAddress()<<std::endl;

    std::cout<<"connect to server successful"<<std::endl;
    m_connectionStatus = true;
    return 1;
  }

  int startGyroProtocol(){
    std::string servResponse;
    char buffer[1024] = {0};
    if(!m_connectionStatus){
      std::cout<<"Connection is not yet established"<<std::endl;
      return -1;
    }
    
    int valrec = m_sensorConnSocket.recv(buffer,1024);
    if(valrec > 0){
      servResponse = buffer;
      std::cout<<"First Server Response:"<<servResponse<<std::endl;
      if(!servResponse.compare("GYRODATA")){
	const char* ok = "OK";
	m_sensorConnSocket.send(ok,strlen(ok));      
      }
      return 1;
    }else if(valrec == 0){
      std::cout<<"Connection is closed by server,client can wait and try again"<<std::endl;
      return -1;
    }else{
      std::cout<<"Error Occured, Data can not be received"<<std::endl;
      return -1;
    }
    return 1;
  }


};

#endif
