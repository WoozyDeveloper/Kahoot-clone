
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <queue>
#include <iostream>
#include <vector>
#include "sqlite3.h"

/* portul folosit */
#define PORT 2908

int place[100];

struct ClientsData
{
    char username[30] = "";
    int score;
};
int waiting_room = 1;
bool first_client = false;
std::vector<ClientsData> clients_data;
std::vector<int> index_clients;
int seconds;
// codul de eroare returnat de anumite apeluri
extern int errno;

struct thData
{
    char username[30] = "";
    int score = 0;
    int idThread; //id-ul thread-ului tinut in evidenta de acest program
    int cl;       //descriptorul intors de accept

    char questions[200][200];
    int answers[10];
};

std::vector<int> all_clients;
int client;
void *manage_time(void *);
static void *treat(void *); /* functia executata de fiecare thread ce realizeaza comunicarea cu clientii */
int raspunde(void *, unsigned int &, char[]);
char question[200];

int callback(void *NotUsed, int argc, char **argv, char **azColName)
{

    // int argc: holds the number of results
    // (array) azColName: holds each column returned
    // (array) argv: holds each value

    char result[200];
    strcpy(result, azColName[0]);

    return 0;

    // for (int i = 0; i < argc; i++)
    // {

    //     // Show column name, value, and newline
    //     std::cout << azColName[i] << ": " << argv[i] << std::endl;
    // }

    // // Insert a newline
    // std::cout << std::endl;

    // // Return successful
    // return 0;
}

int main()
{
    char *zErrMsg = 0;
    sqlite3 *db;
    std::string sql;

    // Save the result of opening the file
    int rc;

    // Save the result of opening the file
    rc = sqlite3_open("example.db", &db);

    if (rc)
    {
        // Show an error message
        std::cout << "DB Error: " << sqlite3_errmsg(db) << std::endl;
        // Close the connection
        sqlite3_close(db);
        // Return an error
        return (1);
    }

    sql = "CREATE TABLE QUESTION ("
          "ID INT PRIMARY KEY     NOT NULL,"
          "QUESTION           TEXT    NOT NULL);";

    rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);

    struct sockaddr_in server; // structura folosita de server
    struct sockaddr_in from;
    int nr; //mesajul primit de trimis la client
    int sd; //descriptorul de socket
    int pid;
    pthread_t th[100]; //Identificatorii thread-urilor care se vor crea
    int i = 0;

    /* crearea unui socket */
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("[server]Eroare la socket().\n");
        return errno;
    }
    /* utilizarea optiunii SO_REUSEADDR */
    int on = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    /* pregatirea structurilor de date */
    bzero(&server, sizeof(server));
    bzero(&from, sizeof(from));

    /* umplem structura folosita de server */
    /* stabilirea familiei de socket-uri */
    server.sin_family = AF_INET;
    /* acceptam orice adresa */
    server.sin_addr.s_addr = htonl(INADDR_ANY); //orice adresa valabila de pe masina pe care e rulat
    /* utilizam un port utilizator */
    server.sin_port = htons(PORT); //folosim portul pus

    /* atasam socketul */
    if (bind(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
        perror("[server]Eroare la bind().\n");
        return errno;
    }

    /* punem serverul sa asculte daca vin clienti sa se conecteze */
    if (listen(sd, 2) == -1)
    {
        perror("[server]Eroare la listen().\n");
        return errno;
    }
    pthread_t time_thread;
    pthread_create(&time_thread, NULL, manage_time, NULL);

    /* servim in mod concurent clientii...folosind thread-uri */
    while (1)
    {
        thData *td; //parametru functia executata de thread
        socklen_t length = sizeof(from);

        printf("[server]Asteptam la portul %d...\n", PORT);
        fflush(stdout);

        // client= malloc(sizeof(int));
        /* acceptam un client (stare blocanta pana la realizarea conexiunii) */
        if ((client = accept(sd, (struct sockaddr *)&from, &length)) < 0)
        {
            perror("[server]Eroare la accept().\n");
            continue;
        }

        /* s-a realizat conexiunea, se astepta mesajul */

        // int idThread; //id-ul threadului
        // int cl; //descriptorul intors de accept

        td = (thData *)malloc(sizeof(thData));
        td->idThread = i++;
        td->cl = client;
        //std::cout << "TD CL ::::" << td->cl << '\n';
        td->answers[0] = 6;
        strcpy(td->questions[i], "");
        for (int i = 1; i < 3; i++) //1,2
        {
            td->answers[i] = 0;
            for (int j = 0; j < 100; j++)
                td->questions[i][j] = ' ';
            if (i % 2 == 0)
                strcpy(td->questions[i], "Ce a fost Stefan cel Mare? A) Domnitor, B) Presedinte, C) Programator, D) Sef al organizatiei mondiale");
            else
                strcpy(td->questions[i], "Cine este Klaus Iohannis? A) Presedintele Romaniei, B) Un domnitor, C) Un programator, D) Un clarvazator");
        }
        for (int i = 3; i < 5; i++) //3,4
        {
            td->answers[i] = 1;
            for (int j = 0; j < 50; j++)
                td->questions[i][j] = ' ';
            if (i % 2 == 0)
                strcpy(td->questions[i], "Unde este Romania? A) Asia, B) Europa, C) Antarctica, D) America de N");
            else
                strcpy(td->questions[i], "Unde se afla Turnul Eiffel? A) Spania, B) Franta, C) Romania, D) Rusia");
        }
        for (int i = 5; i < 7; i++) //5,6
        {
            td->answers[i] = 2;
            for (int j = 0; j < 50; j++)
                td->questions[i][j] = ' ';
            if (i % 2 == 0)
                strcpy(td->questions[i], "Ce este Huawei? A) O personalitate, B) Un programator, C) O firma, D) O abreviere");
            else
                strcpy(td->questions[i], "Care este cel mai mare continent? A) Europa, B) America de N, C) Asia, D) Australia");
        }
        for (int i = 7; i <= 9; i++) //7,8,9
        {
            td->answers[i] = 3;
            for (int j = 0; j < 50; j++)
                td->questions[i][j] = ' ';
            if (i % 3 == 0)
                strcpy(td->questions[i], "Ce este Huawei? A) O personalitate, B) Un programator, C) O echipa de fotbal, D) O firma");
            else if (i % 3 == 1)
                strcpy(td->questions[i], "Ce este Europa? A) O companie, B) O organizatie, C) O tehnologie, D) Un continent");
            else
                strcpy(td->questions[i], "Ce este Intel? A) O firma de constructii, B) O pictura, C) Un nume de telefon, D) O firma de procesoare");
        }
        //1
        // sql = "INSERT INTO QUESTION ('ID', 'QUESTION') VALUES ('1','Apasa A');";
        // rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
        // strcpy(td->questions[1], question);
        // //2
        // sql = "INSERT INTO QUESTION ('ID', 'QUESTION') VALUES ('2','Apasa A');";
        // rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
        // strcpy(td->questions[2], question);

        // //3
        // sql = "INSERT INTO QUESTION ('ID', 'QUESTION') VALUES ('3','Apasa B');";
        // rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
        // strcpy(td->questions[3], question);

        // //4
        // sql = "INSERT INTO QUESTION ('ID', 'QUESTION') VALUES ('4','Apasa B');";
        // rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
        // strcpy(td->questions[4], question);

        // //5
        // sql = "INSERT INTO QUESTION ('ID', 'QUESTION') VALUES ('5','Apasa C');";
        // rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
        // strcpy(td->questions[5], question);

        // //6
        // sql = "INSERT INTO QUESTION ('ID', 'QUESTION') VALUES ('6','Apasa C');";
        // rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
        // strcpy(td->questions[6], question);

        // //7
        // sql = "INSERT INTO QUESTION ('ID', 'QUESTION') VALUES ('7','Apasa D');";
        // rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
        // strcpy(td->questions[7], question);

        // //8
        // sql = "INSERT INTO QUESTION ('ID', 'QUESTION') VALUES ('8','Apasa D');";
        // rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
        // strcpy(td->questions[8], question);

        // //9
        // sql = "INSERT INTO QUESTION ('ID', 'QUESTION') VALUES ('9','Apasa D');";
        // rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
        // strcpy(td->questions[9], question);

        pthread_create(&th[i], NULL, &treat, td);
    } //while
    //free(td);
};

void *manage_time(void *arg)
{

    seconds = 0;
    while (first_client == false)
    {
        sleep(1);
    }
    //s a conectat primul client in punctul asta tre sa trimit semnal cu secundele sa se astepte 15 de secunde sa intre lumea
    while (seconds <= 16)
    {
        sleep(1);
        seconds++;
        std::cout << "\nSECONDS=" << seconds << '\n';
        int aux = strlen("TIMER");
        std::cout << "SIZE OF TIMER = " << aux << '\n';
        for (int i = 0; i < all_clients.size(); i++)
        {
            if (write(all_clients[i], &aux, sizeof(int)) <= 0)
            {
                perror("[Thread]Eroare la write() catre client.\n");
            }
            if (write(all_clients[i], "TIMER", aux) <= 0)
            {
                perror("[Thread]Eroare la write() catre client.\n");
            }

            int nr_bytes = sizeof(int);
            if (write(all_clients[i], &nr_bytes, sizeof(int)) <= 0)
            {
                perror("[Thread]Eroare la write() catre client.\n");
            }
            if (write(all_clients[i], &seconds, sizeof(int)) <= 0)
            {
                perror("[Thread]Eroare la write() catre client.\n");
            }
        }
    }
    seconds = 0;
    while (1)
    {
        printf("!!!A INCEPUT JOCUL!!!\n");
        sleep(1);
        seconds++;
        std::cout << "\nSECONDS=" << seconds << '\n';
        int aux = strlen("TIMER");
        std::cout << "SIZE OF TIMER = " << aux << '\n';
        for (int i = 0; i < all_clients.size(); i++)
        {
            if (write(all_clients[i], &aux, sizeof(int)) <= 0)
            {
                perror("[Thread]Eroare la write() catre client.\n");
            }
            if (write(all_clients[i], "TIMER", aux) <= 0)
            {
                perror("[Thread]Eroare la write() catre client.\n");
            }

            int nr_bytes = sizeof(int);
            if (write(all_clients[i], &nr_bytes, sizeof(int)) <= 0)
            {
                perror("[Thread]Eroare la write() catre client.\n");
            }
            if (seconds == 11)
            {
                if (write(all_clients[i], &seconds, sizeof(int)) <= 0)
                {
                    perror("[Thread]Eroare la write() catre client.\n");
                }
                seconds = 0;
            }
            else if (write(all_clients[i], &seconds, sizeof(int)) <= 0)
            {
                perror("[Thread]Eroare la write() catre client.\n");
            }
        }
    }
}

static void *treat(void *arg)
{
    char myName[30] = "";
    bool once = false;
    unsigned int score = -1;
    while (1)
    {
        thData tdL;
        tdL = *((thData *)arg);
        printf("[thread]- %d - Asteptam mesajul...\n", tdL.idThread);
        fflush(stdout);
        pthread_detach(pthread_self());
        if (once == false)
        {
            if (read(tdL.cl, myName, 30) <= 0)
            {
                perror("Eroare la read() de la client.\n");
            }
            once = true;
        }
        int x = raspunde((thData *)arg, score, myName);
        if (x == -1)
        {
            close((intptr_t)arg);
            free((thData *)arg);
            return (NULL);
        }
    }
    /* am terminat cu acest client, inchidem conexiunea */

    close((intptr_t)arg);
    free((thData *)arg);
    return (NULL);
};

int get_index_from_respond(char res[])
{
    int ans = 0;
    ans = res[1] - '0';
    if (res[2] >= '0' && res[2] <= '9') //2 digit index;
        ans = ans * 10 + (res[2] - '0');
    return ans;
}

int raspunde(void *arg, unsigned int &score, char myName[30])
{
    first_client = true;
    char respond_from_client[200];
    thData tdL;
    tdL = *((thData *)arg);

    ClientsData aux;
    strcpy(aux.username, myName);
    aux.score = score;

    clients_data.push_back(aux);
    all_clients.push_back(tdL.cl);
    if (score == -1)
    {
        int aux = strlen("QUESTION");
        if (write(tdL.cl, &aux, sizeof(int)) <= 0) //nr bytes
        {
            printf("[Thread %d] ", tdL.idThread);
            perror("[Thread]Eroare la write() catre client.\n");
        }
        if (write(tdL.cl, "QUESTION", strlen("QUESTION")) <= 0) //comanda
        {
            printf("[Thread %d] ", tdL.idThread);
            perror("[Thread]Eroare la write() catre client.\n");
        }

        aux = strlen(tdL.questions[0]);
        if (write(tdL.cl, &aux, sizeof(int)) <= 0) //nr bytes
        {
            printf("[Thread %d] ", tdL.idThread);
            perror("[Thread]Eroare la write() catre client.\n");
        }
        if (write(tdL.cl, tdL.questions[0], strlen(tdL.questions[0])) <= 0) //intrebarea
        {
            printf("[Thread %d] ", tdL.idThread);
            perror("[Thread]Eroare la write() catre client.\n");
        }
        else
            printf("[Thread %d]Mesajul a fost trasmis cu succes.\n", tdL.idThread);
        score = 0;
    }
    if (read(tdL.cl, respond_from_client, 30) <= 0)
    {
        printf("RESPOND FROM CLIENT ->%s\n", respond_from_client);
        printf("[Thread %d]\n", tdL.idThread);
        perror("Eroare la read() de la client.\n");
    }

    if (strcmp(respond_from_client, "EXIT") == 0)
    {
        close(tdL.cl);
        return -1;
    }
    printf("[Thread %d]Mesajul a fost receptionat...%s\n", tdL.idThread, respond_from_client);
    if (strcmp(tdL.username, "") == 0)
    {
        sprintf(tdL.username, "%s\0", respond_from_client);
        strcpy(myName, tdL.username);
        printf("%s e numele clientului\n", myName);
    }

    int answer = respond_from_client[0] - '0';                        //get the answer (A,B,C or D)
    int received_index = get_index_from_respond(respond_from_client); //get the index that's found after the answer

    if (received_index <= 9)
    {
        //pressed_button_client = true;
        if (answer == tdL.answers[received_index])
        {
            //std::cout << "AM RASPUNS " << answer << " trebuia raspuns " << tdL.answers[received_index] << '\n';
            clients_data[tdL.idThread].score += 10;
            printf("[Thread %d]Trimitem mesajul inapoi...%s, index %d\n", tdL.idThread, "Raspuns corect", received_index);
        }
        else
        {
            printf("[Thread %d]Trimitem mesajul inapoi...%s, index %d\n", tdL.idThread, "Raspuns gresit", received_index);
        }

        //-------------------------------------------------

        if (received_index == 9)
        {

            for (int i = 0; i < clients_data.size() - 1; i++)
                for (int j = i + 1; j < clients_data.size(); j++)
                    if (clients_data[i].score < clients_data[j].score)
                    {
                        ClientsData aux = clients_data[i];
                        clients_data[i] = clients_data[j];
                        clients_data[j] = aux;
                    }
            //sleep(1);
            int aux = 112;
            printf("CASTIGATORUL TRIMIS ESTE: %s", myName);
            if (write(tdL.cl, &aux, sizeof(int)) <= 0) //nr bytes
            {
                printf("[Thread %d] ", tdL.idThread);
                perror("[Thread]Eroare la write() catre client.\n");
            }
            if (write(tdL.cl, clients_data[0].username, 30) <= 0) //nr bytes
            {
                printf("[Thread %d] ", tdL.idThread);
                perror("[Thread]Eroare la write() catre client.\n");
            }
        }
        int aux = strlen("QUESTION");
        if (write(tdL.cl, &aux, sizeof(int)) <= 0) //nr bytes
        {
            printf("[Thread %d] ", tdL.idThread);
            perror("[Thread]Eroare la write() catre client.\n");
        }
        if (write(tdL.cl, "QUESTION", strlen("QUESTION")) <= 0) //comanda
        {
            printf("[Thread %d] ", tdL.idThread);
            perror("[Thread]Eroare la write() catre client.\n");
        }
        aux = strlen(tdL.questions[received_index + 1]);
        if (write(tdL.cl, &aux, sizeof(int)))
        {
            printf("[Thread %d] ", tdL.idThread);
            perror("[Thread]Eroare la write() catre client.\n");
        }
        if (write(tdL.cl, tdL.questions[received_index + 1], strlen(tdL.questions[received_index + 1])) <= 0) //intrebarea
        {
            printf("[Thread %d] ", tdL.idThread);
            perror("[Thread]Eroare la write() catre client.\n");
        }
        else
        {
            printf("[Thread %d]Mesajul a fost trasmis cu succes. -> %s\n", tdL.idThread, tdL.questions[received_index + 1]);
        }
        seconds = 0;
    }

    return 0;
}