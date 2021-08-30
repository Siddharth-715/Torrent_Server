/********************************TRACKER*************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <map>
using namespace std;

int k = 0; //CLIENT COUNTER {BROKEN}

map<string, string> db; // DATABSE MAP: FOR LOADING DATA FROM SAVED FILE
map<string, string> online;

void error(const char *msg)
{
    perror(msg);
}

string split(string str) // TO SPLIT THE USER DETAILS(USERNAME)
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
            online[usr] = "offline";
        }
        file.close();
    }
}

int rgstr(int sock, string &name) //REGISTRATION OF NEW USER
{
    FILE *file = fopen("data.txt", "a");
    char details[30];
    cout << "REGISTERING NEW USER\n";
    read(sock, details, 30);
    string str = (string)details;
    map<string, string>::iterator it;
    for (it = db.begin(); it != db.end(); it++)
    {
        if (it->first.compare(split(str)) == 0)
        {
            cout << "REGISTRATION FAILED" << endl;
            fclose(file);
            write(sock, "0", 2);
            return 0;
        }
    }
    write(sock, "1", 2);
    name = split(str);
    online[name] = "online";
    cout << "DETAILS OF NEW USER : ";
    cout << details;
    if (file)
    {
        fputs(details, file);
    }
    fclose(file);
    return 1;
}

int login(int sock, string &name) //LOGIN OF EXISTING USER
{
    char details[50];
    memset(details, 0, 30);
    string str;
    cout << "A USER IS LOGGING IN....\n";
    read(sock, details, 30);
    str = (string)details;
    str = str.substr(0, str.length() - 1); //HAVE TO CHANGE LENGTH AS READ() FUCKS UP THE LENGTH (-_-)!
    map<string, string>::iterator it;
    for (it = db.begin(); it != db.end(); it++)
    {
        if ((it->first.compare(split(str)) == 0) && (it->second.compare(split2(str)) == 0))
        {
            str.clear();
            str = it->first;
            write(sock, str.c_str(), 30);
            cout << str << " LOGGED IN!\n";
            name = str;
            online[str] = "online";
            return 1;
        }
    }
    write(sock, "0", 2);
    cout << "LOGIN FAILED!!!\n";
    return 0;
}

struct arg
{
    int sock;
    string port;
};

void *dostuff(void *cli_info) // MESSAGE MANAGER AND FUNCTION CALLS
{
    int n, i=1;
    char buffer[256];
    long csock = (long)((struct arg *)cli_info)->sock;
    string cli_port = ((struct arg *)cli_info)->port;
    int status = 0;
    string name = "USER";
    cout << "A USER CONNECTED VIA PORT: " << cli_port << endl;

    while (true)
    {
        label: 
        bzero(buffer, 256);
        n = read(csock, buffer, 255);
        if (n <= 0)
        {
            std::cout << name << " IS DISCONNECTING..." << std::endl;
            online[name] = "offline";
            break;
        }

        if (buffer[0] == '1') //REGISTRATION CALL
        {
            status = rgstr(csock, name);
            goto label;
        }

        if (buffer[0] == '2') //LOGIN CALL
        {
            status = login(csock, name);
            goto label;
        }

        if (buffer[0] == '3') //ONLINE STATUSES
        {
            map<string, string>::iterator it;
            for (it = online.begin(); it != online.end(); it++)
            {
                if (it->second.compare("online") == 0)
                {
                    write(csock, (it->first).c_str(), 30);
                }
            }
            write(csock, "|", 2);
            goto label;
        }

        if (status == 1)
        {
            cout << name << "<> " << buffer << endl;
            n = write(csock, "<>recived<>", 18);
            if (n < 0)
                error("ERROR writing to socket");
        }

        if (status == 0)
        {
            write(csock, "YOU ARE NOT LOGGED IN!", 35);
            goto label;
        }
    }

    close(csock); //CLOSING THREAD FOR INDIVIDUAL THREAD
    pthread_exit(NULL);
}

int main() //MAIN()
{
    cout << "***TRACKER IS LISTENING ON PORT 9000***" << endl;
    int sockfd, newsockfd, portno, pid, opt;
    char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
    socklen_t clilen;
    pthread_t threads[5];
    struct arg client;
    opt = 1;
    loader();

    struct sockaddr_in serv_addr, cli_addr;
    int n;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = 9000;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    listen(sockfd, 5);
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    clilen = sizeof(cli_addr);

    for (int i = 0; i < 5; i++)
    {

        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0)
            error("ERROR on accept");
        client.sock = newsockfd;

        if (getnameinfo((struct sockaddr *)&cli_addr, clilen, hbuf, sizeof(hbuf), sbuf, sizeof(sbuf), 0) != 0)
            cout << "errrr" << endl; //TO GET PORT NO. OF CLIENT
        else
        {
            client.port = (string)sbuf;
        }

        pthread_create(&threads[i], NULL, dostuff, (void *)&client); //THREADING
    }

    for (int j = 0; j < 5; j++)
        pthread_join(threads[j], NULL); // THREAD JOINING

    close(sockfd); //CLOSING SOCKET FOR MAIN THREAD ONLY
    return 0;
}
