# Text-editor
A text editor using lists 
 Dorim să implementăm propriul editor de text prin însușirea unor operații care sunt deja implementate în editoarele 
 de text existente.

Considerăm că avem un fișier de intrare date.in în care avem un text (în funcție de test el va avea lungimea variabilă).

Am dori să prelucrăm informația din acest fișier prin diverse operații. Pentru aceasta,
vom considera fiecare caracter citit din fișier, elementul unei liste.

Operațiile pe care le vom aplica asupra textului vor fi primite printr-un alt fișier de intrare: operatii.in.
Acesta va avea pe prima linie un număr întreg N, ce va reprezenta numărul de operații care se aplica asupra textului.

În urma tuturor operațiilor aplicate, vom salva noul text într-un fișier ce va avea denumirea rezultat.out.

Deci putem vedea toată această implementare a unor operații asupra unui input citit dintr-un fișier
ca pe un editor propriu de text. Un fișier are asociat cursorul de fișier (file pointer) care indică 
poziția curentă în cadrul fișierului. Cursorul de fișier este un întreg care reprezintă deplasamentul
(offset-ul) față de începutul fișierului. La deschiderea fișierului acesta va avea valoarea 0.

Operațiile posibile asupra datelor din fișier vor fi:

    move n: mută în listă cursorul, cu n poziții de la poziția curentă, pornind de la poziția curentă; n-ul poate fi și număr negativ; dacă deplasarea depășește dimensiunea listei, poziția curentă va fi la începutul, respectiv la finalul listei, în funcție de semnul lui n
    insert String: înserează șirul de caractere String în listă, la poziția curentă a cursorului; cursorul va ramane la finalul sirului inserat
    del n: șterge din listă urmatoarele n elemente de la poziția curentă
    copy n: copiază n elemente din listă de la poziția curentă (această operație nu modifică poziția cursorului; poziția curentă în listă va fi neschimbată în urma salvării elementelor copiate)
    paste: inserează în listă elementele copiate cu ultima comandă copy la poziția curentă a cursorului, poziția cursorului fiind la finalul listei de elemente introduse
    undo: anuleză ultima operație făcută asupra listei (se poate efectua o singură operație undo, adică nu vor exista 2 operații undo consecutive)
    redo: să anuleze ultima operație undo (de asemenea nu vor exista 2 operații redo consecutive)
    backspace: șterge elementul anterior poziției curente (OBS: Dacă ne aflăm la începutul listei această operație nu va face nicio modificare)
    [BONUS]replace cuvânt cuvânt_nou:caută în listă unde apare parametrul cuvânt și îl înlocuiește cu parametrul dat prin cuvânt_nou
