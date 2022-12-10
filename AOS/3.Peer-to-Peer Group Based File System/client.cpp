#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <cmath>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

#include <iomanip>
#include <openssl/sha.h>
#define SHA_DIGEST_LENGTH 20
using namespace std;

struct sockaddr_in c_addr;
// static char fname[100]; // used to store the filename

string uploadSHAval = "";
string downloadSHAval = "";
unordered_map<string, unordered_set<int>>
    chunk_map; // <fileID, set(chunkID)> // used to store the chunks belonging
               // to a particular file at the peer end

map<pair<string, int>,
    unordered_set<int>>
    ip_port_chunk_map; // <<IP:PORT>, set{chunkIDs}>

map<pair<string, int>,
    unordered_set<int>>
    ip_port_chunk_map_final; // <<IP:PORT>, set{chunkIDs}>

unordered_map<string, unordered_map<string, string>>
    down_stat_map; // <userID, <fileID,<[D/C] [groupID] [fileID]>>>

long long
GetFileSize(std::string filepath) { // returns size of the file in bytes
  ifstream fd(filepath, ios::ate | ios::binary);
  long long file_size = fd.tellg();
  fd.close();
  return file_size;
}

string returnFileName(string filePath) {

  /* // split the input command
  vector<string> commandVec; // used to store the words of the input command
  stringstream ss(s);        // to parse the input command
  string word;               // to store the individual words
  while (ss >> word) {
    commandVec.push_back(word); // push the parts of the command into the vector
  } */

  string s = filePath;
  string fileName = s.substr((s.find_last_of('/')) + 1);

  return fileName;
}

vector<string> shas; // vector which will contain SHA's of individual chunks
string shaFunc(string filePath) {
  unsigned char c[SHA_DIGEST_LENGTH];
  //  string filename = argv[1];
  string filepath = "Reference Peer-to-peer networking with BitTorrent.pdf";
  filepath = filePath;
  int i;
  FILE *inFile = fopen(filepath.c_str(), "rb");
  SHA_CTX mdContext;
  SHA_CTX mdContext1;
  int bytes;
  unsigned char data[524288];

  if (inFile == NULL) {
    cout << "Cannot open file" << endl;
    return 0;
  }

  SHA1_Init(&mdContext);
  while ((bytes = fread(data, 1, 524288, inFile)) != 0) {
    SHA1_Update(&mdContext, data, bytes);

    unsigned char c1[SHA_DIGEST_LENGTH];
    SHA1_Init(&mdContext1);
    SHA1_Update(&mdContext1, c1, bytes);
    SHA1_Final(c1, &mdContext1);
    stringstream ss;
    string s = "";
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
      s += c1[i];
    }
    shas.push_back(s);
  }

  // will contain SHA of complete file
  string fileSHA = "";
  SHA1_Final(c, &mdContext);
  /* for (i = 0; i < SHA_DIGEST_LENGTH; i++) {
    // printf("%d |", c[i]);
    //  cout << c[i]+" |";
    fileSHA += c[i] + " |";
  }
  cout << endl; */

  /* for (auto it : shas) {
    cout << it << endl;
  } */

  //   printf(" %s\n", filename);
  //   cout << filename << endl;

  stringstream ss3;
  for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
    ss3 << hex << setw(2) << setfill('0') << (int)c[i];
  }
  fileSHA = ss3.str();
  fclose(inFile);
  return fileSHA;
}

void SendFileToClient(int arg, string filepath) {
  int connfd = arg;
  printf("Connection accepted and id: %d\n", connfd);
  printf("Connected to Client: %s:%d\n", inet_ntoa(c_addr.sin_addr),
         ntohs(c_addr.sin_port));
  string demo;
  // string demo = "file1.png";

  // READ filepath
  demo = filepath;
  // string demo = "client.cpp";
  //  write(connfd, demo.c_str(),100);
  // cout << fname << "\n";
  // FILE *fp = fopen(fname,"rb");;
  string filename = returnFileName(filepath);
  cout << "SendFileToClient: " + filename + "\n";
  cout << "Uploaded filePath: " + demo + "\n";
  int fp = open(demo.c_str(), 00);
  if (fp < 0) {
    printf("File open error");
    //  return NULL;
  }

  /* Read data from file and send it */
  // size_t sum=0;
  while (1) {
    /* First read file in chunks of 256 bytes */
    unsigned char buff[1024] = {0};
    // int nread = fread(buff,1,1024,fp);
    int nread = read(fp, buff, 1024);
    // printf("Bytes read %d \n", nread);

    /* If read was success, send data. */
    if (nread > 0) {
      // printf("Sending \n");
      write(connfd, buff, nread);
      // sum = sum + nread;
    }
    if (nread <= 0) {
      if (nread == 0) {
        printf("End of file\n");
        printf("File transfer completed for id: %d\n", connfd);
      }
      if (nread < 0)
        printf("Error reading\n");
      break;
    }
  }
  // cout << "Total bytes sent from the server = " << sum <<" bytes" << endl;
  /*  printf("Closing Connection for id: %d\n", connfd);
   close(connfd); */
  // shutdown(connfd, SHUT_WR);
  //  sleep(2);
}
int portPeerServer;
int server(string ip, int port) {

  int connfd = 0, err;          // store thread id
  struct sockaddr_in serv_addr; // store socket address
  int listenfd = 0, ret;
  char sendBuff[1025]; // buffer size
  int numrv;
  size_t clen = 0;

  // AF_INET is an address family that is used to designate the type of
  // addresses that
  //  your socket can communicate with (in this case, Internet Protocol v4
  //  addresses).
  // SOCK_STREAM is used as we are using TCP protocol
  listenfd = socket(
      AF_INET, SOCK_STREAM,
      0); // create a new socket & returns file descriptor for the new socket
  if (listenfd < 0) {
    printf("Error in socket creation\n");
    exit(2);
  }

  printf("Socket retrieve success\n");
  // now store the socket address
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr =
      htonl(INADDR_ANY); // htonl reverses the byte order
  // serv_addr.sin_addr.s_addr = inet_addr(ip.c_str());
  // portPeerServer = 5000;
  portPeerServer = port;
  // cout << "Enter the port no for the server of the peer: ";
  // getline(cin, portPeerServer);
  // cin.get();
  // cin >> portPeerServer;

  // serv_addr.sin_port = htons(
  //     portPeerServer); // htons() used to store the number in network byte
  //     order
  //                      // i.e. BIG-ENDIAN format(largest byte goes first)
  // binds the socket with the ip address
  // ret = bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  // if (ret < 0) {
  //   printf("Error in bind\n");
  //   exit(2); // terminates the calling process
  // }

  while (true) {
    serv_addr.sin_port =
        htons(portPeerServer); // htons() used to store the number in network
                               // byte order i.e. BIG-ENDIAN format(largest byte
                               // goes first)ret = bind(listenfd, (struct
                               // sockaddr *)&serv_addr, sizeof(serv_addr));
                               // binds the socket with the ip address
    ret = bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (ret >= 0) {
      break;
    }
    portPeerServer++;
  }

  // keep on listening connections on socket listenfd
  if (listen(listenfd, 10) == -1) {
    printf("Failed to listen\n");
    return -1;
  }

  while (1) { // wait for client to connect; it will always keep on listening

    clen = sizeof(c_addr);
    cout << ("Waiting...\n");
    // cout << "listenfd :" << listenfd << endl;
    connfd = accept(listenfd, (struct sockaddr *)&c_addr, (socklen_t *)&clen);
    // cout << "PRINT1\n";
    if (connfd < 0) {
      printf("Error in accept\n");
      continue;
    }

    // read filepath from peer
    char Buff1[1024];
    int totalBytes = read(connfd, Buff1, sizeof(Buff1));
    // int totalBytes = read(listenfd, Buff1, sizeof(Buff1));
    string s(Buff1);
    /* for (int i = 0; i < totalBytes; i++) {
      s = s + Buff1[i];
    } */
    cout << "filepath received at peer's server is:" << s << endl;

    thread tClient(SendFileToClient, connfd, s);
    tClient.detach();
    // tClient.join();
    /*  if (err != 0)
         printf("\ncan't create thread :[%s]", strerror(err)); */
  }
}

int downloadFromPeer(string ip, string port, string file_path, string file_name,
                     string destination_path, string group_name,
                     string user_name) {

  cout << "Inside download from peer func(): \n";

  // cout << "Printing in peer's CLIENT:\n";
  cout << "ip: " << ip << endl;
  cout << "port: " << port << endl;
  cout << "filePath: " << file_path << endl;
  cout << "file_name: " << file_name << endl;
  cout << "destination_path: " << destination_path << endl;
  cout << "gname: " << group_name << endl;
  cout << "uname: " << user_name << endl;

  int sockfd = 0;
  int bytesReceived = 0;
  char recvBuff[1024];
  memset(recvBuff, '0', sizeof(recvBuff));
  struct sockaddr_in serv_addr;

  /* Create a socket first */
  // we will be using this socket to connect to the server of the peer
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("\n Error : Could not create socket \n");
    return 1;
  }
  int on = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
  /* Initialize sockaddr_in data structure */
  serv_addr.sin_family = AF_INET;
  int peerPortServer = stoi(port);
  serv_addr.sin_port = htons(peerPortServer); // port of another peer's server

  serv_addr.sin_addr.s_addr = inet_addr(ip.c_str());

  /* Attempt a connection */
  if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    printf("\n Error : Connect Failed \n");
    return 1;
  }

  printf("Connected to Peer ip: %s : %d\n", inet_ntoa(serv_addr.sin_addr),
         ntohs(serv_addr.sin_port));

  // send filePath to server
  write(sockfd, file_path.c_str(),
        file_path.size()); // send message to socket
  cout << "File path sent to the peer:" + file_path + "\n";

  // now we are connected to the peer which conatins the requested file
  // now start downloading it and store it in a file

  /* Create file where data will be stored */
  // FILE *fp;
  // char fname[100];
  // read(sockfd, fname, 256);
  // cout << fname << "\n";
  // strcat(fname,"AK");
  // printf("File Name: %s\n", fname);
  printf("Receiving file...");
  cout << "Receiving file..." << endl;
  /* string gg = "";
  // gg = "lol";
  gg = group_name + file_name;
  cout << "gg value in DOWNLOAD: " + gg + "\n"; */
  cout << "Download completed successfully! \n";
  string temp = destination_path + "/" + group_name + file_name;
  cout << temp << endl;
  string dp = destination_path + "/" + group_name + file_name;
  cout << "Download destination path is : " + dp + "\n";
  // cout << "Destination path: " + gg + "\n";
  // fp = fopen(gg.c_str(), "ab");
  int fp = open(dp.c_str(), O_CREAT | O_RDWR, 0777);
  if (fp < 0) {
    printf("Error opening file");
    return 1;
  }

  // DOWNLOADING STARTS HERE
  //  <userID, <fileID,<[D/C] [groupID] [fileID]>>>
  down_stat_map[user_name][file_name] =
      string("D") + " " + group_name + " " + file_name;
  // size_t sum =0;
  /* Receive data in chunks of 256 bytes */
  while ((bytesReceived = read(sockfd, recvBuff, 1024)) > 0) {
    // sz++;
    // gotoxy(0, 4);
    // printf("Received: %llf Mb", (sz / 1024));
    // sum = sum + bytesReceived;
    fflush(stdout);
    // recvBuff[n] = 0;
    // fwrite(recvBuff, 1, bytesReceived, fp);
    write(fp, recvBuff, bytesReceived);

    // printf("%s \n", recvBuff);
  }
  down_stat_map[user_name][file_name] =
      string("C") + " " + group_name + " " + file_name;

  if (bytesReceived < 0) {
    printf("\n Read Error \n");
  }

  // cout << "Total bytes read at the client end = " << sum << " bytes"<<
  // endl;
  // printf("\nFile OK....Completed\n");

  cout << "Before SHA checking[in DOWNLOAD], download file name: " +
              group_name + file_name + "\n";
  downloadSHAval = shaFunc(destination_path);
  if (downloadSHAval == uploadSHAval) {

    cout << "\nFile OK....Completed\n";
  } else {
    cout << "SHA values didn't matched!\n";
  }
  return 0;
}

int client(string ip, int port) {
  int sockfd = 0;
  int bytesReceived = 0;
  char recvBuff[1024];
  memset(recvBuff, '0', sizeof(recvBuff));
  struct sockaddr_in serv_addr;

  /* Create a socket first */
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("\n Error : Could not create socket \n");
    return 1;
  }
  int on = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
  /* Initialize sockaddr_in data structure */
  serv_addr.sin_family = AF_INET;
  // int trackerPortServer = 8087;
  serv_addr.sin_port = htons(port); // port

  // The inet_addr() function shall convert the string pointed to by cp,
  // in the standard IPv4 dotted decimal notation, to an integer value suitable
  // for use as an Internet address.
  //  string ip = "127.0.0.1";
  // string ipr = ip;
  serv_addr.sin_addr.s_addr = inet_addr(ip.c_str());

  /* Attempt a connection */
  if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    printf("\n Error : Connect Failed \n");
    return 1;
  }

  printf("Connected to ip: %s : %d\n", inet_ntoa(serv_addr.sin_addr),
         ntohs(serv_addr.sin_port));

  cin.ignore(); // used to ignore present input in cin  buffer stream

  // send command to the tracker
  while (1) {
    cout << "Enter the command to be sent to the tracker: ";
    string s;
    // getchar();
    getline(cin, s);

    // split the input command
    vector<string> commandVec; // used to store the words of the input command
    stringstream ss(s);        // to parse the input command
    string word;               // to store the individual words
    while (ss >> word) {
      commandVec.push_back(
          word); // push the parts of the command into the vector
    }

    if (commandVec[0] == "create_user") {
      if (commandVec.size() == 3) {
        // send command to tracker
        int size = commandVec.size();
        string sendMessage;
        for (int i = 0; i < size; i++) {
          sendMessage = sendMessage + commandVec[i] + " ";
        }

        write(sockfd, sendMessage.c_str(),
              sendMessage.size() - 1); // send message to socket
        cout << "Input command sent to the tracker by the peer is := "
             << commandVec[0] << " " << commandVec[1] << " " << commandVec[2]
             << " \n";
      } else {
        cout << "Invalid Command Input\n";
        exit(0);
      }

      // read the response from the tracker
      char Buff[8092];
      int totalBytes = read(sockfd, Buff, sizeof(Buff));
      string s1;
      for (int i = 0; i < totalBytes; i++) {
        s1 = s1 + Buff[i];
      }
      cout << "The message received at the peer end, sent by the tracker is: "
           << s1 << endl;
    } else if (commandVec[0] == "login") {
      if (commandVec.size() == 3) {
        // send command to tracker
        int size = commandVec.size();
        string sendMessage;
        for (int i = 0; i < size; i++) {
          sendMessage = sendMessage + commandVec[i] + " ";
        }
        sendMessage = sendMessage +
                      to_string(portPeerServer); // append port of peer's server
                                                 // at the end of the command
        write(sockfd, sendMessage.c_str(),
              sendMessage.size()); // send message to socket
        cout << "Input command sent to the tracker by the peer is := "
             << commandVec[0] << " " << commandVec[1] << " " << commandVec[2]
             << " \n";
      } else {
        cout << "Invalid Command Input\n";
        exit(0);
      }

      // read the response from the tracker
      char Buff[8092];
      int totalBytes = read(sockfd, Buff, sizeof(Buff));
      string s1;
      for (int i = 0; i < totalBytes; i++) {
        s1 = s1 + Buff[i];
      }
      cout << "The message received at the peer end, sent by the tracker is: "
           << s1 << endl;
    } else if (commandVec[0] == "create_group") {
      if (commandVec.size() == 2) {
        // send command to tracker
        int size = commandVec.size();
        string sendMessage;
        for (int i = 0; i < size; i++) {
          sendMessage = sendMessage + commandVec[i] + " ";
        }
        write(sockfd, sendMessage.c_str(),
              sendMessage.size() - 1); // send message to socket
        cout << "Input command sent to the tracker by the peer is := "
             << commandVec[0] << " " << commandVec[1] << " "
             << " \n";
      } else {
        cout << "Invalid Command Input\n";
        exit(0);
      }

      // read the response from the tracker
      char Buff[8092];
      int totalBytes = read(sockfd, Buff, sizeof(Buff));
      string s1;
      for (int i = 0; i < totalBytes; i++) {
        s1 = s1 + Buff[i];
      }
      cout << "The message received at the peer end, sent by the tracker is: "
           << s1 << endl;
    } else if (commandVec[0] == "join_group") {
      if (commandVec.size() == 2) {
        // send command to tracker
        int size = commandVec.size();
        string sendMessage;
        for (int i = 0; i < size; i++) {
          sendMessage = sendMessage + commandVec[i] + " ";
        }
        write(sockfd, sendMessage.c_str(),
              sendMessage.size() - 1); // send message to socket
        cout << "Input command sent to the tracker by the peer is := "
             << commandVec[0] << " " << commandVec[1] << " "
             << " \n";
      } else {
        cout << "Invalid Command Input\n";
        exit(0);
      }

      // read the response from the tracker
      char Buff[8092];
      int totalBytes = read(sockfd, Buff, sizeof(Buff));
      string s1;
      for (int i = 0; i < totalBytes; i++) {
        s1 = s1 + Buff[i];
      }
      cout << "The message received at the peer end, sent by the tracker is: "
           << s1 << endl;
    } else if (commandVec[0] == "list_requests") {
      if (commandVec.size() == 2) {
        // send command to tracker
        int size = commandVec.size();
        string sendMessage;
        for (int i = 0; i < size; i++) {
          sendMessage = sendMessage + commandVec[i] + " ";
        }
        write(sockfd, sendMessage.c_str(),
              sendMessage.size() - 1); // send message to socket
        cout << "Input command sent to the tracker by the peer is := "
             << commandVec[0] << " " << commandVec[1] << " "
             << " \n";
      } else {
        cout << "Invalid Command Input\n";
        exit(0);
      }

      // read the response from the tracker
      char Buff[8092];
      int totalBytes = read(sockfd, Buff, sizeof(Buff));
      string s1;
      for (int i = 0; i < totalBytes; i++) {
        s1 = s1 + Buff[i];
      }
      cout << "The message received at the peer end, sent by the tracker is: "
           << s1 << endl;
    } else if (commandVec[0] == "accept_request") {
      if (commandVec.size() == 3) {
        // send command to tracker
        int size = commandVec.size();
        string sendMessage;
        for (int i = 0; i < size; i++) {
          sendMessage = sendMessage + commandVec[i] + " ";
        }
        write(sockfd, sendMessage.c_str(),
              sendMessage.size() - 1); // send message to socket
        cout << "Input command sent to the tracker by the peer is := "
             << commandVec[0] << " " << commandVec[1] << " " << commandVec[2]
             << " \n";
      } else {
        cout << "Invalid Command Input\n";
        exit(0);
      }

      // read the response from the tracker
      char Buff[8092];
      int totalBytes = read(sockfd, Buff, sizeof(Buff));
      string s1;
      for (int i = 0; i < totalBytes; i++) {
        s1 = s1 + Buff[i];
      }
      cout << "The message received at the peer end, sent by the tracker is: "
           << s1 << endl;
    } else if (commandVec[0] == "list_groups") {
      if (commandVec.size() == 1) {
        // send command to tracker
        int size = commandVec.size();
        string sendMessage;
        for (int i = 0; i < size; i++) {
          sendMessage = sendMessage + commandVec[i] + " ";
        }
        write(sockfd, sendMessage.c_str(),
              sendMessage.size() - 1); // send message to socket
        cout << "Input command sent to the tracker by the peer is := "
             << commandVec[0] << " \n";
      } else {
        cout << "Invalid Command Input\n";
        exit(0);
      }

      // read the response from the tracker
      char Buff[8092];
      int totalBytes = read(sockfd, Buff, sizeof(Buff));
      string s1;
      for (int i = 0; i < totalBytes; i++) {
        s1 = s1 + Buff[i];
      }
      cout << "The message received at the peer end, sent by the tracker is: "
           << s1 << endl;
    } else if (commandVec[0] == "list_files") {
      if (commandVec.size() == 2) {
        // send command to tracker
        int size = commandVec.size();
        string sendMessage;
        for (int i = 0; i < size; i++) {
          sendMessage = sendMessage + commandVec[i] + " ";
        }
        write(sockfd, sendMessage.c_str(),
              sendMessage.size() - 1); // send message to socket
        cout << "Input command sent to the tracker by the peer is := "
             << commandVec[0] << " " << commandVec[1] << " \n";
      } else {
        cout << "Invalid Command Input\n";
        exit(0);
      }

      // read the response from the tracker
      char Buff[8092];
      int totalBytes = read(sockfd, Buff, sizeof(Buff));
      string s1;
      for (int i = 0; i < totalBytes; i++) {
        s1 = s1 + Buff[i];
      }
      cout << "The message received at the peer end, sent by the tracker is: "
           << s1 << endl;
    } else if (commandVec[0] == "upload_file") {
      if (commandVec.size() == 3) {
        cout << "Before SHA checking[in UPLOAD], upload file name: " +
                    commandVec[1] + "\n";
        cout << "CommandVec[1]: " << commandVec[1] << endl;
        cout << "Before fileName()\n";
        string fileName = returnFileName(commandVec[1]);
        cout << "FileName: " + fileName + "\n";
        uploadSHAval = shaFunc(commandVec[1]);
        cout << "after SHA func()";
        // send command to tracker
        // upload_file <file_path> <group_id>
        int size = commandVec.size();
        string sendMessage;
        for (int i = 0; i < size; i++) {
          sendMessage = sendMessage + commandVec[i] + " ";
        }
        cout << "before write!\n";
        write(sockfd, sendMessage.c_str(),
              sendMessage.size() - 1); // send message to socket
        cout << "Input command sent to the tracker by the peer is := "
             << commandVec[0] << " " << commandVec[1] << " " << commandVec[2]
             << " \n";

        // store the chunk info into the peer side
        // first get the size of the file & then
        // divide it by the chunk size to get the total no of chunks
        long long fileSize =
            GetFileSize(commandVec[1]); // returns size of the file in bytes

        int totalChunks = ceil(fileSize / 512000);

        // store the chunk numbers into the set
        for (int i = 0; i < totalChunks; i++) {
          chunk_map[fileName].insert(i + 1);
        }

      } else {
        cout << "Invalid Command Input\n";
        exit(0);
      }

      // read the response from the tracker
      char Buff[8092];
      int totalBytes = read(sockfd, Buff, sizeof(Buff));
      string s1;
      for (int i = 0; i < totalBytes; i++) {
        s1 = s1 + Buff[i];
      }
      cout << "The message received at the peer end, sent by the tracker is: "
           << s1 << endl;
    } else if (commandVec[0] == "download_file") {
      if (commandVec.size() == 4) {
        // download_file <group_id> <file_name>
        // <destination_path>
        // send command to tracker
        int size = commandVec.size();
        string sendMessage;
        for (int i = 0; i < size; i++) {
          sendMessage = sendMessage + commandVec[i] + " ";
        }
        write(sockfd, sendMessage.c_str(),
              sendMessage.size() - 1); // send message to socket
        cout << "Input command sent to the tracker by the peer is := "
             << commandVec[0] << " " << commandVec[1] << " " << commandVec[2]
             << " " << commandVec[3] << " \n";
      } else {
        cout << "Invalid Command Input\n";
        exit(0);
      }

      cout << "Reading response from tracker...\n";
      // read the response from the tracker
      char Buff[8092];
      // cout << "Before read\n";
      int totalBytes = read(sockfd, Buff, sizeof(Buff));
      // cout << "After read\n";
      string s1;
      for (int i = 0; i < totalBytes; i++) {
        s1 = s1 + Buff[i];
      }
      // s1 = $ ip1 port1 filePath1 ip2 port2 filePath2 ip3 port3 filePath3 ....
      char firstS1char = s1[0];
      string s2 = "";
      for (int i = 2; i < s1.length(); i++) {
        s2 = s2 + s1[i];
      }
      // s2 = ip1 port1 filePath1 ip2 port2 filePath2 ip3 port3 filePath3
      // ....currentUID
      /* cout << "String s1: " << s1 << endl;

      cout << "String s2: " << s2 << endl; */

      vector<string> addrVec; // used to store the  address of the input command
      stringstream ss(s2);    // to parse the input command
      string word;            // to store the individual words
      while (ss >> word) {
        addrVec.push_back(word); // push the parts of the socket into the vector
      }

      // CHUNK-SELECTION ALGO:

      // select IP::PORT from map which has least no of chunks and
      // which contains the given chunk & then insert it into final map

      // first store the IP::PORT in the map
      /*       pair<string, int> socketPair;

            for (int i = 0; i < s2.size(); i++) {
              string ip = addrVec[i];          // even positions contains IP
              int port = stoi(addrVec[i + 1]); // odd positions contains PORT
              socketPair.first = ip;
              socketPair.second = port;

              ip_port_chunk_map[socketPair].insert(i + 1);
            }

            // first count the total chunks of the file
            size_t fileSize = GetFileSize(commandVec[2]);
            size_t totalChunks = ceil(fileSize / 512000);
            size_t currentChunkID = 1;

            while (currentChunkID <= totalChunks) {

              // first search for IP::PORT that contains the chunkID
              for (auto it = ip_port_chunk_map.begin(); it !=
         ip_port_chunk_map.end(); it++) {

                if (it->second.find(currentChunkID) !=
                    it->second.end()) { // if currentChunk is present in the map

                  ip_port_chunk_map_final[it->first].insert(
                      currentChunkID); // then insert the [IP::PORT->ChunkID] in
         the
                                       // final-map
                  break;
                }
              }
              currentChunkID++;
            } */

      // now the second map contains the final list of peers which conatains all
      // the chunks of the files now apply thread on all of these peers & then
      // download chunks simultaneously

      // if first char of s1 is $ then the response[s2] contains ip port
      if (firstS1char == '$') {

        // piece selection algorithm
        //  select the peer you want to connect to
        //  and then request the file and download it

        string ip = addrVec[0];
        string port = addrVec[1];
        string filePath = addrVec[2];

        string file_name = commandVec[2];
        string destination_path = commandVec[3];
        string gname = commandVec[1];

        string uname = addrVec.back();

        cout << "Printing in peer's CLIENT:\n";
        cout << "ip: " << ip << endl;
        cout << "port: " << port << endl;
        cout << "filePath: " << filePath << endl;
        cout << "file_name: " << file_name << endl;
        cout << "destination_path: " << destination_path << endl;
        cout << "gname: " << gname << endl;
        cout << "user_name" << uname << endl;

        cout << "Before call to downloadFromPeer() \n";
        thread download(downloadFromPeer, ip, port, filePath, file_name,
                        destination_path, gname, uname);
        download.detach();
      } else { // if the first char is not $ then it is normal error message

        cout << "The message received at the peer end, sent by the tracker is: "
             << s1 << endl;
      }
    } else if (commandVec[0] == "logout") {
      if (commandVec.size() == 1) {
        // send command to tracker
        int size = commandVec.size();
        string sendMessage;
        for (int i = 0; i < size; i++) {
          sendMessage = sendMessage + commandVec[i] + " ";
        }
        write(sockfd, sendMessage.c_str(),
              sendMessage.size() - 1); // send message to socket
        cout << "Input command sent to the tracker by the peer is := "
             << commandVec[0] << " \n";
      } else {
        cout << "Invalid Command Input\n";
        exit(0);
      }

      // read the response from the tracker
      char Buff[8092];
      int totalBytes = read(sockfd, Buff, sizeof(Buff));
      string s1;
      for (int i = 0; i < totalBytes; i++) {
        s1 = s1 + Buff[i];
      }
      cout << "The message received at the peer end, sent by the tracker is: "
           << s1 << endl;
    } else if (commandVec[0] == "show_downloads") {
      if (commandVec.size() == 1) {
        // send command to tracker
        int size = commandVec.size();
        string sendMessage;
        for (int i = 0; i < size; i++) {
          sendMessage = sendMessage + commandVec[i] + " ";
        }
        write(sockfd, sendMessage.c_str(),
              sendMessage.size() - 1); // send message to socket
        cout << "Input command sent to the tracker by the peer is := "
             << commandVec[0] << " \n";
      } else {
        cout << "Invalid Command Input\n";
        exit(0);
      }

      // read the response from the tracker
      char Buff[8092];
      int totalBytes = read(sockfd, Buff, sizeof(Buff));
      string s1;
      for (int i = 0; i < totalBytes; i++) {
        s1 = s1 + Buff[i];
      }
      cout << "The message received at the peer end, sent by the tracker is: "
           << s1 << endl;

      for (unordered_map<string, string>::iterator it =
               down_stat_map[s1].begin();
           it != down_stat_map[s1].end(); ++it) {
        cout << it->second << endl;
      }
    } else if (commandVec[0] == "stop_share") {
      if (commandVec.size() == 3) {
        // send command to tracker
        int size = commandVec.size();
        string sendMessage;
        for (int i = 0; i < size; i++) {
          sendMessage = sendMessage + commandVec[i] + " ";
        }
        write(sockfd, sendMessage.c_str(),
              sendMessage.size() - 1); // send message to socket
        cout << "Input command sent to the tracker by the peer is := "
             << commandVec[0] << " " << commandVec[1] << " " << commandVec[2]
             << " \n";
      } else {
        cout << "Invalid Command Input\n";
        exit(0);
      }

      // read the response from the tracker
      char Buff[8092];
      int totalBytes = read(sockfd, Buff, sizeof(Buff));
      string s1;
      for (int i = 0; i < totalBytes; i++) {
        s1 = s1 + Buff[i];
      }
      cout << "The message received at the peer end, sent by the tracker is: "
           << s1 << endl;
    }
  }
}

#include <bits/stdc++.h>
#include <iostream>
using namespace std;

vector<string> lineVec;
void readLine(string FILENAME) {
  FILE *fp = fopen(FILENAME.c_str(), "r");
  if (fp == NULL)
    exit(EXIT_FAILURE);

  char *line = NULL;
  size_t len = 0;
  while ((getline(&line, &len, fp)) != -1) {
    // push each line into the vector
    lineVec.push_back(line);
  }
  fclose(fp);
  if (line)
    free(line);
}

int main(int argc, char *argv[]) {

  string IP_PORT(argv[1]);
  string trackerTextFile(argv[2]);

  string socket = IP_PORT;
  int found1 = socket.find_last_of(':');
  string PEER_SERVER_IP = socket.substr(0, found1);
  int PEER_SERVER_PORT =
      stoi(socket.substr(found1 + 1, socket.size() - found1));

  /* cout << "PEER_SERVER_IP: " + PEER_SERVER_IP + "\n";
  cout << "PEER_SERVER_PORT: " + to_string(PEER_SERVER_PORT) + "\n";
 */
  readLine(trackerTextFile);

  string s = lineVec.at(0);
  vector<string> trackSocVec; // used to store the words of the input command
  stringstream ss(s);         // to parse the input command
  string word;                // to store the individual words
  while (ss >> word) {
    trackSocVec.push_back(
        word); // push the parts of the command into the vector
  }

  /* for (int i = 0; i < trackSocVec.size(); i++) {
    string socket = trackSocVec[i];
    int found1 = socket.find_last_of(':');
    string PEER_SERVER_IP = socket.substr(0, found1);
    int PEER_CLIENT_PORT =
        stoi(socket.substr(found1 + 1, socket.size() - found1));
    int no = i + 1;
    cout << "PEER_CLIENT_IP" + to_string(no) + ": " + PEER_SERVER_IP + "\n";
    cout << "PEER_CLIENT_PORT" + to_string(no) + ": " +
  to_string(PEER_CLIENT_PORT) + "\n";
  } */

  string socket2 =
      trackSocVec[0]; // implemented single tracker so pick first one always
  int found2 = socket2.find_last_of(':');
  string PEER_CLIENT_IP = socket2.substr(0, found2);
  int PEER_CLIENT_PORT =
      stoi(socket2.substr(found2 + 1, socket2.size() - found2));

  /* cout << "PEER_CLIENT_IP: " + PEER_CLIENT_IP + "\n";
  cout << "PEER_CLIENT_PORT: " + to_string(PEER_CLIENT_PORT) + "\n"; */
  // cout << "FileNameOfTracker: " + trackerTextFile + "\n";

  // call server on a seperate thread which will always keep on running
  thread tServer(server, PEER_SERVER_IP, PEER_SERVER_PORT);
  sleep(1);

  client(PEER_CLIENT_IP, PEER_CLIENT_PORT);
  tServer.join();

  return 0;
}
