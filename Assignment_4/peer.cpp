/********************************PEER*************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fstream>
#include <iostream>
#include <math.h>
#include <map>
#include "sha256.h"
using namespace std;

int tfd;
string myname = "#", pname = "#";
int myport = 0, pport = 0;
map<string, string> db; // DATABSE MAP: FOR LOADING DATA FROM SAVED FILE
int peerc(void);

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

string split2(string str) // TO SPLIT THE USER DETAILS(PASSWORD)
{
    for (int i = 0; i < 15; i++)
    {
        if (str[i] == ':')
            return str.substr(i + 1, (str.length()) - 1);
    }
    str = "WRONG INPUT";
    return str;
}

void loader() // LOADS THE DATA FROM FILE TO DATABASE MAP ON EVERY RUN
{
    fstream file;
    file.open("data.txt", ios::in);
    if (file.is_open())
    {
        string str, usr, pswd;
        while (getline(file, str))
        {
            usr = split(str);
            pswd = split2(str);
            db[usr] = pswd;
        }
        file.close();
    }
}

struct arg
{
    int sock;
    string peer;
};

void *dostuff(void *cli_info) // MESSAGE MANAGER AND FUNCTION CALLS
{
    int n;
    char buffer[256];
    long csock = (long)((struct arg *)cli_info)->sock;
    string cli_name = ((struct arg *)cli_info)->peer;
    bzero(buffer, 256);
    read(csock, buffer, 10);
    pname = (string)buffer;
    pport = atoi(db[pname].c_str());
    cout << "CONNECTED TO " << pname << endl;

    while (true)
    {
        bzero(buffer, 256);
        n = read(csock, buffer, 255);
        if (n <= 0)
        {
            error("Client Disconnecting...");
            break;
        }
        cout << pname << "<> " << buffer ;
        n = write(csock, "<>recived<>", 18);
        if (n < 0)
            error("ERROR writing to socket");
    }

    close(csock); //CLOSING THREAD FOR INDIVIDUAL THREAD
    pthread_exit(NULL);
}

int peers()
{
    int sockfd, newsockfd, portno, pid, opt;
    socklen_t clilen;
    pthread_t threads[5];
    struct arg client;
    loader();
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(myport);
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        system("script.sh");
        error("ERROR on binding");
    }
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    for (int i = 0; i < 5; i++)
    {

        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0)
            error("ERROR on accept");
        client.sock = newsockfd;
        client.peer = myname;
        cout << "SEEMS LIKE SOMEONE CONNECTED TO YOU! " <<  endl;
        pthread_create(&threads[i], NULL, dostuff, (void *)&client); //THREADING
        
    }

    for (int j = 0; j < 5; j++)
        pthread_join(threads[j], NULL); // THREAD JOINING

    close(newsockfd); //CLOSING SOCKET FOR MAIN THREAD ONLY
    close(sockfd);
    
    return 0;
}

int rgstr(int sock) //REGISTRATION OF NEW USER
{
    char details[30];
    cout << "\nENTER DETAILS-> USERNAME:PASSWORD\n";
    fgets(details, 25, stdin);
    write(sock, details, 30);
    string input = split((string)details);
    myport = atoi(split2((string)details).c_str());
    bzero(details, 30);
    read(sock, details, 2);
    if ((string)details == "0")
        cout << "USERNAME ALREADY EXITST!!";
    else
    {
        myname = input;
        myport = atoi(db[myname].c_str());
        peers();
        cout << "WELCOME " << input << endl;
        write(sock, "NEW USER REGISTERED!\n", 30);
    }
    return 0;
}

int login(int sock) //LOGIN OF  USER
{
    char details[50];
    cout << "ENTER DETAILS-> USERNAME:PASSWORD\n";
    fgets(details, 30, stdin);
    myport = atoi(split2(string(details)).c_str());
    write(sock, details, 30);
    memset(details, 0, 30);
    read(sock, details, 30);
    if (details[0] == '0')
    {
        cout << "WRONG CREDENTIALS!\n";
        return 0;
    }
    else
        cout << "WELCOME AGAIN  " << details;
    myname = (string)details;
    myport = atoi(db[myname].c_str());
    int pid = fork();
    if(pid == 0)
        peers();
        
    return 0;
}

int upload(int csock)
{
    ifstream file;
    string str, temp, fname, sha;
    cout << "ENTER NAME OF THE FILE YOU WISH TO UPLOAD: \n";
    cin >> fname;
    file.open(fname, ios::ate);
    float size = file.tellg();
    float bnum = ceil(size / 10240);
    char bbuff[10240];
    for (int i = 0; i < size; i++)
    {
        
    }
        fclose(file);
}

int peerc()
{
    int sockfd, n, p;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];
    char req[] = "exit";
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
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
    serv_addr.sin_port = htons(pport);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");
    write(sockfd, myname.c_str(), 10);

    while (true)
    {
    label:
        cout << pname << "<> ";
        bzero(buffer, 256);
        fgets(buffer, 250, stdin); //MESSAGE/QUERY FOR SERVER
        p = 0;
        string msg = (string)buffer;
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
            close(sockfd);
            return 0;
        }
        n = write(sockfd, buffer, 255); //MESSAGE SENT TO SERVER
        if (n < 0)
            return 0;

        if (msg.compare("online") == 0) //ONLINE STATUS
        {
            cout << "******ONLINE-PEERS******" << endl;
            write(tfd, "3", 2);
            while (true)
            {
                bzero(buffer, 256);
                n = read(sockfd, buffer, 30);
                if (buffer[0] == '|')
                    break;
                printf("%s", buffer);
                cout << std::endl;
            }
            goto label;
        }

        bzero(buffer, 256);
        n = read(sockfd, buffer, 200);
        if (n < 0)
            error("ERROR reading from socket");
        printf("%s", buffer);
        cout << std::endl;
    }

    return 0;
}

int tracker() //TO CONNECT PEER WITH TRACKER
{
    int portno, n, p;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];
    char req[] = "exit";
    string info;
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

label:
    printf("TRACKER<> ");
    bzero(buffer, 256);
    fgets(buffer, 250, stdin); //MESSAGE/QUERY FOR SERVER
    p = 0;
    info = (string)buffer;

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

    if (myname.compare("#") != 0 && myport != 0)
    {
        if (split(info).compare("connect") == 0)
        {
            pname = split2(info).substr(0, (split2(info).length()) - 1);
            pport = atoi(db[pname].c_str());
            cout << "CONNECTING TO " << pname << endl;
            peerc();
            goto label;
        }

        if (buffer[0] == '4') //USER WANTS TO UPLOAD A FILE
        {
            n = write(tfd, buffer, strlen(buffer));
            upload(tfd);
            goto label;
        }

        bzero(buffer, 256);
        n = read(tfd, buffer, 200);
        if (n < 0)
            error("ERROR reading from socket");
        printf("%s", buffer);
        cout << std::endl;
        goto label;
    }
    else
    {
        cout << "YOU ARE NOT LOGGED IN" << endl;
        goto label;
    }
    return 0;
}

int main()
{
    loader();
    cout << "*****WELCOME TO SERVER*****\n";
    cout << "<>ENTER 1 -> CREATE NEW ACCOUNT\n";
    cout << "<>ENTER 2 -> LOGIN IN SERVER\n";
    cout << "<>ENTER 3 -> SEE ONLINE PEERS\n";
    cout << "<>ENTER 4 -> UPLOAD FILE/n";
    cout << "<>TO EXIT -> ENTER 'exit'\n";
    tracker();
    shutdown(tfd, 2);
    return 0;
}