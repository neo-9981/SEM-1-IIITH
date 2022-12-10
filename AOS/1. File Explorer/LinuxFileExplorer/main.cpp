/*** includes ***/
#include <bits/stdc++.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include <iostream>
#include <limits>
#include <stack>
#include <string>
#include <vector>

//#define PATH_MAX 256
using namespace std;

char *HOME = getpwuid(getuid())->pw_dir;
vector<string> current_entry;  // will store the directory/file entries
// I will always maintain the current path onto the left stack
stack<string> leftStack;   // will store absolute paths while we go back
stack<string> rightStack;  // will store the absolute paths while we go forward
int fileStart = 0;         // starting entry index of current directory/file
int fileEnd = 0;           // ending entry index of current directory/file
int currStartIndx;         // store the current start index of the window
int currEndIndx;           // store the current end index of the window
int Cx;                    // x-coordinate of current cursor
int Cy;                    // y-coordinate of current cursor
int k = 10;                // k denotes window size

//  Command Mode Variables

string commandModeInputString;          // string to take the command
vector<string> commandModeInputVector;  // tokenized vector containing command

// functionality to convert char *array to string
string pathConvertToString(char *arr) {
  string str;
  for (int i = 0; i < PATH_MAX; i++) {
    if (arr[i])
      str.push_back(arr[i]);
    else
      break;
  }
  return str;
}

// gives the real path
string giveRealPath(string path) {
  char cr[PATH_MAX];
  realpath(path.c_str(), cr);
  string convertedPath = string(cr);
  return convertedPath;
}

// get the directory path from where we started the program & convert it to
// string
char *currentDirectoryPath = get_current_dir_name();
string currentDirectoryPathString = pathConvertToString(currentDirectoryPath);

// used to move the cursor to a <x,y> co-ordinate on the screen
void gotoxy(int x, int y) { printf("\033[%d;%dH", x, y); }

#define CTRL_KEY(k) ((k)&0x1f)

struct termios orig_termios;

void disableRawMode() {
  struct termios orig_termios;
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode() {
  struct termios raw;
  atexit(disableRawMode);
  tcgetattr(STDIN_FILENO, &raw);
  raw.c_lflag &= ~(ECHO);
  raw.c_lflag &= ~(ICANON);
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

char editorReadKey() {
  int nread;
  char c;
  while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    if (nread == -1 && errno != EAGAIN)
      ;
  }
  return c;
}

// clears the screen completely & move to the top left-corner
void editorRefreshScreen() {
  write(STDOUT_FILENO, "\x1b[H", 3);   // moves to the top left-corner
  write(STDOUT_FILENO, "\x1b[2J", 4);  // clears the screen
}

void editorProcessKeypress() {
  char c = editorReadKey();

  switch (c) {
    case CTRL_KEY('q'):
      exit(0);
      break;
  }
}

void clearRightStack() {
  while (!rightStack.empty()) {
    rightStack.pop();
  }
}

// used to populate the vector with current directory elements
void listDirectoryFiles(string dirname) {
  current_entry.clear();  // reset the vector before refilling it
  cout << current_entry.size() << endl;
  DIR *dir =
      opendir(dirname.c_str());  // return pointer to the directory on which we
                                 // are performing our operations
  // dirname is going to be our directory which we are going to open (both
  // absolute path & relative path) perform all the operations on the directory
  if (dir == NULL) {
    return;
  }

  // actual processing begins here
  // printf("Reading files in: %s\n", dirname);

  struct dirent *entity;  // here we are declaring a pointer entity to struct
                          // dirent which is used to iterate over all the
                          // elements of the directory(i.e. files & folders)that
                          // we have opened
  entity = readdir(dir);  // start reading the directory's content

  while (entity != NULL) {  // while there are elements in the directory

    current_entry.push_back(pathConvertToString(entity->d_name));
    entity = readdir(dir);
  }

  closedir(dir);  // closing the dir pointer
  sort(current_entry.begin(),
       current_entry.end());  // sort the entries of the vector
  // update start and end of file/folder(vector)
  fileStart = 0;
  fileEnd = current_entry.size() - 1;
}

// it will print the files in the current window size
void printK_DirectoryFiles() {
  /* if (currEndIndx > fileEnd) {  // check for out of bound access
   currEndIndx = fileEnd;
 }
*/
  // starting printing elements of current window
  for (int i = currStartIndx; i <= min(fileEnd, currEndIndx); i++) {
    struct stat stat_obj {};  // used to get stats for file/folder

    string path;
    if (currentDirectoryPathString == "/") {
      path = currentDirectoryPathString + current_entry[i];
    } else {
      // store the path of the element in the vector to path variable
      path = currentDirectoryPathString + "/" + current_entry[i];
    }

    stat(path.c_str(),
         &stat_obj);  // get the stat of the current element of the vector
    {
      cout << setw(20) << left << current_entry[i] << setw(6) << left
           << int(stat_obj.st_size) << " ";

      struct passwd *u_name = getpwuid(stat_obj.st_uid);
      struct group *g_name = getgrgid(stat_obj.st_gid);

      cout << u_name->pw_name << " ";
      cout << g_name->gr_name << " ";

      cout << (S_ISDIR(stat_obj.st_mode) ? 'd' : '-');
      cout << ((stat_obj.st_mode & S_IRUSR) ? 'r' : '-');
      cout << ((stat_obj.st_mode & S_IWUSR) ? 'w' : '-');
      cout << ((stat_obj.st_mode & S_IXUSR) ? 'x' : '-');
      cout << ((stat_obj.st_mode & S_IRGRP) ? 'r' : '-');
      cout << ((stat_obj.st_mode & S_IWGRP) ? 'w' : '-');
      cout << ((stat_obj.st_mode & S_IXGRP) ? 'x' : '-');
      cout << ((stat_obj.st_mode & S_IROTH) ? 'r' : '-');
      cout << ((stat_obj.st_mode & S_IWOTH) ? 'w' : '-');
      cout << ((stat_obj.st_mode & S_IXOTH) ? 'x' : '-');

      char *s = ctime(&stat_obj.st_mtime);
      s[20] = '\0';
      cout << " " << s;
    }
    cout << endl;
  }
}

// functionality for the enter key press
void enterKeyFunction() {
  // get the current cursor position
  // using that cursor position retrieve the d->name from the vector
  // now

  // update the currentDirectoryPathString only when we enter into directory
  // when we open the file no need to push current path onto the stack as its is
  // already present onto the leftStack top
  struct stat stat_obj {};  // used to get stats for file/folder
  leftStack.push(currentDirectoryPathString);
  clearRightStack();

  string path =
      currentDirectoryPathString + "/" + current_entry[currStartIndx + Cx - 1];
  stat(path.c_str(), &stat_obj);
  if (S_ISDIR(stat_obj.st_mode)) {  // to enter into directory
    // update the cursor to the top-left corner
    Cx = 1;
    Cy = 1;
    currentDirectoryPathString =
        path;  // update the path to the new directory path

    // push the current directory path onto the leftStack to maintain current
    // directory path into the leftStack
    listDirectoryFiles(
        currentDirectoryPathString);  // update the vector to store the entries
                                      // of the new directory

    // clear the screen & then move the cursor to the top-left corner
    editorRefreshScreen();

    printK_DirectoryFiles();  // print the complete file
    gotoxy(Cx, Cy);           // move the cursor to the top left corner
  } else {  // to enter into file into it's default application

    pid_t pid = fork();
    if (pid == 0) {  // open the file into the child process
      execl("/usr/bin/xdg-open", "xdg-open", path.c_str(), NULL);
      exit(1);
    }
    // parent will usually wait for child here
  }
}

// functionality for the backspace key press
void backspaceKeyFunction(string currentDirectoryPathString) {
  // first we will get the current directory path
  // then we will remove dirname from the end of the path string
  // now we will pass this updated path to the listDirectory function to update
  // the vector with the last directory elements now we will print all the
  // elements of the directory

  // update the cursor to the top-left corner

  char ch = '/';
  int lastIndex = currentDirectoryPathString.find_last_of(ch);
  string parent = currentDirectoryPathString.substr(0, lastIndex);
  if (parent != currentDirectoryPathString) {
    Cx = 1;
    Cy = 1;
    leftStack.push(currentDirectoryPathString);
    clearRightStack();

    currentDirectoryPathString = parent;

    /* // appending '/..' to newPath so that we will move one level back
    currentDirectoryPathString = currentDirectoryPathString + "/..";
     */// push the current directory path onto the leftStack to maintain current directory path into the leftStack
    listDirectoryFiles(
        currentDirectoryPathString);  // updating the vector with the elements
                                      // of the previous directory
    // clear the screen & then move the cursor to the top-left corner
    editorRefreshScreen();

    printK_DirectoryFiles();  // printing all the elements of the previous
                              // directory
    gotoxy(Cx, Cy);           // move the cursor to the top left corner
  }
}

// functionality for the left-arrow key press
void upArrowKeyFunction() {
  if (Cx == 1) {  // move the window up by one postion
    if (currStartIndx > 0) {
      currStartIndx--;
      currEndIndx--;
    }
  } else {
    // move the cursor up by one position
    Cx--;
  }
  editorRefreshScreen();    // clear the screen & move the cursor to top left
                            // corner
  printK_DirectoryFiles();  //
  gotoxy(Cx, Cy);
}

// functionality for the down-arrow key press
void downArrowKeyFunction() {
  if (Cx == k) {  // if cursor is at window end
    // slide the window down by 1 row
    if (currEndIndx <= k - 1) {
      currStartIndx++;
      currEndIndx++;
    }

  } else {
    // slide the cursor down by 1 row
    Cx++;
  }
  editorRefreshScreen();
  printK_DirectoryFiles();
  gotoxy(Cx, Cy);
}

// functionality for the left-arrow key press
void leftArrowKeyFunction(string currentDirectoryPathString) {
  // if the leftStack conatins more than 1 element
  // we are checking this because my leftStack always contain the current
  // absolute path so it will never be empty & when it contains single element
  // then it means I need to go to the current directory only(i.e. no need to
  // update the path).

  if (!leftStack.empty()) {
    // update the cursor to the top-left corner
    Cx = 1;
    Cy = 1;

    // remove the top element from the top of the leftStack & push it onto the
    // rightStack
    rightStack.push(currentDirectoryPathString);
    currentDirectoryPathString = leftStack.top();
    leftStack.pop();
    /*  string leftStackTop = leftStack.top();
     if(!leftStack.empty()){
     leftStack.pop();
     rightStack.push(leftStackTop); */
    // update the currentDirectoryPathString with the leftDirectoryStackTop

    // populate the vector with the updated path
    listDirectoryFiles(currentDirectoryPathString);

    // refresh screen
    editorRefreshScreen();

    // print the directory contents onto the screen
    printK_DirectoryFiles();

    gotoxy(Cx, Cy);
  }
}

// functionality for the right-arrow key press
void rightArrowKeyFunction(string currentDirectoryPathString) {
  // if the rightStack is not empty
  cout << "adfd"
       << " " << rightStack.size() << endl;
  if (!rightStack.empty()) {
    // update the cursor to the top-left corner
    Cx = 1;
    Cy = 1;
    cout << "hl" << endl;
    // remove the top element from the top of the rightStack & push it onto the
    // leftStack
    leftStack.push(currentDirectoryPathString);
    currentDirectoryPathString = rightStack.top();
    rightStack.pop();

    // populate the vector with the updated path
    listDirectoryFiles(currentDirectoryPathString);

    // clear the screen & move to the top-left corner
    editorRefreshScreen();
    // print the directory contents onto the screen
    printK_DirectoryFiles();
    // move cursor to the top-left corner
    gotoxy(Cx, Cy);
  }
}

// functionality for the 'h' key press
void homeKeyFunction(string currentDirectoryPathString) {
  // update the cursor to the top-left corner
  Cx = 1;
  Cy = 1;

  // currentDirectoryPathString = pathConvertToString(HOME);
  //  clear the screen & move to the top-left corner
  editorRefreshScreen();
  // populate the vector with the new directory elements
  // enter the home path & update the global path
  string path = pathConvertToString(HOME);
  if (!(currentDirectoryPathString == path)) {
    leftStack.push(currentDirectoryPathString);
    clearRightStack();
    currentDirectoryPathString = pathConvertToString(HOME);
    listDirectoryFiles(
        currentDirectoryPathString);  // populate the vector with new values
    printK_DirectoryFiles();
    // move the cursor back to the top-left corner of the screen
    gotoxy(Cx, Cy);
  }
}

///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///////////            COMMAND - MODE FUNCTIONS
/////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

// converts input command into tokens, converts to absolute path & pushes back
// into the vector
void tokenizeString(string commandModeInputString) {
  // vector[0] will contain the command type
  // vector[n-1] will contain the destination address
  // remaining locations will contain the source addresses

  string path;  // temporary path variable

  std::istringstream ss(commandModeInputString);
  std::string token;

  while (std::getline(ss, token, '|')) {
    commandModeInputVector.push_back(token);  // push the tokens to the vector
  }

  // check for validity of commands and give absolute paths
  for (int i = 1; i <= commandModeInputVector.size() - 1; i++) {
    if (commandModeInputVector[i].front() == '~') {  // token[0].front() == ~
      // replace ~ with /home/getlogin()[gives username].front()
      // check for validity of the path using opendir(close the pointer if valid
      // else throw error msg & break)
      path = giveRealPath(pathConvertToString(HOME));
      commandModeInputVector[i] = path;
      /* if (opendir(path.c_str()) == NULL) {
        cout << "Invalid Path!" << endl;
        return !isValidCommand;
      }; */

    } else if (commandModeInputVector[i].front() ==
               '/') {  // token[0].front() == /
      // pass the given path to realpath function
      // check for validity of the path using opendir(close the pointer if valid
      // else throw error msg & break)
      path = giveRealPath(commandModeInputVector[i]);
      commandModeInputVector[i] = path;
      /* if (opendir(path.c_str()) == NULL) {
        cout << "Invalid Path!" << endl;
        return !isValidCommand;
      }; */
    } else if (commandModeInputVector[i].front() ==
               '.') {  // token[0].front() == .
      // then append the current path to given path and send it to real path
      // function check for validity of the path using opendir(close the pointer
      // if valid else throw error msg & break)
      path = currentDirectoryPathString + "/" + commandModeInputVector[i];
      path = giveRealPath(path);
      commandModeInputVector[i] = path;
      /* if (opendir(path.c_str()) == NULL) {
        cout << "Invalid Path!" << endl;
        return !isValidCommand;
      }; */
    } /* else if (commandModeInputVector[i].front() == '..') {  // token[0] ==
    ..
      // then append the current path to given path and send it to real path
      // function check for validity of the path using opendir(close the pointer
      // if valid else throw error msg & break)
      path = currentDirectoryPathString + "/" + commandModeInputVector[i];
      path = giveRealPath(path);
      if (opendir(path.c_str()) == NULL) {
        cout << "Invalid Path!" << endl;
        return !isValidCommand;
      };

    } */
    else {  // when only filename is given i.e. token[0] = alphabet
            // then append the current path to given path and send it to real
            // path function
      // then whatever there is in the filename append it to the current path
      // check for validity of the path using opendir(close the pointer if valid
      // else throw error msg & break)
      path = currentDirectoryPathString + "/" + commandModeInputVector[i];
      path = giveRealPath(path);
      commandModeInputVector[i] = path;
      /* if (opendir(path.c_str()) == NULL) {
        cout << "Invalid Path!" << endl;
        !isValidCommand;
        return false;
      }; */
    }
  }
}

// checks whether the given path belongs to a directory
// returns true if it belongs to directory else it returns false(i.e. belongs to
// file)
bool checkDir(string path) {
  // pass the absolute path
  bool flag = false;
  struct stat stat_obj;
  if ((stat(path.c_str(), &stat_obj)) == -1) {  // failed to create stat object
    cout << "Invalid name:" << endl;
    return flag;  // returns false
  } else {
    if ((S_ISDIR(stat_obj.st_mode))) {  // check whether it is a valid directory
      return !(flag);                   // return true
    } else {
      return flag;  // return false
    }
  }
  return flag;  // return false
}

// copy command
bool copyFileCommand() {
  // first convert the string into tokens
  // tokenizeString(commandModeInputString);
  int size = commandModeInputVector.size();
  string destinationPath = commandModeInputVector[size - 1];
  if (!checkDir(destinationPath)) {  // if destination is not a directory
    return false;
  }
  int noOfSourceFiles =
      commandModeInputVector.size() - 2;  // no of source files

  // keep copying the files iteratively
  for (int i = 0; i < noOfSourceFiles; i++) {
    string sourcePath = commandModeInputVector[i + 1];
    string fileName = sourcePath.substr((sourcePath.find_last_of('/')) + 1);
    // x.substr(x.find(":") + 1);
    destinationPath = destinationPath + "/" + fileName;
    char writeBuffer[1024];
    struct stat source;
    struct stat destination;
    int readRetVal;
    int sourceOpenRetVal = open(sourcePath.c_str(), O_RDONLY);
    // int destinationOpenRetVal = open(destinationPath.c_str(), O_WRONLY |
    // O_CREAT, S_IRUSR | S_IWUSR);
    int destinationOpenRetVal = open(
        destinationPath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, source.st_mode);

    if ((stat(destinationPath.c_str(), &destination)) == -1) {
      cout << "destination file error!" << endl;
      return false;
    }

    if ((stat(sourcePath.c_str(), &source)) == -1) {
      cout << "source file error!" << endl;
      return false;
    }

    int bufferSize = sizeof(writeBuffer);
    while ((readRetVal = read(sourceOpenRetVal, writeBuffer, bufferSize)) > 0) {
      write(destinationOpenRetVal, writeBuffer, readRetVal);
    }
    chown(destinationPath.c_str(), source.st_uid, source.st_gid);
    chmod(destinationPath.c_str(), source.st_mode);

    // clear the writeBuffer at the end of the loop
    memset(writeBuffer, 0, bufferSize);
  }
  return true;  // copy is successful
}

// copy directory command
bool copyDirectoryCommand() {
  // first convert the string into tokens
  // tokenizeString(commandModeInputString);
  int size = commandModeInputVector.size();
  // check destination path
  string destinationPath = commandModeInputVector[size - 1];
  if (!checkDir(destinationPath)) {  // if destination is not a directory
    return false;
  }
  int noOfSourceDirectories =
      commandModeInputVector.size() - 2;  // no of source files

  // keep copying the directories iteratively
  for (int i = 0; i < noOfSourceDirectories; i++) {
    string sourcePath = commandModeInputVector[i + 1];

    DIR *dir = opendir(sourcePath.c_str());
    struct dirent *entity;
    int mkdrirRetVal;
    mkdrirRetVal =
        mkdir(destinationPath.c_str(), 0777);  // giving all permissions
    // dir = opendir(sourcePath.c_str());
    if (dir == NULL) {
      cout << "Cannot open the directory!\n";
      return false;  // failure
    }
    entity = readdir(dir);    // start reading the directory's content
    while (entity != NULL) {  // while there are elements in the directory

      // skip the current path as well as parent path
      if (!(entity->d_name == ".") && !(entity->d_name == "..")) {
        // update the destination path for current element
        string destinationPathCurrentElement =
            destinationPath + "/" + entity->d_name;

        // update the source path for current element
        string pathCurrentElement = sourcePath + "/" + entity->d_name;

        struct stat statObj;
        if (stat(pathCurrentElement.c_str(), &statObj) == -1) {
          cout << "Error!\n";
          return false;
        }
        if (!(S_ISDIR(statObj.st_mode))) {  // if current element is file
          copyFileCommand();  // call copy file function containing arguments
        } else {
          copyDirectoryCommand();  // call copy directory function containing
                                   // arguments
        }
      }
    }
  }
}

int flag = 0;
// used to rename both file & directory
bool renameCommand() {
  if (flag) {  // to be used while moving directory: flag=0 means we are moving
               // directory
    rename(commandModeInputVector[1].c_str(),
           commandModeInputVector[2].c_str());
    return true;
  }
  // tokenizeString(commandModeInputString);
  //  rename command must have exactly 3 tokens
  if (!(commandModeInputVector.size() == 3)) {
    cout << "Invalid command!" << endl;
    return false;
  }

  string oldFilename = commandModeInputVector[1];
  if (!flag) {
    if (!(checkDir(
            oldFilename))) {  // check whether oldFilename is a valid Directory
      return false;
    }
  }

  string newFilename = commandModeInputVector[2];

  int retVal = rename(oldFilename.c_str(), newFilename.c_str());
  if (retVal != 0) {
    return false;  // when rename function fails
  }
  return true;
}

// used to move both file & directory
bool moveCommand() {
  // tokenizeString(commandModeInputString);
  string folderName = commandModeInputVector[1].substr(
      (commandModeInputVector[1].find_last_of('/')) + 1);
  commandModeInputVector[2] = commandModeInputVector[2] + "/" + folderName;
  flag = 1;  // to reuse rename command
  bool retVal = renameCommand();
  if (!retVal) {
    return false;
  }
  return true;
}

// create_file command
bool createFileCommand() {
  // create tokens
  // tokenizeString(commandModeInputString);

  int size = commandModeInputVector.size();

  if (size == 2) {  // when there is no destination given we have to create fil
                    // in the current directory

    /* string fileName =
            destinationPath.substr((destinationPath.find_last_of('/')) + 1); */
    string destinationPath =
        currentDirectoryPathString + "/" + commandModeInputVector[1];
    int createRetVal = creat(destinationPath.c_str(), 0777);
    if (createRetVal == -1) {
      cout << "Cannot create file!\n";
      return false;
    }
  } else {  // when we have multiple files to be created
    for (int i = 1; i < size - 1; i++) {
      string destinationPath = commandModeInputVector[size - 1];
      /* string fileName =
          destinationPath.substr((destinationPath.find_last_of('/')) + 1);
*/  // destinationPath = destinationPath + "/" + commandModeInputVector[i];
      int createRetVal = creat(destinationPath.c_str(), 777);
      if (createRetVal > -1) {
        cout << "Cannot create file!\n";
        return false;
      }
    }
  }
  return false;
}

// create_dir command
bool createDirectoryCommand() {
  // tokenizeString(commandModeInputString);
  int vectorSize = commandModeInputVector.size();
  if (vectorSize == 2) {
    string destination = commandModeInputVector[vectorSize - 1];
    int retVal = mkdir(destination.c_str(), 0777);
    if (retVal == -1) {
      cout << "Cannot create directory!\n";
      return false;
    }
  } else {
    for (int i = 1; i < vectorSize - 1; i++) {
      string destination = commandModeInputVector[i];
      int retVal = mkdir(destination.c_str(), 0777);
      if (retVal == -1) {
        cout << "Cannot create directory!\n";
        return false;
      }
    }
  }
  return true;
}

// delete_file command
bool removeFileCommand(string path) {
  // tokenize the input string & converts to valid absolute paths
  // tokenizeString(commandModeInputString);

  int rmvRetVal = remove(path.c_str());
  if (rmvRetVal != 0) {
    cout << "Cannot delete file!\n";
    return false;
  }
  return true;
}

// delete_dir command
bool removeDirectoryCommand(string destinationPath) {
  /* // first convert the string into tokens & valid absolute paths
  tokenizeString(commandModeInputString);
  int size = commandModeInputVector.size();

   if(commandModeInputVector.size()!=2){
      cout << "Invalid command!\n";
      return false;
    }  */
  // check destination path

  if (!checkDir(destinationPath)) {  // if destination is not a directory
    cout << "The location you entered is not a directory!\n";
    return false;
  }

  DIR *dir = opendir(destinationPath.c_str());
  struct dirent *entity;

  if (dir == NULL) {
    cout << "Cannot open the directory!\n";
    return false;  // failure
  }
  entity = readdir(dir);    // start reading the directory's content
  while (entity != NULL) {  // while there are elements in the directory

    // skip the current path as well as parent path
    string var = pathConvertToString(entity->d_name);
    if (!(var == ".") && !(var == "..")) {
      /* //update the destination path for current element
      string destinationPathCurrentElement = destinationPath + "/" +
entity->d_name;

      //update the source path for current element
      string pathCurrentElement = sourcePath + "/" + entity->d_name; */
      string tempPath = destinationPath + "/" + var;
      struct stat statObj;
      if (stat(tempPath.c_str(), &statObj) == -1) {
        cout << "Error!\n";
        return false;
      }
      if (!(S_ISDIR(statObj.st_mode))) {  // if current element is file
        removeFileCommand(tempPath);      //

      } else {  // if the current element is folder
        removeDirectoryCommand(tempPath);
      }
    }
  }
  closedir(dir);
  int rmdirRetVal =
      rmdir(destinationPath.c_str());  // remove the empty directory i.r.
                                       // recursion base case directory
  if (rmdirRetVal == -1) {
    cout << "Error occured while deleting the directory[BASE CASE EMPTY "
            "DIRECTORY]\n";
    return false;
  }

  return true;
}

// goto command
bool gotoCommand() {
  // tokenizeString(commandModeInputString);
  if (commandModeInputVector[0] != "gotoCommand") {
    cout << "Not a goto command!" << endl;
    return false;
  }
  if (commandModeInputVector.size() != 2) {
    cout << "Invalid command!" << endl;
    return false;
  }
  string path = commandModeInputVector[1];

  struct stat entity;
  if (stat(path.c_str(), &entity)) {
    cout << "Invalid destination" << endl;
    return false;
  }
  if (!S_ISDIR(entity.st_mode)) {
    cout << "Invalid destination";
    return false;
  }

  leftStack.push(currentDirectoryPathString);
  currentDirectoryPathString = path;
  editorRefreshScreen();
  listDirectoryFiles(currentDirectoryPathString);
  printK_DirectoryFiles();
  gotoxy(1, 1);

  return true;
}

// search command
bool searchCommand(string fileName, string path) {
  DIR *dir = opendir(path.c_str());
  struct dirent *entity;

  if (dir == NULL) {
    cout << "Cannot open the directory!\n";
    return false;  // failure
  }
  entity = readdir(dir);    // start reading the directory's content
  while (entity != NULL) {  // while there are elements in the directory

    string var = pathConvertToString(entity->d_name);
    if (!(var == ".") && !(var == "..")) {
      if (var == fileName) {
        return true;
      }

      string tempString = path + "/" + var;
      
      if (checkDir(tempString)) {
        if (searchCommand(fileName, tempString)) {
          return true;
        }
      }
    }
   entity = readdir(dir);
  }
   return false;
}
// functionality of COMMAND MODE
int commandMode() {
  gotoxy(k + 10, 1);
  cout << "Enter your Command: " << endl << "$ ";
  //commandModeInputString.clear();
  while (1) {
    char inputCommandCharacter;
    cin.get(inputCommandCharacter);

    if (inputCommandCharacter ==
        27) {  // ESCAPE character will go back to normal mode

      // before exiting clear the input command string
      commandModeInputString.clear();
      return -1;
    } else if (inputCommandCharacter == '\n') {
      // first clear the tokenized vector
      commandModeInputVector.clear();
      // first convert the input command into tokens & push the absolute real
      // paths to the vector
      string fileName = commandModeInputString.substr(
          (commandModeInputString.find_last_of('|')) + 1);
      tokenizeString(commandModeInputString);
      // handle command

      if (commandModeInputVector[0] == "copy") {  // COPY FILE
        copyFileCommand();
      } else if (commandModeInputVector[0] == "copy_dir") {  // COPY DIRECTORY
        copyDirectoryCommand();
      } else if (commandModeInputVector[0] == "move") {  // FILE
        moveCommand();
      } else if (commandModeInputVector[0] == "rename") {  // RENAME
        renameCommand();
      } else if (commandModeInputVector[0] == "create_file") {  // CREATE FILE

        createFileCommand();
      } else if (commandModeInputVector[0] ==
                 "create_dir") {  // CREATE DIRECTORY
        createDirectoryCommand();
      } else if (commandModeInputVector[0] == "delete_file") {  // DELETE FILE
        string path = commandModeInputVector[1];
        removeFileCommand(path);
      } else if (commandModeInputVector[0] ==
                 "delete_dir") {  // DELETE DIRECTORY
        string path = commandModeInputVector[1];
        removeDirectoryCommand(path);
      } else if (commandModeInputVector[0] == "goto") {  // GOTO
        gotoCommand();
      } else if (commandModeInputVector[0] == "search") {  // SEARCH
          cout << endl;
        if(searchCommand(fileName, currentDirectoryPathString)){
          cout<<endl;
          cout << true << endl;
        }
        else{
          cout << false<<endl;
        }
      } else {
        cout << "Invalid command input" << endl;
        return -1;
      }
        commandModeInputString.clear();
        gotoxy(k + 11, 3);
    } else if (inputCommandCharacter == 127) {  // BACKSPACE character so remove
                                                // one chracater and print again
      if (!commandModeInputString.empty()) {
        cout << '\r' << "$ " << string(commandModeInputString.length(), ' ');
        commandModeInputString.pop_back();
        cout << '\r' << "$ " << commandModeInputString;
      }
    } else {  // normal case so keep pushing & reprinting
      commandModeInputString.push_back(inputCommandCharacter);

      cout << commandModeInputString.back();
    }
  }
}

// functionality for NORMAL MODE
void normalMode(string currentFilePath) {  // we will pass the current
                                           // directory/file path
  // editorRefreshScreen();
  enableRawMode();

  // struct ordinates co_ordinates;  // co-ordinates object will be used to
  // store
  //  the location for cursor
  Cx = 1;               // initialise the x-coordinate to 1
  Cy = 1;               // initialise the y-coordinate to 1
  currStartIndx = 0;    // starting index of current window
  currEndIndx = k - 1;  // ending index of current window

  listDirectoryFiles(
      currentFilePath);  // store the directory files into the vector

  editorRefreshScreen();
  printK_DirectoryFiles();  // initially display the first k entries
  gotoxy(Cx,
         Cy);  // cursor will move to the co-ordinate <Cx,Cy>(<1,1>)
               // [initially it will move to top-most left corner]
  char ipKeyPress;
  while (1) {
    cin.get(ipKeyPress);  // input keypress will be stored in ipKeyPress
    switch (ipKeyPress) {
      case 'B':  // down-arrow key press

        downArrowKeyFunction();
        break;

      case 'A':  // up-arrow key press

        upArrowKeyFunction();
        break;
      case 10:  // enter key press

        enterKeyFunction();  // passing current x_co-ordinate to get the
                             // current element of the vector

        break;

      case 127:  // backspace key press
                 // currentDirectoryPath = get_current_dir_name();
        backspaceKeyFunction(currentDirectoryPathString);
        break;

      case 'D':  // left-arrow key press
                 // currentDirectoryPath = get_current_dir_name();
        leftArrowKeyFunction(currentDirectoryPathString);
        break;

      case 'C':  // right-arrow key press
                 // currentDirectoryPath = get_current_dir_name();
        rightArrowKeyFunction(currentDirectoryPathString);
        break;

      case 'h':  // h[HOME] key press
        // currentDirectoryPath = get_current_dir_name();
        /* gotoxy(k+10, 1);
        cout << "I am in h" << endl; */

        homeKeyFunction(currentDirectoryPathString);
        break;

      case ':':  // gotoxy(k+5,1);
        // cout << "In case-command mode" << endl;
        // getchar();
        commandMode();
        // now we have returned back from Command mode
        // now clear screen from the row above the command display till the end
        // row of the screen
        //  now move the cursor back to top-left corner
        editorRefreshScreen();
        printK_DirectoryFiles();  // initially display the first k entries
        gotoxy(Cx,
               Cy);  // cursor will move to the co-ordinate <Cx,Cy>(<1,1>)
                     // [initially it will move to top-most left corner]

        break;

      default:

        break;
    }
  }

  cout << flush;
}

int main() {
  // initially push the current directory path onto the leftStack
  // leftStack.push(currentDirectoryPathString);
  // currentDirectoryPathString = "/home/aadesh";

  normalMode(currentDirectoryPathString);
  // cout << "\r" ;
  return 0;
}
