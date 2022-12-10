#include <arpa/inet.h>
#include <bits/stdc++.h>
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
#include <sys/types.h>
#include <thread>
#include <unistd.h>

using namespace std;

struct sockaddr_in c_addr;
// static char fname[100]; // used to store the filename

struct user_info {
  string user_id;
  string password;
  string clientIP;
  string serverPORT;
  bool isAlive{
      false}; // to keep the status whether the client is logged in or not
};

struct file_info {
  string fileID;
  string filePath;
  string SHA;
  string noOfChunks;
  // bool download {false};
  unordered_set<string> userID;
};

struct group_info {
  string group_id;
  string admin_id;
  deque<string> user_id_order{}; // used to store the ids for pending requests
  unordered_set<string>
      user_id_present{}; // used to store the ids present in the group
                         /*   map<string, vector<string>>
                               fileIDtoListOfUploaders{}; // <file_id, list_of_users>
                          */
  unordered_map<string, struct file_info> file_map; // <fileID, fileObject>
};

unordered_map<string, struct user_info> user_map;   // <userID, object>
unordered_map<string, struct group_info> group_map; // <groupID, object>

// string chunkCount() {}
// string SHA1Hash() {}

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

void sendData(int socketDiscriptor) {
  string currentUID = ""; // denotes the currently logged in user
  bool flag = true;
  while (
      flag) { // tracker will keep on listnening for the commands from the peer

    char ipBuff[1024];
    int totalBytes =
        read(socketDiscriptor, ipBuff,
             sizeof(ipBuff)); // read the data send by the peer into buffer
    string s;
    for (int i = 0; i < totalBytes; i++) {
      s = s + ipBuff[i];
    }
    // now parse the string
    vector<string> commandVec;
    stringstream ss(s); // to parse the input command
    string word;        // to store the individual words
    while (ss >> word) {
      commandVec.push_back(
          word); // push the parts of the command into the vector
    }
    cout << "Command received at the tracker is: " << s << "\n";

    if (commandVec[0] == "create_user") { // handle the create_user command
                                          // create_user <user_id> <password>
      string message = "";
      if (user_map.find(commandVec[1]) ==
          user_map.end()) { // if the user is not present
        user_map[commandVec[1]].user_id = commandVec[1];  // add the username
        user_map[commandVec[1]].password = commandVec[2]; // store the password

        // RESPONSE
        cout << "User " + commandVec[1] +
                    " created successfully!\n"; // @tracker
        message = "User " + commandVec[1] + " created successfully!\n"; // @peer
      } else { // if the user is already present

        // RESPONSE
        cout << "User " + commandVec[1] + " is already present!\n"; // @tracker
        message = "User " + commandVec[1] + " is already present!\n"; // @peer
      }

      // give response message back to the peer
      write(socketDiscriptor, message.c_str(),
            message.size()); // send message to socket
      cout << "Acknowledgement sent to the peer!\n";
      cout << "WAITING...\n";
      cout << endl;

    } else if (commandVec[0] == "login") { // handle the login command
                                           // login <user_id> <password>
      string message = "";
      // if the CurrentUser is already present
      if (currentUID != "") {
        // some user is already logged in and it's entry is also present

        // RESPONSE
        cout << "User " + commandVec[1] +
                    " is already logged in!\n"; // @tracker
        message = "User " + commandVec[1] + " is already logged in!\n"; // @peer
      }
      // if the CurrentUser is not present
      else if (user_map.find(commandVec[1]) ==
               user_map.end()) { // if the user is not present in the db

        // RESPONSE
        cout << "User " + commandVec[1] +
                    " is not present in the db\n"; // @tracker
        message =
            "User " + commandVec[1] + " is not present in the db\n"; // @peer
      } else if (user_map[commandVec[1]].isAlive) { // user logged in on another
                                                    // terminal
        // RESPONSE
        cout
            << "User " + commandVec[1] +
                   " is already logged in from another terminal!\n"; // @tracker
        message = "User " + commandVec[1] +
                  " is already logged in from another terminal!\n"; //  @peer
      } else { // if the user is already present but not logged in
        // first check for correct userID & Password
        if (user_map.find(commandVec[1]) ==
            user_map.end()) { // means the user is not present in the map

          // RESPONSE
          cout << "User " + commandVec[1] + " doesn't exist!\n";   // @tracker
          message = "User " + commandVec[1] + " doesn't exist!\n"; // @peer
        } else { // user is present now check for password
          if (user_map[commandVec[1]].password ==
              commandVec[2]) { // if the passwords match

            user_map[commandVec[1]].isAlive = true;
            currentUID = commandVec[1];

            // store the IP & PORT of the  peers
            user_map[commandVec[1]].clientIP = inet_ntoa(c_addr.sin_addr);
            user_map[commandVec[1]].serverPORT =
                commandVec[3]; // store the peer's Server Port

            // RESPONSE
            cout << "User " + commandVec[1] +
                        " is successfully logged in!\n"; // @tracker
            message = "User " + commandVec[1] +
                      " is successfully logged in!\n"; // @peer
          } else {                                     // incorrect password

            // RESPONSE
            cout << "Password for User " + commandVec[1] +
                        " is incorrect!\n"; // @tracker
            message = "Password for User " + commandVec[1] +
                      " is incorrect!\n"; // @peer
          }
        }
      }

      // give response message back to the peer
      write(socketDiscriptor, message.c_str(),
            message.size()); // send message to socket
      cout << "Acknowledgement sent to the peer!\n";
      cout << "WAITING...\n";
      cout << endl;
    } else if (commandVec[0] ==
               "create_group") { // handle the create_group command
      // create_group <group_id>
      string message = "";
      if (user_map.find(commandVec[1]) ==
          user_map.end()) { // if the group is not present
        // make the currentUID as admin and add it to the group

        if (user_map[currentUID]
                .isAlive) { // the current user must be logged in
          group_map[commandVec[1]].group_id = commandVec[1];
          group_map[commandVec[1]].admin_id =
              currentUID; // enter user_id of the user who sent the request
          group_map[commandVec[1]].user_id_present.insert(
              currentUID); // add the current user to the group

          // RESPONSE
          cout << "Group " + commandVec[1] +
                      " successfully created!\n"; // @tracker
          message =
              "Group " + commandVec[1] + " successfully created!\n"; // @peer
        } else { // when the current user is not logged in

          // RESPONSE
          cout << "User " + currentUID + " is not logged in!\n";   //@tracker
          message = "User " + currentUID + " is not logged in!\n"; //@peer
        }

      } else { // if the group is already present

        // RESPONSE
        cout << "Group " + commandVec[1] + "is already present!\n"; // @tracker
        message = "Group " + commandVec[1] + "is already present!\n"; // @peer
      }

      // give response message back to the peer
      write(socketDiscriptor, message.c_str(),
            message.size()); // send message to socket
      cout << "Acknowledgement sent to the peer!\n";
      cout << "WAITING...\n";
      cout << endl;
    } else if (commandVec[0] == "join_group") { // handle the join_group command
                                                // join_group <group_id>
      string message = "";
      // first check whether the group exist or not
      if (group_map.find(commandVec[1]) ==
          group_map.end()) { // the group doesn't exists

        // RESPONSE
        cout << "Group " + commandVec[1] + " doesn't exist!\n";   // @tracker
        message = "Group " + commandVec[1] + " doesn't exist!\n"; // @peer
      } else { // if the group already exist
        // first check whether it is already user or not
        if (group_map[commandVec[1]].user_id_present.find(currentUID) ==
            group_map[commandVec[1]]
                .user_id_present
                .end()) { // the user is not present in the group

          if (user_map[currentUID]
                  .isAlive) { // when the user is already logged in
            group_map[commandVec[1]].user_id_order.push_back(
                currentUID); // push the user into the dequeue

            // RESPONSE
            cout
                << "User " + currentUID +
                       " successfully added to the request queue!\n"; // @tracker
            message = "User " + currentUID +
                      " successfully added to the request queue!\n"; // @peer
          } else { // when the user is not logged in

            // RESPONSE
            cout << "User " + currentUID + " is not logged in!\n";   // @tracker
            message = "User " + currentUID + " is not logged in!\n"; // @peer
          }

        } else { // if the user already is present in the group
          // RESPONSE
          cout << "User " + currentUID +
                      " is already present in the group!\n"; // @tracker
          message = "User " + currentUID +
                    " is already present in the group!\n"; // @peer
        }
      }

      // give response message back to the peer
      write(socketDiscriptor, message.c_str(),
            message.size()); // send message to socket
      cout << "Acknowledgement sent to the peer!\n";
      cout << "WAITING...\n";
      cout << endl;

    } else if (commandVec[0] == "leave_group") { // handle the leave_group
                                                 // leave_group <group_id>
      string message = "";

      if (group_map.find(commandVec[1]) ==
          group_map.end()) { // if the group is not present
        // RESPONSE
        cout << "Group " + commandVec[1] + " doesn't exists!\n";   // @tracker
        message = "Group " + commandVec[1] + " doesn't exists!\n"; // @peer
      } else { // the group is present
               // first check whether it is already user or not
        if (group_map[commandVec[1]].user_id_present.find(currentUID) ==
            group_map[commandVec[1]]
                .user_id_present.end()) { // the user is not present
          // RESPONSE
          cout << "User " + currentUID +
                      " is not present in the group!\n"; // @tracker
          message =
              "User " + currentUID + " is not present in the group!\n"; // @peer

        } else { // if the user already is present in the group
          // now check whether admin is leaving or non-admin is leaving
          if (group_map[commandVec[1]].admin_id == currentUID &&
              user_map[currentUID].isAlive) { // if admin is leaving the group
            // if it is the only one present in the group then delete the group
            if (group_map[commandVec[1]].user_id_present.size() == 1) {
              group_map.erase(commandVec[1]);

              // RESPONSE
              cout << "Only admin was present in the group so we deleted the " +
                          commandVec[1] + " group!\n"; // @tracker
              "Only admin was present in the group so we deleted the " +
                  commandVec[1] + " group!\n"; // @peer
            } else {
              // make the next element as the admin & then remove the admin from
              // the list of users
              group_map[commandVec[1]].admin_id =
                  group_map[commandVec[1]].user_id_order.front();
              group_map[commandVec[1]].user_id_order.pop_front();
              group_map[commandVec[1]].user_id_present.erase(currentUID);

              // RESPONSE
              cout << "User " + currentUID +
                          " successfully left the group!\n"; // @tracker
              message = "User " + currentUID +
                        " successfully left the group!\n"; // @peer
            }
          } else if (group_map[commandVec[1]].admin_id != currentUID &&
                     user_map[currentUID]
                         .isAlive) { // when non-admin is leaving the group
            group_map[commandVec[1]].user_id_present.erase(currentUID);

            // RESPONSE
            cout << "User " + currentUID +
                        " successfully left the group!\n"; // @tracker
            message = "User " + currentUID +
                      " successfully left the group!\n"; // @peer
          } else { // when the user is not logged in

            // RESPONSE
            cout << "User " + currentUID + " is not logged in!\n";   // @tracker
            message = "User " + currentUID + " is not logged in!\n"; // @peer
          }
        }
      }

      // give response message back to the peer
      write(socketDiscriptor, message.c_str(),
            message.size()); // send message to socket
      cout << "Acknowledgement sent to the peer!\n";
      cout << "WAITING...\n";
      cout << endl;
    } else if (commandVec[0] == "list_requests") { // handle the list_requests
                                                   // command[list pending join]
      // list_requests<group_id>
      string message = "";
      // first check whether the group is present or not
      if (group_map.find(commandVec[1]) ==
          group_map.end()) { // if the group is not present

        // RESPONSE
        cout << "Group " + commandVec[1] + " is not present!\n";   // @tracker
        message = "Group " + commandVec[1] + " is not present!\n"; // @peer
      } else { // the group is present

        if (user_map[currentUID].isAlive &&
            group_map[commandVec[1]].user_id_present.find(currentUID) !=
                group_map[commandVec[1]]
                    .user_id_present
                    .end()) { // the user should be logged in as well as it
                              // should be present in the group

          if (group_map[commandVec[1]].user_id_present.size() >
              0) { // if the set is non-empty

            // RESPONSE
            message = "The list of users who are yet to join the Group " +
                      commandVec[1] + " are:";
            for (auto it = group_map[commandVec[1]].user_id_order.begin();
                 it != group_map[commandVec[1]].user_id_order.end(); ++it) {
              message = message + " " + *it;
            } // @peer end

            cout << message; //@tracker end
          } else {

            // RESPONSE
            cout
                << "There are no users left in the pending join queue!\n"; // @tracker
            message =
                "There are no users left in the pending join queue!\n"; // @peer
          }

        } else {
          // as the current user is not logged-in or not part of the group, so
          // we can't show the list

          // RESPONSE
          cout << "User " + currentUID +
                      " doesn't have permission to view the list "
                      "of pending requests!\n"; // @tracker
          message = "User " + currentUID +
                    " doesn't have permission to view the list "
                    "of pending requests!\n"; // @peer
        }
      }

      // give response message back to the peer
      write(socketDiscriptor, message.c_str(),
            message.size()); // send message to socket
      cout << "Acknowledgement sent to the peer!\n";
      cout << "WAITING...\n";
      cout << endl;
    } else if (commandVec[0] ==
               "accept_request") { // handle the accept_request command
      // accept_request <group_id> <user_id>
      string message = "";
      // first check whether it is a valid group or not
      if (group_map.find(commandVec[1]) ==
          group_map.end()) { // if the group is not present
        // RESPONSE
        cout << "Group " + commandVec[1] + " is not present!\n";   // @tracker
        message = "Group " + commandVec[1] + " is not present!\n"; // @peer
      } else { // the group is present
        // now pending request will be approved only by the admin and it should
        // be logged in
        if (user_map[group_map[commandVec[1]].admin_id]
                .isAlive) { // current user is admin & logged in so approve
          // now check whether the given user is already present in the group or
          // not
          if (group_map[commandVec[1]].user_id_present.find(commandVec[2]) ==
              group_map[commandVec[1]]
                  .user_id_present
                  .end()) { // if the user is not present in the group
            // now remove the front user from the dequeue and add it to the set
            // of users in the group
            group_map[commandVec[1]].user_id_present.insert(
                group_map[commandVec[1]].user_id_order.front());
            group_map[commandVec[1]].user_id_order.pop_front();

            // RESPONSE
            cout
                << "User " + commandVec[2] +
                       " is successfully added by admin to the group!\n"; // @tracker
            message =
                "User " + commandVec[2] +
                " is successfully added by admin to the group!\n"; // @peer
          } else { // if the user is already present in the group

            // RESPONSE
            cout << "User " + commandVec[2] + " is already present in the " +
                        commandVec[1] + " group!\n"; // @tracker
            message = "User " + commandVec[2] + " is already present in the " +
                      commandVec[1] + " group!\n"; //@peer
          }

        } else { // admin is not logged in

          // RESPONSE
          cout << "User " + group_map[commandVec[1]].admin_id +
                      "[ADMIN] is not logged in, so we can't approve this "
                      "request!\n"; //@tracker
          message = "User " + group_map[commandVec[1]].admin_id +
                    "[ADMIN] is not logged in, so we can't approve this "
                    "request!\n"; //@peer
        }
      }

      // give response message back to the peer
      write(socketDiscriptor, message.c_str(),
            message.size()); // send message to socket
      cout << "Acknowledgement sent to the peer!\n";
      cout << "WAITING...\n";
      cout << endl;
    } else if (commandVec[0] ==
               "list_groups") { // handle the list_groups command
      // list_groups
      string message = "";

      if (user_map[currentUID].isAlive) { // first check whether the currentUID
                                          // is logged in or not

        // check whether group_map contains elements or not
        if (group_map.empty()) { // when group_map is empty

          // RESPONSE
          cout << "There are no groups present in the network!\n";   // @tracker
          message = "There are no groups present in the network!\n"; // @peer
        } else { // when the group_map is not empty

          // RESPONSE
          message = "The list of groups present in the network are: ";
          for (auto itr = group_map.begin(); itr != group_map.end(); ++itr) {
            message = message + itr->first + " ";
          } // @peer

          cout << message; // @tracker
        }
      } else { // currentUID is not logged in

        // RESPONSE
        cout << "User " + currentUID + " is not logged in!\n";   // @tracker
        message = "User " + currentUID + " is not logged in!\n"; // @peer
      }

      // give response message back to the peer
      write(socketDiscriptor, message.c_str(),
            message.size()); // send message to socket
      cout << "Acknowledgement sent to the peer!\n";
      cout << "WAITING...\n";
      cout << endl;
    } else if (commandVec[0] ==
               "list_files") { // handle the list_files command[print all the
                               // sharable files in the network]
      // list_files <group_id>
      string message = "";
      // first check whether the group is present or not
      if (group_map.find(commandVec[1]) ==
          group_map.end()) { // if the group is not present
        // RESPONSE
        cout << "Group " + commandVec[1] + " is not present!\n";   // @tracker
        message = "Group " + commandVec[1] + " is not present!\n"; // @peer
      } else {                              // the group is present
        if (user_map[currentUID].isAlive) { // now check whether the currentUID
                                            // is logged in or not

          // now check whether the files are present or not
          if (group_map[commandVec[1]].file_map.size() >
              0) { // there are some files present in the group

            // RESPONSE
            message = "The list of files present in the Group " +
                      commandVec[1] + " are:";
            for (auto it : group_map[commandVec[1]].file_map) {
              message += " " + it.first;
            }                        // @peer
            cout << message << endl; // @tracker
          } else {                   // when no file is present in the network

            // RESPONSE
            cout << "No file is present is present in the Group " +
                        commandVec[1] + " !\n"; // @tracker
            message = "No file is present is present in the Group " +
                      commandVec[1] + " !\n"; // @peer
          }
        } else { // currentUID is not logged in

          // RESPONSE
          cout << "User " + currentUID + " is not logged in!\n";   // @tracker
          message = "User " + currentUID + " is not logged in!\n"; // @peer
        }
      }

      // give response message back to the peer
      write(socketDiscriptor, message.c_str(),
            message.size()); // send message to socket
      cout << "Acknowledgement sent to the peer!\n";
      cout << "WAITING...\n";
      cout << endl;

    } else if (commandVec[0] ==
               "upload_file") { // handle the upload_file command
      // upload_file <file_path> <group_id>
      string message = "";
      string fileName = returnFileName(commandVec[1]);
      // first check whether the group is present or not
      if (group_map.find(commandVec[2]) ==
          group_map.end()) { // if the group is not present

        // RESPONSE
        cout << "Group " + commandVec[2] + " is not present!\n";   //@tracker
        message = "Group " + commandVec[2] + " is not present!\n"; // @peer

      } else { // the group is present
        // now check whether the currenUID is logged in or not
        if (user_map[currentUID].isAlive) { // currentUID is logged in

          // now handle code for file upload
          group_map[commandVec[2]].file_map[fileName].fileID = fileName;
          group_map[commandVec[2]].file_map[fileName].noOfChunks =
              ""; // call functions
          group_map[commandVec[2]].file_map[fileName].SHA =
              ""; // call functions
          group_map[commandVec[2]].file_map[fileName].userID.insert(currentUID);
          group_map[commandVec[2]].file_map[fileName].filePath =
              commandVec[1]; // store the filePath

          // RESPONSE
          cout << "Successfully uploaded the file " + fileName +
                      " to the tracker!\n"; // @tracker
          message = "Successfully uploaded the file " + fileName +
                    " to the tracker!\n"; // @peer
        } else {                          // currentUID is not logged in

          // RESPONSE
          cout << "User " + currentUID + " is not logged in!\n";   // @tracker
          message = "User " + currentUID + " is not logged in!\n"; // @peer
        }
      }

      // give response message back to the peer
      write(socketDiscriptor, message.c_str(),
            message.size()); // send message to socket
      cout << "Acknowledgement sent to the peer!\n";
      cout << "WAITING...\n";
      cout << endl;

    } else if (commandVec[0] ==
               "download_file") { // handle the download_file command
                                  // download_file <group_id> <file_name>
                                  //  <destination_path>

      string message = "";
      // first check whether the group is present or not
      if (group_map.find(commandVec[1]) ==
          group_map.end()) { // if the group is not present

        // RESPONSE
        cout << "Group " + commandVec[1] + " is not present!\n";   //@tracker
        message = "Group " + commandVec[1] + " is not present!\n"; // @peer

      } else { // the group is present
        // now check whether the currenUID is logged in or not
        if (user_map[currentUID].isAlive) { // currentUID is logged in
          // handle file name
          if (group_map[commandVec[1]].file_map.find(commandVec[2]) ==
              group_map[commandVec[1]]
                  .file_map.end()) { // file is not present in the map

            // RESPONSE
            cout << "Cannot download the file as the " + commandVec[2] +
                        " file is not present @Tracker!\n";
            message = "Cannot download the file as the " + commandVec[2] +
                      " file is not present @Tracker!\n";
          } else { // file is present in the map

            // iterate over file-map set users
            message = "$";
            for (auto it = group_map[commandVec[1]]
                               .file_map[commandVec[2]]
                               .userID.begin();
                 it !=
                 group_map[commandVec[1]].file_map[commandVec[2]].userID.end();
                 it++) {

              // check for logged in users
              if (user_map[*it].isAlive) { // if the user is logged in
                // store the IP & PORT & filePath of the logged in peers

                string ip = user_map[*it].clientIP;
                string port = user_map[*it].serverPORT;
                string filePath =
                    group_map[commandVec[1]].file_map[commandVec[2]].filePath;
                // cout << "PORT: " + port + "\n";
                message += " " + ip + " " + port + " " +
                           filePath; // append ip and port and filePath
              }
            }

            message = message + " " + currentUID; // append currentUID at the end
          }

        } else { // currentUID is not logged in

          // RESPONSE
          cout << "User " + currentUID + " is not logged in!\n";   // @tracker
          message = "User " + currentUID + " is not logged in!\n"; // @peer
        }
      }

      cout << "The port::ip sent to peer for downloads are: " + message + " \n";
      // give response message back to the peer
      write(socketDiscriptor, message.c_str(),
            message.size()); // send message to socket
      cout << "Acknowledgement sent to the peer!\n";
      cout << "WAITING...\n";
      cout << endl;

    } else if (commandVec[0] == "logout") { // handle the logout command
      // logout
      string message;

      if (user_map[currentUID].isAlive) { // currentUID must be logged in first
        user_map[currentUID].isAlive = false;
        string user = currentUID;
        currentUID = ""; // make it empty as no one is logged in
        user_map[currentUID].serverPORT =
            ""; // as we are logging out make port as null

        // RESPONSE
        cout << "User " + user + " successfully logged out!\n";   // @tracker
        message = "User " + user + " successfully logged out!\n"; // @peer

      } else { // user was already logged out

        // RESPONSE
        cout << "User " + currentUID + " is already logged out!\n"; // @tracker
        message = "User " + currentUID + " is already logged out!\n"; //@peer
      }

      // give response message back to the peer
      write(socketDiscriptor, message.c_str(),
            message.size()); // send message to socket
      cout << "Acknowledgement sent to the peer!\n";
      cout << "WAITING...\n";
      cout << endl;
    } else if (commandVec[0] ==
               "show_downloads") { // handle the show_downloads command
      // show_downloads
      string message = "";
      // if current user is logged in then only send

      if (user_map[currentUID].isAlive) {
        // send current_UID, gname & fname
        cout << "Reply[currently logged in user] sent to peer!\n";
        string uname = currentUID;
        message = uname;
      } else { // user was already logged out

        // RESPONSE
        cout << "User " + currentUID + " is already logged out!\n"; // @tracker
        message = "User " + currentUID + " is already logged out!\n"; //@peer
      }

      // give response message back to the peer
      write(socketDiscriptor, message.c_str(),
            message.size()); // send message to socket
      cout << "Acknowledgement sent to the peer!\n";
      cout << "WAITING...\n";
      cout << endl;

    } else if (commandVec[0] == "stop_share") { // handle the stop_share command
      // stop_share <group_id> <file_name>
    } else {
      cout << "Invalid Command Input\n";
      exit(1);
    }
  }
}

int server(string ip, int port) {

  // code for commands
  int connfd = 0, err;          // store thread id
  struct sockaddr_in serv_addr; // store socket address
  int listenfd = 0, ret;
  char sendBuff[1025]; // buffer size
  memset(sendBuff, 0, sizeof(sendBuff));
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

  serv_addr.sin_port =
      htons(port); // htons() used to store the number in network byte order
                   // i.e. BIG-ENDIAN format(largest byte goes first)

  // serv_addr.sin_addr.s_addr = inet_addr(ip.c_str());
  /* cout << "Enter the port no. for Tracker: ";
  cin >> portTracker; */
  // portTracker = 8087;
  // binds the socket with the ip address
  ret = bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  if (ret < 0) {
    printf("Error in bind\n");
    exit(2); // terminates the calling process
  }

  // keep on listening connections on socket listenfd
  if (listen(listenfd, 10) == -1) {
    printf("Failed to listen\n");
    return -1;
  }
  int count = -1;
  while (1) { // wait for client to connect; it will always keep on listening

    clen = sizeof(c_addr);
    count++;
    cout << "Waiting for Client " << count << "...\n";
    // printf("Waiting...\n");
    //  cout<<"Waiting\n";
    //  cout << "listenfd :" << listenfd << endl;
    connfd = accept(listenfd, (struct sockaddr *)&c_addr, (socklen_t *)&clen);
    // cout << "PRINT1\n";
    if (connfd < 0) {
      printf("Error in accept\n");
      continue;
    }
    /* int valread;
    valread = read(connfd, sendBuff, 1025);
    // printf("%s\n", sendBuff);
    cout << valread << " bytes were read into the tracker's buffer.\n";
    cout << "Printing message at the tracker end: " << string(sendBuff) ;
     */

    thread tPeer(sendData, connfd);
    // tPeer.detach();
    tPeer.detach();
    /* send(new_socket, hello, strlen(hello), 0);
    printf("Hello message sent\n"); */

    // closing the connected socket
    // close(listenfd);
    // closing the listening socket
    // shutdown(listenfd, SHUT_RDWR);

    /*  if (err != 0)
         printf("\ncan't create thread :[%s]", strerror(err)); */
  }
}

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

void quitFunc() {

  while (1) {
    string s;
    getline(cin, s);
    if (s == "quit") {
      exit(1);
    }
  }
}

int main(int argc, char *argv[]) {

  thread quitThread(quitFunc);
  quitThread.detach();

  string trackerTextFile(argv[1]);
  string trackerNo(argv[2]);
  int trackNo = stoi(trackerNo);

  readLine(trackerTextFile);

  // split the input
  string s = lineVec.at(0);
  vector<string> socVec; // used to store the words of the input command
  stringstream ss(s);    // to parse the input command
  string word;           // to store the individual words
  while (ss >> word) {
    socVec.push_back(word); // push the parts of the command into the vector
  }

  string s1 = socVec[trackNo];
  int found1 = s1.find_last_of(':');
  string TRACKER_IP = s1.substr(0, found1);
  int TRACKER_PORT = stoi(s1.substr(found1 + 1, s1.size() - found1));

  server(TRACKER_IP, TRACKER_PORT);

  /*  cout << "IP : " + TRACKER_IP + "\n";
   cout << "PORT : " + to_string(TRACKER_PORT) + "\n"; */
}