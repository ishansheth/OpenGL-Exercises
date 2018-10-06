#ifndef GY80SENSORDATA_HXX_
#define GY80SENSORDATA_HXX_

#include<unistd.h> 
#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>


#define PORT 8080
#define PI 3.14159265

const char* serverIP = "169.254.133.250";


class SensorDataSource{

  std::thread m_clientThread;
  
  int m_clientSock = 0,valread;

  struct sockaddr_in m_servAddr;

  float* m_sensorData;

  bool m_connectionStatus;

  static constexpr int CHUNKSIZE = 3;

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


  void clientWorkerThread(){
    while(1){
      std::cout<<"Thread running....."<<std::endl;
      readBytes();
    }
  }

public:
  // ctor
  SensorDataSource():m_sensorData(new float[CHUNKSIZE]),m_connectionStatus(false){}

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

    // The size of the buffer 'CHUNKSIZE' should be able to be set and when its filled, this function will return the buffer
  int readBytes(){
    
    int readbytes = 0;
    int receivedBytesChunk = 0;
    if(!m_connectionStatus){
      std::cout<<"Connection is not yet established"<<std::endl;
      return -1;
    }
    
    while(readbytes != CHUNKSIZE*sizeof(float)){
      /**
	 The 'read' is told to read 'CHUNKSIZE*sizeof(float)' bytes from the socket, but it is possible that it will read
	 less bytes than the reuqested and return the number of bytes read. So there is a need of loop which will keep track
	 of number of total bytes requested and total bytes read from the socket. And when both of them are equal, then 
	 leave the loop with full buffer. If this loop is not used, then it will give garbage value in the provided buffer
      **/
      receivedBytesChunk = read(m_clientSock,m_sensorData+readbytes,CHUNKSIZE*sizeof(float)-readbytes);
      std::cout<<"Received-----"<<receivedBytesChunk<<std::endl;
      if(receivedBytesChunk > 0)
	readbytes += receivedBytesChunk;
      else if(receivedBytesChunk == 0){
	std::cout<<"Server closed the connection"<<std::endl;
	return -1;
      }      
    }

    for(int i = 0;i<readbytes/(sizeof(float));i=i+3){
      //      float pitch = (atan2(m_sensorData[i],sqrt(m_sensorData[i+1]*m_sensorData[i+1] + m_sensorData[i+2]*m_sensorData[i+2])) * 180.0f) / PI;
      //      float roll = (atan2(m_sensorData[i+1],sqrt(m_sensorData[i]*m_sensorData[i] + m_sensorData[i+2]*m_sensorData[i+2])) * 180.0f) / PI;
      //      std::cout<<"Pitch:"<<pitch<<"  Roll:"<<roll<<std::endl;
      //      std::cout<<":"<<m_sensorData[i]<<":"<<m_sensorData[i+1]<<":"<<m_sensorData[i+2]<<std::endl;
    }
    return 1;
  }
  
  int connectToServer(){
    if((m_clientSock = socket(AF_INET,SOCK_STREAM,0))<0){
      printf("socket can not be created");
      return -1;
    }

    m_servAddr.sin_addr.s_addr = inet_addr(serverIP);
    m_servAddr.sin_family = AF_INET;
    m_servAddr.sin_port = htons(PORT);
    
    if(connect(m_clientSock,(struct sockaddr*)&m_servAddr,sizeof(m_servAddr))<0){
      std::cout<<"connect to server failed"<<std::endl;
      return -1;
    }
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
    
    int valrec = read(m_clientSock,buffer,1024);
    if(valrec > 0){
      servResponse = buffer;
      std::cout<<"First Server Response:"<<servResponse<<std::endl;
      if(!servResponse.compare("GYRODATA")){
	const char* ok = "OK";
	send(m_clientSock,ok,strlen(ok),0);      
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


  float* getSensorData(){
    return m_sensorData;
  }
};

#endif
