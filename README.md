
# Progetto C++ e Qt - settembre 2022
Questa repository contiene il mio progetto svolto per l'esame di Programmazione C++.
La traccia del progetto è il file Esame-220926.pdf
## Multiset ordinato
Il progetto C++ consiste nella realizzazione di un multiset ordinato minimizzando l'uso della memoria (cioè l'implementazione della classe container deve utilizzare un array dinamico di struct <elemento, numero di occorrenze>). 
Un multiset ordinato è un insieme ordinato che ammette valori duplicati: i valori duplicati non sono tuttavia salvati in memoria, in quanto viene salvato solo il numero di occorrenze, nella struct interna, per ogni elemento distinto. 

Per specificare la policy di ordinamento tra elementi di tipo generico T si utilizza un funtore di ordinamento, mentre per verificare l'uguaglianza viene usato un funtore di uguaglianza.

Si può costruire un multiset ordinato a partire da una coppia di iteratori, di inizio e fine sequenza, generici che si riferiscono a una sequenza di dati di tipo Q, lasciando al compilatore la conversione.

L'iteratore di sola lettura, const_iterator, che ho implementato è di categoria bidirectional per due motivi:

 - la struttura dati è ordinata quindi ha senso potermi chiedere quale elemento precede un altro;
 - ho controllato sulla STL e [std::multiset](https://en.cppreference.com/w/cpp/container/multiset) utilizza un bidirectional.

L'operatore di uguaglianza è templato perché il tipo del funtore di ordinamento utilizzato sul right hand side può essere diverso, quindi due multiset ordinati, per la traccia, sono uguali se e solo se contengono stessi elementi e stesse occorrenze: non conta l'ordine nell'operator==, nonostante la struttura dati sia ordinata. Ad esempio, i due multiset {<1, 1>, <2, 1>} e {<2, 1>, <1, 1>} risultano uguali, anche se l'ordinamento usato è diverso.
### Gestione particolare delle eccezioni
In ordered_multiset.h, alle righe 378, 384 e 445, ho utilizzato un particolare meccanismo per la gestione implicita delle eccezioni provenienti dal tipo generico T nel momento in cui utilizzo l'operatore di assegnamento sulla struct interna, che a sua volta utilizza il copy constructor della struct che infine nell'initialization list usa il copy constructor del tipo T. Al posto di fare il solito try-catch sul singolo array dinamico (nei metodi add e remove), quello che vado a fare è crearmi un dato automatico temporaneo di tipo ordered_multiset e crearmi l'array dinamico come dato membro: lavorando su un dato automatico, se avviene un'eccezione allora esce di scope e quindi viene chiamato il distruttore che ha il compito di deallocare l'array dinamico, non permettendo memory leak, e poi l'eccezione arriverà come sempre al chiamante. Ho scoperto durante l'orale che questa cosa ha un nome e si chiama **stack unwinding**.
### Testing
Per quanto riguarda invece il main.cpp, ho strutturato i test in modo particolare: per ogni tipo di dato testato, semplice e custom, faccio diverse chiamate (una per ogni combinazione di funtori) alla mia funzione templata che "in automatico" permette di testare ogni metodo dell'interfaccia pubblica, richiamando funzioni di comodo che generano ogni volta il risultato atteso per ogni situazione.
Di default l'output dei test viene salvato su file .txt (uno per ogni tipo di dato) all'interno della cartella "./tests_output/", perché, nonostante abbia implementato delle stampe colorate per terminali Linux, le stampe tutte insieme non sono molto leggibili. Per modificare la directory di output è sufficiente cambiare la macro definita alla riga 39 del main.cpp: il percorso deve terminare con uno slash ed è importante che esista. Se invece si desidera stampare tutto su console è sufficiente commentare la riga 35 che definisce il tag OUTPUT_TO_FILE.
### Valutazione progetto C++ (in 30esimi, poi convertito in 25esimi)
Non ho avuto penalizzazioni per la parte di C++, anzi ho avuto due punti bonus:

 - Documentazione: Eccellente. Implementazione con array dinamico (+1)
 - main.cpp: Molti test strutturati molto bene anche su tipi custom (point, person, fraction, multiset) (+1)

## Dynamic interaction graph
Per il progetto Qt è stata richiesta l'implementazione di un'interfaccia grafica per la visualizzazione di un dynamic interaction graph, modello usato per le interazioni sociali. 
In pratica ogni nodo è come se fosse una persona di un social network.

Il grafo all'inizio viene inizializzato con un numero casuale di nodi, poi dopo ogni secondo, finché non si arriva a 30 nodi, si aggiungono nuovi nodi, si aggiungono nuovi archi e infine si incrementa il numero di alcune interazioni.

Ogni nodo presenta un colore che cambia ogni volta che cambia il numero più alto di interazioni presente sugli archi che collegano il nodo.

Il colore viene codificato tramite una colormap.

### Implementazione
Ho utilizzato una matrice di adiacenza 30x30, in quanto non si tiene conto dell'efficienza, per salvare il numero di interazioni tra due nodi, ma solo la parte inferiore viene usata perché si tratta di un grafo non orientato e quindi la matrice risulterebbe simmetrica altrimenti.
Sulla diagonale principale salvo il numero massimo di interazioni per ogni nodo, in quanto è comodo per gestire il ridimensionamento della finestra nel momento in cui devo ridisegnare tutto nel paintEvent. 
Invece, quando il grafo si sta inizializzando (numero di nodi < 30) non posso subito utilizzare il colore finale per ogni nodo perché vedrei in anticipo colori diversi rispetto agli archi disegnati sul grafo in quel secondo.
Un'alternativa al ridimensionamento della finestra era bloccare la dimensione della finestra, perché nella traccia con "contenuto adattivo" si intende semplicemente che il grafo sia sempre visibile (guardare e-learning).
### Valutazione progetto Qt (su 5 punti)
A differenza di C++, ho avuto una penalizzazione di un punto in Qt perché nella finestra non ho mostrato una colorbar:

 - Manca una colorbar con cui interpretare il colore dei nodi al variare della cardinalità di edge (-1).


Si tratta dell'unica cosa da aggiungere; comunque partendo da 29 è possibile avere 30L all'orale, rispondendo bene a ogni domanda.

