#include "Button.h"
#include "Screen.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <graphics.h>
#include <iostream>

using namespace std;

/* codul de eroare returnat de anumite apeluri */

extern int errno;
bool in_waiting_room = true;
/* portul de conectare la server*/
int port;

int timer, skip_question;
int sd; // descriptorul de socket

void EndGame(Screen &s)
{
    cleardevice();
    outtextxy(s.screenWidth - 100, 50, "EXIT");
    Question q("CASTIGATORUL ESTE:", getmaxwidth() / 2, getmaxheight() / 4);
    char winner[30];
    if (read(sd, winner, 30) < 0)
    {
        perror("[EndGame]Eroare la read() de la EndGame().\n");
    }
    printf("Am reusit sa preiau castigatorul %s\n", winner);
    Question q2(winner, getmaxwidth() / 2, getmaxheight() / 4 + 20);
}

void *manage_server(void *arg)
{
    Screen screen = *(Screen *)(arg);
    while (1)
    {
        int aux; //cati bytes are comanda
        if (read(sd, &aux, sizeof(int)) < 0)
        {
            perror("[client_thread]Eroare la read() de la server de la command 1.\n");
            return NULL;
        }
        if (aux == 112) //end the game
        {
            printf("APELEZ ENDGAME() PT CA S A TERMINAT JOCUL\n");
            EndGame(screen);
        }
        char *command = (char *)malloc(sizeof(char) * aux); //comanda
        if (read(sd, command, aux) < 0)
        {
            perror("[client_thread]Eroare la read() de la server de la command 2.\n");
            return NULL;
        }
        command[aux] = '\0';
        std::cout << "COMMAND = " << command << " cu -> " << aux << '\n';
        if (strcmp(command, "TIMER") == 0)
        {
            int nr_bytes;
            if (read(sd, &nr_bytes, sizeof(int)) < 0)
            {
                perror("[client_thread]Eroare la read() de la server.\n");
                return NULL;
            }
            if (read(sd, &timer, nr_bytes) < 0)
            {
                perror("[client_thread]Eroare la read() de la server.\n");
                return NULL;
            }
            if (timer > 11 && in_waiting_room == false)
            {
                printf("SAR PESTE INTREBARE ACUM!!!!\n");
            }

            std::cout << "TIMER=" << timer << '\n';
        }

        if (in_waiting_room == false)
        {
            if (strcmp(command, "QUESTION") == 0)
            {
                int aux2 = 0;
                if (read(sd, &aux2, sizeof(int)) < 0)
                {
                    perror("[client_thread]Eroare la read() de la server de la command 3.\n");
                    return NULL;
                }
                char answer_from_server[200] = "";
                if (read(sd, answer_from_server, aux2) < 0)
                {
                    perror("[client_thread]Eroare la read() de la server.\n");
                    return NULL;
                }

                /* afisam mesajul primit */
                printf("[client_thread]Mesajul primit este: %s\n", answer_from_server);
                cleardevice();
                screen.SetQuestion();
                Question q(answer_from_server, getmaxwidth() / 2, getmaxheight() / 4);
                screen.answer = -1;
            }
        }
        free(command);
    }
}

void Wait()
{
    while (timer <= 9)
        delay(100);
}

int GetButtonClick(Screen &screen) ///Returns 1 if clicked on button A, 2 for B, etc...
{
    int x = -1, y = -1;
    while (!ismouseclick(WM_LBUTTONDOWN) && timer <= 10)
    {
        delay(10);
    }
    if (timer <= 10)
    {
        getmouseclick(WM_LBUTTONDOWN, &x, &y);

        //std::cout << "ACM VAD DE CLICK ->";
        //test where the user clicked and if the coordonates are inside a button
        if (screen.leftUpPoint[0].x <= x && x <= screen.downRightPoint[0].x &&
            screen.leftUpPoint[0].y - 5 <= y && y <= screen.downRightPoint[0].y + 5)
        {
            setfillstyle(HATCH_FILL, GREEN);
            floodfill(x, y, WHITE);
            Wait();
            screen.answer = 0;
            return 0;
        }
        else if (screen.leftUpPoint[1].x <= x && x <= screen.downRightPoint[1].x &&
                 screen.leftUpPoint[1].y - 5 <= y && y <= screen.downRightPoint[1].y + 5)
        {
            setfillstyle(HATCH_FILL, GREEN);
            floodfill(x, y, WHITE);
            Wait();
            screen.answer = 1;
            return 1;
        }
        else if (screen.leftUpPoint[2].x <= x && x <= screen.downRightPoint[2].x &&
                 screen.leftUpPoint[2].y - 5 <= y && y <= screen.downRightPoint[2].y + 5)
        {
            setfillstyle(HATCH_FILL, GREEN);
            floodfill(x, y, WHITE);
            Wait();
            screen.answer = 2;
            return 2;
        }
        else if (screen.leftUpPoint[3].x <= x && x <= screen.downRightPoint[3].x &&
                 screen.leftUpPoint[3].y - 5 <= y && y <= screen.downRightPoint[3].y + 5)
        {
            setfillstyle(HATCH_FILL, GREEN);
            floodfill(x, y, WHITE);
            Wait();
            screen.answer = 3;
            return 3;
        }
        else if (x > screen.screenWidth - 200 && y < 200)
        {
            std::cout << "AR TREBUI SA IES\n";
            screen.answer = 4;
            return 4;
        }
    }
    else // if (in_waiting_room == false)
    {
        printf("AM RETURNAT 5\n");
        screen.answer = 5;
        return 5;
    }
    screen.answer = -1;
    return -1;
}
int seconds = 0;
void Lobby()
{
    cleardevice();
    while (timer < 15)
    {
        char seconds_char[3] = "";
        seconds_char[1] = (15 - (int)timer) % 10 + '0';
        seconds_char[0] = (15 - (int)timer) / 10 + '0';
        sleep(1);
        cleardevice();
        outtextxy(getmaxwidth() / 2, getmaxheight() / 2 + 100, seconds_char);
        seconds = timer;
    }
}

int main(int argc, char *argv[])
{
    int index_intrebare = 0;
    struct sockaddr_in server; // structura folosita pentru conectare
                               // mesajul trimis
    int nr = 0;
    char buf[10];

    /* exista toate argumentele in linia de comanda? */
    if (argc != 3)
    {
        printf("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
        return -1;
    }

    /* stabilim portul */
    port = atoi(argv[2]);

    /* cream socketul */
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Eroare la socket().\n");
        return errno;
    }

    /* umplem structura folosita pentru realizarea conexiunii cu serverul */
    /* familia socket-ului */
    server.sin_family = AF_INET;
    /* adresa IP a serverului */
    server.sin_addr.s_addr = inet_addr(argv[1]);
    /* portul de conectare */
    server.sin_port = htons(port);

    int gdriver = DETECT, gmode, errorcode;
    /* initialize graphics mode */

    initgraph(&gdriver, &gmode, "");

    /* read result of initialization */
    errorcode = graphresult();
    if (errorcode != grOk) /* an error occurred */
    {
        printf("Graphics error: %s\n", grapherrormsg(errorcode));
        printf("Press any key to exit:");
        getch();
        exit(1); /* return with error code */
    }

    /* ne conectam la server */
    if (connect(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
        perror("[client]Eroare la connect().\n");
        return errno;
    }
    printf("SD la connect=%d\n", sd);
    Screen screen;
    if (write(sd, screen.username, strlen(screen.username)) <= 0)
    {
        perror("[client]Eroare la write() spre server.\n");
        return errno;
    }
    Screen *p_screen = &screen;
    pthread_t manage_server_thread;
    pthread_create(&manage_server_thread, NULL, manage_server, p_screen);
    Lobby();
    in_waiting_room = false;
    int i = 0;
    char response[5] = "EXIT";
    if (screen.questions_set == true)
    {
        while (1)
        {
            usleep(10000);
            int click_response = GetButtonClick(screen);
            if (click_response == 5)
            {
                printf("AOLEU AOLEU S A TERMINAT TIMPUL\n");
                char respond_to_server[30];
                sprintf(respond_to_server, "%d%d\0", screen.answer, index_intrebare++);
                /* trimiterea mesajului la server */
                if (write(sd, respond_to_server, strlen(respond_to_server)) <= 0)
                {
                    perror("[client]Eroare la write() spre server.\n");
                    return errno;
                }
                usleep(1000000);
            }
            switch (screen.answer)
            {
            case 0:
                std::cout << "Answer in client ----> " << screen.answer << '\n';
                screen.answer = 0;
                break;
            case 1:
                std::cout << "Answer in client ----> " << screen.answer << '\n';
                screen.answer = 1;
                break;
            case 2:
                std::cout << "Answer in client ----> " << screen.answer << '\n';
                screen.answer = 2;
                break;
            case 3:
                std::cout << "Answer in client ----> " << screen.answer << '\n';
                screen.answer = 3;
                break;
            case 4:
                char answer_from_server[200];
                if (write(sd, response, sizeof(response)) <= 0)
                {
                    perror("[client]Eroare la write() spre server.\n");
                    return errno;
                }
                exit(0);
                break;
            default:
                screen.answer = -1;
                break;
            }
            if (screen.answer >= 0)
            {

                char respond_to_server[30];
                sprintf(respond_to_server, "%d%d\0", screen.answer, index_intrebare++);
                /* trimiterea mesajului la server */
                if (write(sd, respond_to_server, strlen(respond_to_server)) <= 0)
                {
                    perror("[client]Eroare la write() spre server.\n");
                    return errno;
                }
            }
        }
    }
    close(sd);
}