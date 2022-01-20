\documentclass{article}
\usepackage{graphicx}
\usepackage[utf8]{inputenc}
\usepackage{listings}

\title{Quizz Game}
\author{Adrian Onofrei}
\date{December 2021}

\begin{document}

\maketitle

\section{Abstract}
Proiectul constă în crearea unui joc multiplayer numit 'Quizz Game'. În această aplicație jucătorii se conectează si incep jocul, primind acelasi set de întrebări, în aceeași ordine. Server-ul va procesa răspunsurile lor si le va oferi un punctaj, la finalul jocului apărând un clasament.

\section{Introducere}
Această aplicație de tip server-client va avea un server TCP multithreading.
Când un client accesează jocul este întâmpinat de o pagină unde trebuie să își pună un nume de jucător la alegerea sa. Dacă acesta nu dorește acest lucru, poate participa în sesiunea jocului ca și \textbf{guest}, numele acestuia apărând sub forma: 'Guest$\_[index\_player]$ ( $Guest\_0$, $Guest\_1$, ...). După alegerea numelui și apăsarea tastei \textbf{ENTER}, clientul intră în jocul propriu-zis, unde are pe monitor întrebarea, iar jos variantele de răspuns, clientul trebuind să dea click pe cea pe care o consideră corectă. La final jucătorul va putea să-și vadă scorul obținut și poziția sa în clasament.\\
\pagebreak{}
\begin{center}
Pagina de \textbf{'LOGIN'}
\includegraphics[width=1\columnwidth]{login.PNG}\\
Interfața grafică din timpul \textbf{JOCULUI}
\includegraphics[width=1\columnwidth]{joc.PNG}\\
\end{center}

\section{Tehnologii utilizate}
Analizând aplicația 'Quizz Game' am decis să utilizez un server TCP deoarece, spre deosebire de un server UDP, cel TCP oferă siguranța că informația va ajunge unde trebuie, în cazul nostru, răspunsul clientului, trimiterea întrebării către client, preluarea întrebărilor din baza de date de către server. Deși protocolul TCP este mult mai încet în comparație cu UDP, acesta ne permite să retransmitem pachetul de date pierdute.\\
De asemenea, este nevoie de un server concurent și nu iterativ, deoarece clienții când își vor deschide jocul vor dori să îl joace, nu să își aștepte rândul. De aceea vom crea câte un thread pentru fiecare client ce se conectează la server, thread-ul fiind responsabil doar de clientul său și de nimic altceva în rest.\\
Pentru interfața grafică utilizez librăria graphics.h, cu ajutorul căreia am creat mai multe obiecte: button, screen, question care să simplifice scrierea codului pe viitor. De asemenea, interfața este creată în așa fel încât să fie scalable, butoanele primind o lungime raportată la lungimea ecranului clientului.\\
Pentru a stoca întrebările vom avea o bază de date în SQLite, bază de date ce va fi interogată de către server, ce va extrage întrebările pentru clienți.

\section{Arhitectura aplicației}
Pentru o vizualizare mai precisă a ceea ce am descris mai sus, am creat o diagramă ce reprezintă cum va funcționa  această aplicație:\\
\includegraphics[width=1\columnwidth]{diagrama.drawio.png}\\
Se poate observa cu ușurință 'scheletul' aplicației si ce protocol utilizăm în fiecare situație. De asemenea se poate observa că serverul crează 3 thread-uri, fiecare dintre ele fiind responsabile de câte un client.
\pagebreak{}
\section{Detalii de implementare}
\subsection{SERVER}\\
Serverul este pornit, iar acesta creaza in mod dinamic cate un thread pentru fiecare client ce se conecteaza la el. Thread-ul respectiv va avea grija doar de clientul sau, fiind responsabil sa primeasca raspunsurile clientului si sa-i trimita inapoi urmatoarea intrebare si, la finalul jocului, numele castigatorului.
De asemenea, in cazul in care un client paraseste aplicatia, jocul va continua intre jucatorii ramasi.\\
Serverul primeste de la client raspunsul in modalitatea descrisa in sectiunea 'CLIENT' de mai jos, verifica daca raspunsul pentru intrebarea respectiva este corect, adauga la scor 10 in caz afirmativ, apoi trimite urmatoarea intrebare catre clienti.\\
Modalitatea prin care se trimite o intrebare este urmatoarea: se trimite numarul de octeti al cuvantului 'QUESTION', se trimite stringul 'QUESTION' ce va reprezenta un identificator pentru ce e trimis in client, dupa aceea se trimite numarul de octeti al intrebarii, iar apoi intrebarea. Intr-o maniera asemanatoare se trimit datele si pentru timp. Pentru a cronometra clientii am creat un thread ce are ca job sa se ocupe doar cu trimisul de secunde ce au trecut de la inceperea intrebarii. Aceste date se trimit intr-o maniera asemanatoare ca cea descrisa anterior pentru intrebari.
\subsection{CLASELE AJUTATOARE}\\
Am creat o serie de clase ce m-au ajutat pe parcursul proiectului sa obtin rezultatele dorite mai rapid si mai simplu. De exemplu, clasa buton este responsabila in a crea un chenar cu text in mijloc, asemeni unui buton. In clasa Screen se 'deseneaza' ecranul in mod scalable, butoanele luadu-si dimensiunile in functie de rezolutia maxima suportata de ecranul pe care ruleaza aplicatia. De asemenea constructorul acestei clase incepe jocul propriu zis, aici avand loc 'login-ul'.\\
\subsection{CLIENT}\\
In aceasta clasa se afla codul ce are grija de client. Aici se afla aplicatia propriu zisa ce trebuie rulata pe calculatorul clientului, aceasta utilizandu-se de clasele ajutatoare in crearea interfetei grafice si in preluarea inputului. In clasa client se inregistreaza inputul primit de la client si se trimite in server sub forma de char[dim].\\ Pentru a explica pe scurt, clientul are 4 variante de raspuns: A,B,C,D, fiecare reprezentand cate unul din numerele 0,1,2,3. Spre server este trimis un text format din acest numar ce reprezinta varianta aleasa urmat de index-ul intrebarii la care a raspuns.\\
In client putem primi de la server 3 'comenzi' ce ne arata in realitate ce reprezinta datele pe care urmeaza sa le citim. Daca comanda este 'TIMER' inseamna ca urmeaza sa citim numarul de secunde, altfel, pentru comanda 'QUESTION' deducem ca urmeaza sa citim urmatoarea intreabare oferita de server.
\\
\\
Am ales sa creez un thread pentru a cronometra clientii din server pentru a putea face cronometrarea in acelasi timp cu verificarea.
\begin{verbatim}

\end{verbatim}

\section{Explaination of some code}\\
Mai jos avem codul functiei responsabile cu manage-uirea secundelor, dar inainte de asta, o explicatie pe scurt ar fi ca avem un vector numit 'all clients' unde retinem toti file descriptorii clientilor conectati. Asteptam o secunda, o numaram, apoi parcurgem tot array-ul mentionat anterior si trimitem numarul de biti ai comenzii, comanda 'TIMER', numarul de biti al textului ce reprezinta numarul de secunde, textul ce reprezinta numarul de secunde.\\
\\
\begin{lstlisting}[language=C++]
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
\end{lstlisting}

\section{Concluzii}

În concluzie, aplicația va funcționa pe baza unui server TCP multithreading cu o 'infinitate' de clienți ce vor juca 'Quizz Game' prin intermediul unei interfețe grafice scalable. O metodă de îmbunătățire ar fi din punct de vedere grafic schimbarea librăriei utilizate, iar din punct de vedere al conexiunii, extragerea tuturor întrebărilor din baza de date în server cu o singură interogare, salvându-le într-un array de string-uri.

\section{Bibliografie}
\begin{thebibliography}{10}
    \bibitem{}
    Notițele luate de-a lungul seminariilor\\
    
    \bibitem{}
    Site-ul laboratorului de unde am preluat fragmente de cod, informații legate de serverele TCP si UDP, etc...\\
    \url{https://profs.info.uaic.ro/~computernetworks/cursullaboratorul.php}
    
    \bibitem{}
    UDP vs TCP\\
    \url{https://www.youtube.com/watch?v=Vdc8TCESIg8}
    
    \bibitem{}
    Cu acest site am reușit să realizez diagrama de la capitolul \textbf{'Arhitectura aplicației'}\\
    \url{https://app.diagrams.net/}
    
    \bibitem{}
    Cum să adaugi cod în latex\\
    \url{https://www.overleaf.com/learn/latex/Code_listing}
    
    \bibitem{}
    Codul model pentru un server multithreading\\
    \url{https://profs.info.uaic.ro/~computernetworks/files/NetEx/S12/ServerConcThread/servTcpConcTh2.c}
  \end{thebibliography}  
\end{document}
