/********************************PEER*************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fstream>
#include <iostream>
using namespace std;

int main()
{
    cout << "*****WELCOME TO SERVER*****\n";
    cout << "<>ENTER 1 -> CREATE NEW ACCOUNT\n";
    cout << "<>ENTER 2 -> LOGIN IN SERVER\n";
    cout << "<>ENTER 3 -> SEE ONLINE PEERS\n";
    cout << "<>TO EXIT -> ENTER 'exit'\n";
}