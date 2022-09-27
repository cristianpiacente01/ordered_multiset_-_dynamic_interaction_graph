#ifndef ORDERED_MULTISET_H
#define ORDERED_MULTISET_H

#include <algorithm> // std::swap
#include <ostream> // std::ostream
#include "element_not_found.h" // element_not_found

#include <iterator> // std::bidirectional_iterator_tag
#include <cstddef>  // std::ptrdiff_t

/**
 * @file ordered_multiset.h
 * @brief Dichiarazione della classe ordered_multiset.
 * 
 * Questo file contiene la classe templata ordered_multiset,
 * in cui nella sezione privata e' definita una struct interna multi_element:
 * in pratica quest'ultima consiste in una coppia <valore, occorrenze>.
 * 
 * Inoltre nell'interfaccia pubblica e' presente un iteratore di sola lettura
 * const_iterator di tipo bidirectional.
 * 
 * @author 866020 Piacente Cristian
 */


/**
 * @brief Classe che rappresenta un multiset ordinato.
 * 
 * La classe realizza un multiset ordinato di oggetti di tipo T
 * e vengono memorizzati solo una volta gli elementi duplicati.
 * 
 * L'utente sceglie la policy di ordinamento tramite il funtore Cmp,
 * che restituisce true se i due valori passati come argomento sono nell'ordine desiderato,
 * e la policy di uguaglianza tra due elementi di tipo T tramite il funtore Eql,
 * che definisce il criterio per verificare se due elementi sono uguali tra di loro.
 * 
 * @tparam T tipo degli elementi contenuti all'interno del multiset ordinato
 * @tparam Cmp funtore che specifica la policy per l'ordinamento degli elementi
 * @tparam Eql funtore che stabilisce la policy di uguaglianza tra due elementi
 */
template <typename T, typename Cmp, typename Eql>
class ordered_multiset {
    
    struct multi_element; // forward declaration per il costruttore privato di const_iterator

    public:
        typedef T value_type; ///< Tipo degli elementi contenuti nel multiset ordinato
        typedef unsigned int size_type; ///< Tipo utilizzato dalla lunghezza dell'array di multi_element, ma anche dal numero totale di elementi e dal numero di occorrenze

        /**
         * @brief Costruttore di default (metodo fondamentale)
         * 
         * Costruttore di default per istanziare un oggetto di tipo ordered_multiset.
         * 
         * Setta il puntatore all'array di multi_element a nullptr
         * e la dimensione dell'array a 0.
         * 
         * @post _elements == nullptr
         * @post _array_size == 0
         * 
         */
        ordered_multiset() : _elements(nullptr), _array_size(0) { }

        /**
         * @brief Costruttore che prende una coppia di iteratori generici (punto 1 nel testo)
         * 
         * Costruttore che prende come argomenti una coppia iteratori generici, ossia un iteratore di inizio sequenza
         * e un iteratore di fine sequenza di dati generici Q. 
         * 
         * Il tipo Q, a cui fanno riferimento gli iteratori, puo' essere diverso da T 
         * ed e' lasciata al compilatore la gestione della conversione di dati tra Q e T.
         * 
         * Gli iteratori sono passati per valore, come detto a lezione, perche' di norma sono dati "piccoli", 
         * che impiegano poca memoria.
         * 
         * Nell'initialization list inizializzo il puntatore all'array di multi_element a nullptr
         * e la dimensione dell'array a 0 per portare subito la classe in uno stato coerente.
         * 
         * Nel corpo del metodo viene wrappato in un try-catch un for che scorre la sequenza di dati
         * e che utilizza il metodo add, che appunto puo' generare un'eccezione, 
         * per aggiungere un elemento alla volta al multiset ordinato.
         * 
         * In caso avvenga un'eccezione durante l'aggiunta degli elementi, 
         * viene fatta la recovery degli errori, tramite il metodo clear()  
         * che riporta a uno stato coerente la classe, e viene rilanciata l'eccezione al chiamante.
         * 
         * @tparam Iterator tipo dell'iteratore generico
         * @param begin iteratore di inizio sequenza
         * @param end iteratore di fine sequenza
         * 
         * @throw std::bad_alloc possibile eccezione di allocazione causata dalla add()
         * @throw std::exception possibile eccezione proveniente dal tipo generico T
         */
        template <typename Iterator>
        ordered_multiset(Iterator begin, Iterator end) : _elements(nullptr), _array_size(0) {
            try {
                for (; begin != end; ++begin)
                    add(static_cast<value_type>(*begin));
            }
            catch (...) {
                clear();
                throw;
            }
        }

        /**
         * @brief Distruttore (metodo fondamentale)
         * 
         * Il distruttore semplicemente richiama il metodo clear(),
         * che dealloca le risorse acquisite dalla classe 
         * e riporta a uno stato coerente (pattern RAII,
         * chi acquisisce e' responsabile del rilascio delle risorse, 
         * quindi la classe diventa il proprietario  
         * e deve deallocare tramite il distruttore).
         * 
         * @post _elements == nullptr
         * @post _array_size == 0
         * 
         */
        ~ordered_multiset() {
            clear();
        }

        /**
         * @brief Copy constructor (metodo fondamentale)
         * 
         * Copy constructor che permette di costruire un multiset ordinato a partire da un altro dello stesso tipo 
         * (stessi parametri templati).
         * 
         * Come negli altri costruttori, all'inizio porto a uno stato coerente la classe tramite l'initialization list.
         * 
         * Nel corpo del costruttore, come prima cosa alloco un nuovo array di multi_element 
         * con dimensione uguale alla dimensione dell'altro multiset ordinato.
         * 
         * Questa operazione iniziale non solo puo' generare un'eccezione di tipo std::bad_alloc a causa della new,
         * ma anche un altro tipo di eccezione, poiche' nel momento in cui si alloca un array di multi_element viene richiamato il ctor di default di
         * multi_element su ogni cella e questo in realta' potrebbe generare un'eccezione lanciata da parte del tipo generico T,
         * poiche' implicitamente viene costruito l'elemento interno al multi_element con T() e il ctor di default di T potrebbe generare
         * un'eccezione.
         * 
         * Se questa operazione fallisce si rimane in uno stato coerente, altrimenti si continua e 
         * in un try-catch vengono eseguite istruzioni che potrebbero generare un'eccezione, poiche' 
         * vado ad utilizzare l'operator= (si tratta di assegnamenti e non di inizializzazioni
         * perche' l'array e' gia' stato costruito in automatico col ctor di default, essendo un array di tipo custom) 
         * su multi_element, il quale utilizza il copy constructor di multi_element, 
         * che a sua volta nell'initialization list utilizza il cctor del tipo generico T.
         * 
         * Per quanto riguarda il try-catch, nel blocco try vengono ricopiati i dati membro dell'altro multiset ordinato 
         * (quindi l'array di multi_element e la dimensione) senza condivisione di dati tra le due istanze di multiset ordinato.
         * 
         * Se un'operazione dovesse fallire, si entra nel blocco catch che effettua la recovery degli errori,
         * riportando in uno stato coerente la classe, e rilancia l'eccezione al chiamante.
         * 
         * @param other altro multiset ordinato di cui fare la copia senza condivisione di dati
         * 
         * @throw std::bad_alloc possibile eccezione di allocazione
         * @throw std::exception possibile eccezione proveniente dal tipo generico T
         * 
         * @post _elements == other._elements
         * @post _array_size == other._array_size
         */
        ordered_multiset(const ordered_multiset &other) : _elements(nullptr), _array_size(0) {
            _elements = new multi_element[other._array_size];

            try {
                for (size_type i = 0; i < other._array_size; ++i)
                    _elements[i] = other._elements[i];
                _array_size = other._array_size;
            } 
            catch (...) {
                clear();
                throw;
            }
        }

        /**
         * @brief Operatore di assegnamento (metodo fondamentale)
         * 
         * Operatore di assegnamento che permette di effettuare una copia dei dati indipendente (senza condivisione) 
         * a partire da un'altra istanza di multiset ordinato, con gli stessi parametri templati.
         * 
         * La differenza tra copy constructor e operatore di assegnamento consiste nel fatto che
         * il cctor viene usato per costruire un multiset ordinato senza avere dati in precedenza,
         * mentre l'operator= viene richiamato nel momento in cui esistono gia' dei dati, 
         * quindi il multiset ordinato da cui si parte e' gia' stato inizializzato con un costruttore.
         * 
         * C'e' appunto una forte correlazione con il cctor in quanto l'operator= 
         * (se il puntatore this e' diverso dall'indirizzo dell'altro multiset ordinato)
         * sfrutta il copy constructor su un dato automatico per effettuare una copia indipendente di other,
         * se fallisce abbiamo comunque uno stato coerente, altrimenti scambia con la swap() il contenuto 
         * dell'istanza corrente puntata da this con il contenuto del dato automatico tmp (copia di other).
         * 
         * A fine metodo si esce di scope e i dati contenuti in tmp, ossia i vecchi dati dell'istanza corrente,
         * vengono distrutti in automatico poiche' tmp e' un dato automatico che risiede sullo stack 
         * e che vive solo nel suo scope.
         * 
         * Le eccezioni che potrebbero essere generate in questo metodo sono le stesse del copy constructor,
         * poiche' viene utilizzato per fare la copia di other, mentre le post-condizioni sono quelle del metodo swap().
         * 
         * @param other altro multiset ordinato da cui ricopiare i dati senza condivisione
         * 
         * @throw std::bad_alloc possibile eccezione di allocazione
         * @throw std::exception possibile eccezione proveniente dal tipo generico T
         * 
         * @return ordered_multiset& istanza corrente del multiset ordinato
         * 
         * @post _elements == other._elements
         * @post _array_size == other._array_size
         */
        ordered_multiset &operator=(const ordered_multiset &other) {
            if (this != &other) {
                ordered_multiset tmp(other);
                swap(tmp);
            }
            return *this;
        }

        /**
         * @brief Metodo clear che svuota il contenuto del multiset ordinato
         * 
         * Metodo utilizzato soprattutto per la recovery degli errori nei blocchi catch,
         * per riportare a uno stato coerente la classe.
         * 
         * Svuota tutto il contenuto del multiset ordinato, cioe' dealloca tutto l'array di multi_element
         * e riporta i dati membro in uno stato consistente.
         * 
         * Questo metodo puo' anche essere utilizzato da parte dell'utente per eliminare tutto il contenuto
         * del multiset ordinato con una singola operazione, risparmiando molte chiamate al metodo remove().
         * 
         * @post _elements == nullptr
         * @post _array_size == 0
         * 
         */
        void clear() {
            delete[] _elements;
            _elements = nullptr;
            _array_size = 0;
        }

        /**
         * @brief Metodo swap che scambia i dati del multiset ordinato con i dati di other
         * 
         * Metodo swap utilizzato per scambiare lo stato dell'istanza corrente di multiset ordinato
         * con lo stato di other.
         * 
         * Consiste semplicemente in chiamate a std::swap() per i dati membro, ossia l'array di multi_element 
         * e la dimensione dell'array, in modo tale che other avra' lo stato che era dell'istanza corrente
         * e l'istanza corrente avra' lo stato di other.
         * 
         * E' molto utile per altri metodi come add() 
         * che per evitare la recovery degli errori si appoggiano a un dato automatico "temporaneo"
         * e alla fine si scambia il contenuto dell'istanza corrente *this 
         * con il contenuto del dato automatico "temporaneo" (questa tecnica e' approfondita sul metodo add()),
         * cosi' il dato automatico contiene il vecchio stato e viene distrutto, appunto in automatico, a fine scope.
         * 
         * @post _elements == other._elements e viceversa
         * @post _array_size == other._array_size e viceversa
         * 
         * @param other altro multiset ordinato con cui scambiare i dati
         */
        void swap(ordered_multiset &other) {
            std::swap(_elements, other._elements);
            std::swap(_array_size, other._array_size);
        }

        /**
         * @brief Metodo size che restituisce la cardinalita' del multiset ordinato (metodo essenziale per la classe)
         * 
         * Metodo size, essenziale per la classe, che calcola il numero totale di elementi del multiset ordinato,
         * cioe' la cardinalita' del multiset ordinato data dalla somma delle molteplicita'
         * degli elementi contenuti.
         * 
         * Questo metodo calcola ogni volta il numero complessivo di elementi contenuti
         * per risparmiare l'uso di un aggiuntivo dato membro, che non porterebbe
         * a un vero e proprio uso minimale della memoria 
         * (anche se "minimizzare l'uso della memoria" riguarda la struttura dati 
         * e la grandezza tipica di un intero e' solo 4 bytes).
         * 
         * @return size_type numero totale degli elementi
         */
        size_type size() const {
            size_type result = 0;
            for (size_type i = 0; i < _array_size; ++i)
                result += _elements[i]._count;
            return result;
        }

        /**
         * @brief Metodo add che aggiunge un elemento al multiset ordinato (metodo essenziale per la classe)
         * 
         * Metodo add, essenziale per la classe, che permette di aggiungere un elemento al multiset ordinato.
         * 
         * L'aggiunta sfrutta l'implementazione particolare del multiset ordinato, che consiste
         * nell'evitare la memorizzazione di duplicati per ogni elemento contenuto.
         * 
         * In base a quanto appena detto, innanzitutto controllo col metodo contains() 
         * se l'elemento in questione e' gia' stato aggiunto in precedenza,
         * in caso affermativo scorro l'array di multi_element finche' non arrivo all'elemento in questione 
         * (viene sfruttata l'istanza _equals del funtore di uguaglianza),
         * poiche' se appartiene gia' al multiset ordinato devo solo incrementare il numero di occorrenze
         * all'interno della struct multi_element, per evitare di memorizzare duplicati.
         * Altrimenti, utilizzo la tecnica di appoggiarmi su un dato automatico tmp
         * per preservare lo stato coerente della classe.
         * 
         * Questa tecnica consiste in:
         * - istanziare un dato automatico che vive sullo stack solo nel suo scope
         * - effettuare tutte le operazioni "delicate" che potrebbero generare eccezioni sul dato automatico, 
         * come allocazione di memoria o altre eccezioni provenienti da un tipo generico
         * - se una di queste operazioni fallisce, viene lanciata un'eccezione al chiamante 
         * e il dato automatico esce di scope, quindi viene richiamato il suo distruttore e non ci sono memory leak
         * - altrimenti, al termine del metodo si effettua la swap con il dato automatico, 
         * in modo da lasciare i vecchi dati sul dato automatico che verra' distrutto a fine scope.
         * 
         * Qui di seguito viene spiegato in particolare il caso del metodo add.
         * Innanzitutto creo un multiset ordinato col costruttore di default, chiamato tmp 
         * perche' si tratta del dato automatico che verra' distrutto in automatico a fine scope.
         * Effettuo un'allocazione di un array dinamico di multi_element con dimensione = dimensione attuale + 1,
         * se questa operazione fallisce viene lanciata un'eccezione std::bad_alloc al chiamante 
         * (oppure un'altra eccezione proveniente dal ctor di default di T, implicitamente richiamato dal ctor di default di multi_element, 
         * quest'ultimo utilizzato per ogni cella di multi_element ogni volta che si alloca un array di multi_element) e tmp esce di scope.
         * Una volta cambiato su tmp il puntatore ad array di multi_element, cambio anche la dimensione.
         * Ora, poiche' si tratta di una struttura dati ordinata che permette all'utente di scegliere la policy di ordinamento 
         * (tramite funtore di ordinamento), devo cercare l'indice dell'array in cui memorizzare l'elemento passato come argomento.
         * Per fare cio', utilizzo un puntatore che all'inizio punta alla prima cella dell'array _elements: 
         * finche' non vado oltre la dimensione dell'array e finche' l'operator() di _compare (istanza del funtore di ordinamento)
         * restituisce true, applicato sull'elemento di tipo T e sull'elemento passato come argomento al metodo add(),
         * continuo incrementando sia la variabile che corrispondera' all'indice corretto in cui memorizzare l'elemento, 
         * sia il puntatore a un multi_element, per confrontare con il prossimo multi_element dell'array.
         * 
         * Trovato l'indice, vengono effettuate tre operazioni 
         * (che potrebbero generare un'eccezione proveniente dal tipo generico T, 
         * spiegato nella descrizione del copy constructor di ordered_multiset, ma se avviene un'eccezione come gia' detto non ci sono recovery da fare,
         * ne' memory leak, poiche' sto lavorando su un dato automatico e questo esce di scope)
         * prima di terminare con la swap:
         * - ricopio nell'array di tmp tutti i multi_element dall'indice 0 all'indice trovato - 1
         * - inserisco il nuovo elemento nella posizione corretta, utilizzando 1 come numero di occorrenze all'interno della struct
         * - ricopio nell'array di tmp in posizione i + 1 tutti gli elementi di posizione i, con i = indice trovato, ..., lunghezza del vecchio array - 1
         * (in pratica ricopio tutti gli elementi successivi, 
         * nel nuovo array viene utilizzato i + 1 proprio perche' l'inserimento del nuovo elemento causa uno "shift" a destra degli indici).
         * 
         * Infine, posso finalmente effettuare la swap tra l'istanza corrente e tmp: ora this ha i nuovi dati aggiornati e tmp contiene i vecchi dati,
         * quest'ultimo esce di scope a fine metodo e viene chiamato in automatico il distruttore che effettua la deallocazione del vecchio array,
         * evitando sia memory leak sia una gestione delle eccezioni (per effettuare un'eventuale recovery degli errori).
         * 
         * Questa tecnica di appoggiarmi su un dato automatico di tipo ordered_multiset (con stessi parametri templati) verra' sfruttata anche in altri metodi 
         * che contengono diverse operazioni che potrebbero potenzialmente lanciare eccezioni.
         * 
         * @param element elemento da aggiungere al multiset ordinato
         * 
         * @throw std::bad_alloc possibile eccezione di allocazione
         * @throw std::exception possibile eccezione proveniente dal tipo generico T
         * 
         * @post multiplicity(element) > 0
         */
        void add(const value_type &element) {
            if (contains(element)) {
                for (size_type i = 0; i < _array_size; ++i)
                    if (_equals(_elements[i]._element, element))
                        ++(_elements[i]._count);

            }
            else {
                ordered_multiset tmp;

                tmp._elements = new multi_element[_array_size + 1];
                tmp._array_size = _array_size + 1;

                size_type correct_index = 0;

                multi_element *curr = _elements;

                while (correct_index < _array_size && _compare(curr->_element, element)) {
                    ++correct_index;
                    ++curr;
                }

                for (size_type i = 0; i < correct_index; ++i)
                    tmp._elements[i] = _elements[i];
                
                tmp._elements[correct_index]._element = element;
                tmp._elements[correct_index]._count = 1;
                
                for (size_type i = correct_index; i < _array_size; ++i)
                    tmp._elements[i + 1] = _elements[i];
                
                swap(tmp);
            }
        }

        /**
         * @brief Metodo remove che rimuove un occorrenza di element dal multiset ordinato (metodo essenziale per la classe)
         * 
         * Metodo remove, essenziale per la classe, che permette di rimuovere un'occorrenza dell'elemento passato come argomento.
         * 
         * Come pre-condizione dev'essere vero che il multiset ordinato contiene almeno un'occorrenza dell'elemento
         * che si desidera eliminare. Se non viene rispettata la pre-condizione, viene lanciata la mia eccezione custom
         * element_not_found, che avverte l'utente, con un messaggio recuperabile tramite metodo what(), dell'impossibilita' 
         * di rimuovere un elemento non presente.
         * 
         * A questo punto, se la pre-condizione e' vera, scorro l'array di multi_element finche' non trovo l'elemento in questione,
         * utilizzando sempre il funtore di uguaglianza, e una volta trovato devo controllare quante occorrenze sono presenti.
         * 
         * Questo perche' la rimozione completa di un elemento avviene solo quando il numero di occorrenze diventa 0.
         * 
         * Se il conteggio e' maggiore di 1, semplicemente decremento il contatore di occorrenze, altrimenti
         * gestisco la rimozione completa strutturando il codice in base alla tecnica che ho documentato nel dettaglio nel metodo add().
         * 
         * Tralasciando i dettagli gia' spiegati di questa tecnica di lavorare con un dato automatico,
         * la rimozione completa e' strutturata nel modo seguente:
         * - alloco un array di dimensione = dimensione attuale - 1
         * - ricopio nel nuovo array di multi_element tutte le celle tranne quella che rappresenta l'elemento da eliminare definitivamente,
         * utilizzando una variabile current_index che incremento ogni volta che inserisco un multi_element nel nuovo array.
         * 
         * Infine, sempre in base alla tecnica utilizzata, effettuo la swap con il dato automatico che contiene i dati su cui ho lavorato.
         *
         * @param element elemento da rimuovere dal multiset ordinato (viene eliminata solo un'occorrenza)
         * 
         * @throw element_not_found eccezione custom lanciata se si prova a rimuovere un elemento non presente
         * @throw std::bad_alloc possibile eccezione di allocazione
         * @throw std::exception possibile eccezione proveniente dal tipo generico T
         * 
         * @pre  multiplicity(element) > 0
         * @post multiplicity(element) == multiplicity(element) - 1
         */
        void remove(const value_type &element) {
            if (!contains(element))
                throw element_not_found("Impossibile rimuovere un elemento non presente nel multiset ordinato.");
                
            for (size_type i = 0; i < _array_size; ++i)
                    if (_equals(_elements[i]._element, element)) {

                        if (_elements[i]._count > 1)
                            --(_elements[i]._count);

                        else {
                            ordered_multiset tmp;

                            tmp._elements = new multi_element[_array_size - 1];
                            tmp._array_size = _array_size - 1;

                            size_type current_index = 0;
                            
                            for (size_type j = 0; j < _array_size; ++j)
                                if (j != i)
                                    tmp._elements[current_index++] = _elements[j];
                            
                            swap(tmp);
                        }

                        break;
                    }
            
        }

        /**
         * @brief Metodo multiplicity che conta le occorrenze di element (metodo essenziale per la classe)
         * 
         * Metodo multiplicity, essenziale per la classe, che restituisce la molteplicita' dell'elemento passato come argomento,
         * ossia restituisce il numero di occorrenze.
         * 
         * Scorro l'array di multi_element e se trovo l'elemento che sto cercando allora restituisco il numero di occorrenze 
         * salvato nella struct multi_element, altrimenti il metodo restituisce 0. 
         * 
         * Si tratta di un metodo essenziale per la classe e metodi come contains() si basano su di esso.
         * 
         * @param element elemento di cui si vuole contare le occorrenze
         * @return size_type molteplicita' (numero di occorrenze) di element nel multiset ordinato
         */
        size_type multiplicity(const value_type &element) const {
            for (size_type i = 0; i < _array_size; ++i)
                if (_equals(_elements[i]._element, element))
                    return _elements[i]._count;

            return 0;
        }

        /**
         * @brief Operatore di confronto operator== (punto 3 nel testo)
         * 
         * Operatore di confronto operator== che verifica l'uguaglianza tra due multiset ordinati,
         * non tenendo conto della policy utilizzata per l'ordinamento.
         * 
         * Infatti questo metodo e' templato con un parametro generico CmpOther, automaticamente dedotto dall'argomento other,
         * che consiste nel tipo del funtore di ordinamento utilizzato da other, quindi potenzialmente diverso da Cmp 
         * (quest'ultimo utilizzato invece dall'istanza corrente).
         * 
         * Questo operatore controlla se nei due multiset ordinati sono presenti gli stessi elementi con lo stesso numero di occorrenze.
         * Tuttavia, avendo cambiato un parametro templato sul secondo tipo di dato allora si stanno trattando due tipi di dato diversi, 
         * il primo templato su <T, Cmp, Eql> e poi c'e' l'argomento, templato su <T, CmpOther, Eql>, quindi non si riesce ad accedere ai dati membro del secondo
         * ed e' necessario ricorrere all'interfaccia pubblica di other.
         * 
         * Questo operatore controlla prima di tutto, utilizzando il metodo size(), se il numero totale di elementi contenuti nel primo multiset ordinato
         * e' uguale al numero totale di elementi del secondo. 
         * Se non sono uguali viene restituito false, altrimenti scorro l'array di multi_element dell'istanza corrente e per ogni elemento controllo se
         * il numero di occorrenze salvato nel multi_element e' uguale al numero restituito da multiplicity() chiamato su other, passando l'elemento in questione.
         * Se entrambi i controlli hanno successo allora sono sicuro di avere due multiset ordinati con gli stessi elementi e lo stesso numero di occorrenze,
         * in quanto gli elementi con 0 occorrenze non vengono tenuti nell'array ma eliminati completamente, quindi non dovrebbero esserci corner case non considerati.
         * 
         * @tparam CmpOther funtore che specifica la policy per l'ordinamento di other
         * @param other altro multiset ordinato con cui verificare l'uguaglianza
         * @return true se i due multiset ordinati contengono gli stessi elementi con le stesse occorrenze (l'ordinamento non conta)
         * @return false il numero totale di elementi e' diverso oppure il conteggio di un elemento sull'istanza corrente e' diverso da quello su other
         */
        template <typename CmpOther>
        bool operator==(const ordered_multiset<T, CmpOther, Eql> &other) const {
            if (size() != other.size())
                return false;
            
            for (size_type i = 0; i < _array_size; ++i)
                if (_elements[i]._count != other.multiplicity(_elements[i]._element))
                    return false;

            return true;
        }

        /**
         * @brief Metodo contains che controlla se element esiste nel multiset ordinato (punto 4 nel testo)
         * 
         * Metodo contains che controlla se l'elemento passato come argomento e' presente nel multiset ordinato.
         * 
         * Per fare cio', sfrutta il metodo multiplicity() e il risultato di contains() sara' true 
         * se il numero di occorrenze di element e' maggiore di 0, false se non esiste alcuna occorrenza di element.
         * 
         * @param element elemento da cercare nel multiset ordinato
         * @return true se e' presente almeno un'occorrenza di element nel multiset ordinato
         * @return false se non e' presente alcuna occorrenza di element
         */
        bool contains(const value_type &element) const {
            return multiplicity(element) > 0;
        }

        /**
         * @brief Funzione globale operator<< per un multiset ordinato (punto 5 nel testo)
         * 
         * Funzione globale operator<< overloadata per inviare il contenuto di un multiset ordinato (ordered_multiset) su uno stream di output (std::ostream).
         * 
         * Il contenuto del multiset ordinato viene inviato nella forma 
         * {<valore1, occorrenze1>, <valore2, occorrenze2>, ..., <valoreN, occorrenzeN>} come richiesto, in cui <valore, occorrenze> indica un multi_element.
         * 
         * Questa funzione globale ha la keyword friend per poter accedere ai dati membro della classe, un'alternativa sarebbe
         * scrivere la funzione fuori dalla classe e utilizzare gli iteratori, ma l'alternativa sarebbe scomoda perche' l'iteratore ritorna tutti gli elementi
         * del multiset ordinato, cioe' anche i duplicati da contare oppure si dovrebbe utilizzare il metodo multiplicity() per ogni elemento diverso.
         * Quindi credo che l'ideale sia rendere friend l'operator<< e accedere all'array di multi_element.
         * 
         * @param os stream di output su cui inviare il contenuto del multiset ordinato om
         * @param om multiset ordinato di cui si desidera inviare il contenuto sullo stream di output os
         * @return std::ostream& stream di output os dopo aver mandato il contenuto di om
         */
        friend std::ostream &operator<<(std::ostream &os, const ordered_multiset &om) {
            os << "{";
            for (typename ordered_multiset::size_type i = 0; i < om._array_size; ++i) {
                os << "<" << om._elements[i]._element << ", " << om._elements[i]._count << ">";
                if (i != om._array_size - 1)
                    os << ", ";
            }
            os << "}";
            return os;
        }

        /**
         * @brief Iteratore di sola lettura di tipo bidirectional (punto 2 nel testo)
         * 
         */
        class const_iterator {
            public:
                // traits
                typedef std::bidirectional_iterator_tag iterator_category; ///< Categoria dell'iteratore, bidirectional in questo caso
                typedef T                               value_type; ///< Tipo del dato riferito
                typedef ptrdiff_t                       difference_type; ///< Tipo che potrebbe essere usato per identificare la distanza tra iteratori
                typedef const T*                        pointer; ///< Tipo del puntatore al dato riferito
                typedef const T&                        reference; ///< Tipo della reference al dato riferito

                /**
                 * @brief Costruttore di default, inizializza il puntatore a multi_element con nullptr e il conteggio delle occorrenze rimanenti a 0
                 * 
                 * @post _ptr == nullptr
                 * @post _count == 0
                 */
                const_iterator() : _ptr(nullptr), _count(0) { }
                
                /**
                 * @brief Copy constructor, e' voluta la condivisione di dati tramite il puntatore
                 * 
                 * @param other altro const_iterator da cui copiare lo stato
                 * 
                 * @post _ptr == other._ptr
                 * @post _count == other._count
                 */
                const_iterator(const const_iterator &other) : _ptr(other._ptr), _count(other._count) { }

                /**
                 * @brief Operatore di assegnamento, non serve controllare che il puntatore this sia diverso dall'indirizzo di other
                 * 
                 * @param other altro const_iterator da cui copiare i dati 
                 * @return const_iterator& istanza corrente di const_iterator
                 */
                const_iterator& operator=(const const_iterator &other) {
                    _ptr = other._ptr;
                    _count = other._count;
                    return *this;
                }

                /**
                 * @brief Distruttore, vuoto perche' il costruttore non acquisisce risorse
                 * 
                 */
                ~const_iterator() { }

                /**
                 * @brief Operator* (dereferenziamento)
                 * 
                 * @return reference dato riferito dall'iteratore
                 */
                reference operator*() const {
                    return _ptr->_element;
                }

                /**
                 * @brief Operator-> che restituisce un puntatore al dato riferito
                 * 
                 * @return pointer puntatore al dato riferito
                 */
                pointer operator->() const {
                    return &(_ptr->_element);
                }
                
                /**
                 * @brief Operatore di post-incremento
                 * 
                 * Operatore di post-incremento, restituisce una copia del const_iterator prima dell'incremento.
                 * 
                 * L'operazione di incremento su questo iteratore e' particolare e viene spiegata nel pre-incremento,
                 * poiche' e' presente lo stesso blocco di codice per effettuare l'incremento.
                 * 
                 * @return const_iterator copia del const_iterator precedente all'incremento
                 */
                const_iterator operator++(int) {
                    const_iterator tmp(*this);

                    if (_count == 0)
                        _count = _ptr->_count;

                    --_count;

                    if (_count == 0)
                        ++_ptr;

                    return tmp;
                }
                
                /**
                 * @brief Operatore di pre-incremento
                 * 
                 * Operatore di pre-incremento, restituisce l'istanza corrente dopo aver eseguito l'incremento.
                 * 
                 * Questo iteratore e' particolare poiche' utilizza due dati membro, 
                 * ossia il puntatore al multi_element e un contatore di occorrenze rimanenti.
                 * 
                 * Il contatore del const_iterator e' fondamentale per implementare l'incremento: viene utilizzato per sapere 
                 * quante volte bisogna ancora rimanere sul dato riferito prima di puntare alla prossima cella di multi_element,
                 * poiche' nella struttura interna del multiset ordinato non memorizziamo duplicati ma il numero di occorrenze per quell'elemento,
                 * quindi l'iteratore dovra' riferirsi allo stesso elemento finche' non si esauriscono tutte le occorrenze.
                 * 
                 * Il contatore _count del const_iterator all'inizio viene inizializzato sempre con valore 0
                 * perche' se andassi a leggere subito dal puntatore di multi_element il numero di occorrenze avrei un problema di memoria 
                 * con l'iteratore di fine sequenza, in quanto questo punta alla cella successiva alla fine dell'array di multi_element, 
                 * e quindi Valgrind darebbe un errore di tipo Invalid read of size 4.
                 * 
                 * Inoltre se nel codice dell'incremento andassi a leggere subito dopo aver incrementato il puntatore
                 * allora si avrebbe lo stesso problema una volta arrivati alla fine, poiche' si andrebbe oltre l'ultimo elemento dell'array di multi_element.
                 * 
                 * Come valore segnaposto del contatore di occorrenze rimanenti utilizzo 0 poiche' non si ha mai un elemento con 0 occorrenze nell'array di multi_element
                 * e perche' sto utilizzando unsigned int, che mi sembra piu' appropriato rispetto ad int.
                 * 
                 * La logica dell'incremento funziona nel seguente modo:
                 * - se il contatore e' uguale a 0 allora leggo dal multi_element il numero di occorrenze
                 * - decremento sempre il contatore
                 * - se ora e' uguale a 0 significa che c'era una sola occorrenza, incremento il puntatore a multi_element, altrimenti rimango sullo stesso multi_element.
                 * 
                 * Questa logica permette di evitare problemi di memoria, rimandando la lettura del numero di occorrenze solo quando viene richiesto un incremento.
                 * 
                 * @return const_iterator& istanza corrente dopo l'incremento
                 */
                const_iterator& operator++() {
                    if (_count == 0)
                        _count = _ptr->_count;

                    --_count;

                    if (_count == 0)
                        ++_ptr;

                    return *this;
                }

                /**
                 * @brief Operatore di post-decremento
                 * 
                 * Operatore di post-decremento, restituisce una copia del const_iterator prima del decremento.
                 * 
                 * L'operazione di decremento, come per l'incremento, su questo iteratore e' particolare e viene spiegata nel pre-decremento.
                 * 
                 * @return const_iterator copia del const_iterator precedente al decremento
                 */
                const_iterator operator--(int) {
                    const_iterator tmp(*this);

                    if (_count == 0) { // se e' 0 (valore segnaposto usato nel ctor e nell'incremento) siamo all'inizio della sequenza di occorrenze per questo elemento, mi sposto verso sinistra
                        --_ptr;
                        _count = 1; // ora siamo alla fine delle occorrenze dell'elemento precedente
                    }
                    else {
                        ++_count; // mi sposto verso sinistra, aumenta il numero di occorrenze rimanenti verso destra

                        if (_count == _ptr->_count + 1) { // siamo andati oltre
                            --_ptr;
                            _count = 1;
                        }
                    }

                    return tmp;
                }

                /**
                 * @brief Operatore di pre-decremento
                 * 
                 * Operatore di pre-decremento, restituisce l'istanza corrente dopo aver eseguito il decremento.
                 * 
                 * Questo iteratore e' particolare poiche' utilizza due dati membro, 
                 * ossia il puntatore al multi_element e un contatore di occorrenze rimanenti verso destra (viene riutilizzato
                 * lo stesso rispetto agli operatori di incremento ma questa volta cambia la logica nell'utilizzo).
                 * 
                 * Rispetto all'incremento, il contatore viene utilizzato sempre per tener conto di quante occorrenze rimangono leggendo verso destra,
                 * quindi non cambia la sua semantica. Tuttavia, la struttura del decremento e' leggermente piu' complessa rispetto all'incremento, 
                 * questo perche' innanzitutto devo controllare se come contatore abbiamo il valore segnaposto 0 utilizzato sia dal costruttore che
                 * dall'incremento quando ci si sposta: se e' questo il caso, allora so che devo subito spostarmi verso sinistra al precedente,
                 * quindi decremento il puntatore a multi_element e imposto a 1 il numero di occorrenze rimanenti verso destra, poiche'
                 * siamo alla fine della sequenza di occorrenze dell'elemento.
                 * 
                 * Altrimenti, viene incrementato il numero di occorrenze rimanenti verso destra, proprio perche' ci stiamo spostando verso sinistra,
                 * e il controllo per passare al prossimo multi_element riguarda il fatto che siamo andati oltre il numero di occorrenze salvato nel multi_element.
                 * 
                 * La logica del decremento funziona nel seguente modo:
                 * - se e' presente il valore segnaposto, mi sposto subito verso sinistra e setto a 1 il numero di occorrenze rimanenti verso destra
                 * - altrimenti, incremento il contatore
                 *  - se ora abbiamo superato il numero di occorrenze memorizzato nella struct, significa che mi devo spostare al precedente multi_element, decremento il puntatore a multi_element, altrimenti si rimane sullo stesso multi_element.
                 * 
                 * @return const_iterator& istanza corrente dopo l'incremento
                 */
                const_iterator &operator--() {
                    if (_count == 0) {
                        --_ptr;
                        _count = 1;
                    }
                    else {
                        ++_count;

                        if (_count == _ptr->_count + 1) {
                            --_ptr;
                            _count = 1;
                        }
                    }

                    return *this;
                }

                /**
                 * @brief Operatore di confronto operator==
                 * 
                 * Due const_iterator sono uguali se e solo se puntano allo stesso multi_element e hanno lo stesso numero di occorrenze rimanenti,
                 * poiche' ad esempio se ho una sequenza di dati 1 4 4 4 7 10 12 e due const_iterator in cui il primo si riferisce alla prima occorrenza di 4 
                 * e il secondo alla seconda occorrenza di 4,
                 * per me sono due const_iterator diversi in quanto si trovano in punti diversi della sequenza di dati.
                 * 
                 * CORNER CASE: se un contatore e' uguale a 1 e l'altro 0, i due iteratori possono essere sia uguali sia diversi.
                 * 
                 * Esempio:
                 *  - con la sequenza di dati 3 3, begin si riferisce alla prima occorrenza di 3 e ha contatore uguale a 0,
                 *  end si riferisce all'elemento oltre la fine della sequenza e ha contatore uguale a 0,
                 *  decremento end e il contatore diventa 1 e si riferisce all'ultima occorrenza di 3: i due iteratori sono DIVERSI
                 *  - con la sequenza di dati 3 (cioe' una sola occorrenza), begin si riferisce all'elemento 3 e ha contatore uguale a 0, 
                 *  end all'elemento oltre la fine con contatore uguale a 0, decremento end e il contatore diventa 1: 
                 *  i due iteratori si riferiscono allo stesso elemento (UGUALI) ma il conteggio delle occorrenze rimanenti e' diverso!
                 * 
                 * Per risolvere questo problema, se siamo in questa situazione allora ci aspettiamo che il puntatore
                 * al multi_element punti a un multi_element con un elemento della sequenza, 
                 * poiche' se abbiamo il contatore segnaposto 0 da una parte (utilizzato solo dal costruttore e 
                 * dall'incremento appena si arriva al successivo multi_element) e il contatore 1 dall'altra significa che
                 * e' stata effettuata almeno un'operazione, quindi posso andare a fare un accesso alla memoria, in particolare controllo se
                 * il numero di occorrenze memorizzato nel multi_element e' uguale a 1, se e' cosi' allora i due iteratori risultano uguali,
                 * altrimenti viene restituito false.
                 * 
                 * NUOVO ALTRO CORNER CASE! Se un contatore e' 0 (quindi "punta" all'inizio dell'elemento) e l'altro diverso da zero 
                 * ma l'elemento e' lo stesso (quindi puntano ad occorrenze diverse, in particolare il primo alla prima occorrenza),
                 * decrementando il secondo questo non arrivera' mai ad avere contatore uguale a zero.
                 * Quindi ho rivalutato il controllo che avevo inserito in precedenza, mi serve poter leggere il numero di occorrenze
                 * anche in questo caso, per semplificare utilizzo il copy constructor di const_iterator ed effettuo due incrementi sulle copie,
                 * in questo modo non avro' piu' 0 nel primo contatore e in seguito controllo se ora i due puntatori e contatori sono uguali:
                 * se sono diversi significa che stavo puntando a occorrenze diverse dello stesso elemento. 
                 * 
                 * Questo nuovo controllo permette anche di risolvere il corner case precedente.
                 * 
                 * @param other altro const_iterator con cui verificare l'uguaglianza
                 * @return true se entrambi i dati membro dei due const_iterator sono uguali
                 * @return false se il puntatore a multi_element e' diverso oppure e' diverso il numero di occorrenze rimanenti
                 */
                bool operator==(const const_iterator &other) const {
                    if (_ptr != other._ptr)
                        return false;

                    if (_count == other._count)
                        return true;

                    const_iterator copy_1(*this), copy_2(other);
                    ++copy_1;
                    ++copy_2;
                    return (copy_1._ptr == copy_2._ptr) && (copy_1._count == copy_2._count);
                }
                
                /**
                 * @brief Operatore di diversita' operator!=
                 * 
                 * @param other altro const_iterator con cui verificare la diversita' (NOT dell'uguaglianza)
                 * @return true se operator==(other) e' false
                 * @return false se operator==(other) e' true
                 */
                bool operator!=(const const_iterator &other) const {
                    return !(*this == other);
                }

            private:

                // La classe container deve essere messa friend dell'iteratore per poter
                // usare il costruttore di inizializzazione.
                friend class ordered_multiset;

                /**
                 * @brief Costruttore privato di inizializzazione usato dalla classe container
                 * 
                 * Inizializza il puntatore _ptr con l'argomento ptr e il numero di occorrenze rimanenti a 0 per il motivo spiegato nel pre-incremento.
                 * 
                 * @param ptr puntatore a multi_element
                 * 
                 * @post _ptr == ptr
                 * @post _count == 0
                 */
                const_iterator(const multi_element *ptr) : _ptr(ptr), _count(0) { }

                const multi_element *_ptr; ///< Puntatore a multi_element (struttura interna del multiset ordinato) costante: l'iteratore fa da tramite per i dati contenuti
                size_type _count; ///< Contatore di occorrenze rimanenti verso destra, all'inizio ha valore 0 (spiegazione sul pre-incremento)
                
            }; // classe const_iterator
	
        /**
         * @brief Metodo begin che restituisce l'iteratore all'inizio della sequenza dati (punto 2 nel testo)
         * 
         * Metodo begin che restituisce un const_iterator costruito passando il puntatore all'array di multi_element,
         * corrispondente al puntatore al primo multi_element. Fa riferimento all'inizio della sequenza dati.
         * 
         * @return const_iterator iteratore che si riferisce all'inizio della sequenza dati
         */
        const_iterator begin() const {
            return const_iterator(_elements);
        }
        
        /**
         * @brief Metodo end che restituisce l'iteratore alla fine della sequenza dati (punto 2 nel testo)
         * 
         * Metodo end che restituisce un const_iterator costruito passando il puntatore alla cella successiva 
         * rispetto all'ultimo multi_element nell'array. Fa riferimento alla fine della sequenza dati.
         * 
         * @return const_iterator iteratore che si riferisce alla fine della sequenza dati
         */
        const_iterator end() const {
            return const_iterator(_elements + _array_size);
        }




    private:
        /**
         * @brief Struttura interna che rappresenta una coppia <valore, occorrenze>
         * 
         * Struttura interna fondamentale per minimizzare l'uso della memoria (in quanto il multiset ordinato consiste in un array dinamico di multi_element), 
         * accessibile solo dalla classe ordered_multiset 
         * poiche' non dev'essere possibile esporre una struttura interna all'esterno.
         * 
         * multi_element possiede due dati membro, che ho lasciato con visibilita' public perche' non e' necessario fare information hiding per una struct privata,
         * che rappresentano l'elemento di tipo generico T e il suo numero di occorrenze.
         * 
         * Oltre ai due dati membro ho messo i 4 metodi fondamentali che non devono mai mancare, anche se il compilatore li genera nel caso.
         * 
         */
        struct multi_element {
            value_type _element; ///< Elemento generico di tipo T
            size_type _count; ///< Numero di occorrenze di _element

            /**
             * @brief Costruttore di default per istanziare un multi_element con 0 occorrenze
             * 
             * Si tratta dell'unico costruttore che viene utilizzato nel momento in cui si alloca un array di multi_element,
             * ma nel codice non viene mai utilizzato da solo in quanto porterebbe ad avere uno stato non coerente del multiset ordinato
             * (non ha senso parlare di elementi con 0 occorrenze, infatti nel metodo remove() nel momento in cui le occorrenze diventano 0 si ha una rimozione completa).
             * 
             * Potrebbe generare un'eccezione proveniente dal costruttore di default del tipo generico T, poiche' implicitamente viene richiamato
             * sul dato membro _element per costruire l'elemento di tipo T.
             * 
             * @post _count == 0
             * @post _element == T()
             * 
             * @throw std::exception possibile eccezione proveniente dal tipo generico T
             */
            multi_element() : _count(0) { }

            /**
             * @brief Distruttore di multi_element
             * 
             * In questo caso vuoto perche' non vengono acquisite risorse da un multi_element.
             */
            ~multi_element() { }

            /**
             * @brief Copy constructor di multi_element
             * 
             * Il copy constructor di multi_element nell'initialization list richiama il copy constructor del tipo generico T, per creare una copia dell'elemento, e
             * il (pseudo) copy constructor di size_type, che e' un typedef di unsigned int.
             * 
             * Il copy constructor del tipo generico T potrebbe generare un'eccezione, poiche' non conosciamo ad esempio cio' che eventualmente acquisisce.
             * 
             * @param other altro multi_element da cui copiare i dati
             * 
             * @throw std::exception possibile eccezione proveniente dal tipo generico T
             * 
             * @post _element == other._element
             * @post _count == other._count
             */
            multi_element(const multi_element &other) : _element(other._element), _count(other._count) { }

            /**
             * @brief Operatore di assegnamento di multi_element
             * 
             * Operatore di assegnamento che puo' essere utilizzato nel caso di un multi_element gia' inizializzato, 
             * per effettuare una copia indipendente dei dati con l'argomento other.
             * 
             * La struttura del codice e' la solita dell'operator= e per la stessa motivazione del copy constructor (poiche' se ne fa uso)
             * potremmo avere un'eccezione proveniente dal tipo generico T.
             * 
             * 
             * @param other altro multi_element da cui ricopiare i dati
             * 
             * @throw std::exception possibile eccezione proveniente dal tipo generico T
             * 
             * @return multi_element& istanza corrente del multi_element
             * 
             * @post _element == other._element
             * @post _count == other._count
             */
            multi_element &operator=(const multi_element &other) {
                if (this != &other) {
                    multi_element tmp(other);
                    std::swap(_element, tmp._element);
                    std::swap(_count, tmp._count);
                }
                return *this;
            }
            
        };

        multi_element *_elements; ///< Puntatore all'array dinamico di multi_element
        size_type _array_size; ///< Lunghezza dell'array di multi_element
        Cmp _compare; ///< Istanza del funtore di ordinamento
        Eql _equals; ///< Istanza del funtore di uguaglianza

};

#endif