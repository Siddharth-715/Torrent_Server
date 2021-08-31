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

int tfd;

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

string split(string str) // TO SPLIT THE USER DETAILS(USERNAME)
                         // {UNLIKE SERVER, PASWWORD NOT EXTRACTED AS IT MAY COMPROMISE SECURITY}
{
    for (int i = 0; i < 15; i++)
    {
        if (str[i] == ':')
            return str.substr(0, i);
    }
    str = "WRONG INPUT";
    return str;
}

int rgstr(int sock) //REGISTRATION OF NEW USER
{
    char details[30];
    cout << "\nENTER DETAILS-> USERNAME:PASSWORD\n";
    fgets(details, 25, stdin);
    write(sock, details, 30);
    string input = split(string(details));
    bzero(details, 30);
    read(sock, details, 2);
    if ((string)details == "0")
        cout << "USERNAME ALREADY EXITST!!";
    else
    {
        cout << "WELCOME " << input << endl;
        write(sock, "NEW USER REGISTERED!\n", 30);
    }
    return 0;
}

int login(int sock) //LOGIN OF  USER
{
    char details[50];
    cout << "\nENTER DETAILS-> USERNAME:PASSWORD\n";
    fgets(details, 30, stdin);
    write(sock, details, 30);
    memset(details, 0, 30);
    read(sock, details, 30);
    if (details[0] == '0')
        cout << "WRONG CREDENTIALS!\n";
    else
        cout << "\nWELCOME AGAIN  " << details << endl;

    return 0;
}


int tracker() //MAIN()
{
    int  portno, n, p;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    int i = 1;
    char buffer[256];
    char req[] = "exit";
    portno = 9000;
    tfd = socket(AF_INET, SOCK_STREAM, 0);
    if (tfd < 0)
        error("ERROR opening socket");
    server = gethostbyname("siddharth-IdeaPad-5");
    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy((char *)&serv_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(tfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

   
    // while (true)
    // {
    label:
        printf("TRACKER<> ");
        bzero(buffer, 256);
        fgets(buffer, 250, stdin); //MESSAGE/QUERY FOR SERVER
        p = 0;

        for (int i = 0; i < 4; i++) //FOR EXITING SERVER
        {
            if (buffer[i] == req[i])
                p = p + 1;
            else
                p = 1;
        }
        if (p == 4)
        {
            std::cout << "EXITING...." << std::endl;
            close(tfd);
            return 0;
        }

        if (buffer[0] == '1') //REGISTRATION CALL
        {
            n = write(tfd, buffer, strlen(buffer));
            rgstr(tfd);
            goto label;
        }

        if (buffer[0] == '2') //LOGIN CALL
        {
            n = write(tfd, buffer, strlen(buffer));
            login(tfd);
            goto label;
        }

        n = write(tfd, buffer, 255); //MESSAGE SENT TO SERVER
        if (n < 0)
            return 0;

        if (buffer[0] == '3') //ONLINE STATUS
        {
            cout << "******ONLINE-PEERS******" << endl;
            while (true)
            {
                bzero(buffer, 256);
                n = read(tfd, buffer, 30);
                if (buffer[0] == '|')
                    break;
                printf("%s", buffer);
                cout << std::endl;
            }
            goto label;
        }

        bzero(buffer, 256);
        n = read(tfd, buffer, 200);
        if (n < 0)
            error("ERROR reading from socket");
        printf("%s", buffer);
        cout << std::endl;
    //}

    return 0;
}

int main()
{
    cout << "*****WELCOME TO SERVER*****\n";
    cout << "<>ENTER 1 -> CREATE NEW ACCOUNT\n";
    cout << "<>ENTER 2 -> LOGIN IN SERVER\n";
    cout << "<>ENTER 3 -> SEE ONLINE PEERS\n";
    cout << "<>TO EXIT -> ENTER 'exit'\n";
    tracker();
}