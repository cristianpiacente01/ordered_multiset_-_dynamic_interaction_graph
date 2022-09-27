// 866020 Piacente Cristian c.piacente@campus.unimib.it

#include "graphwidget.h"
#include <QPainter> // QPainter
#include <QRandomGenerator> // QRandomGenerator
#include <QTime> // QTime
#include <QCoreApplication> // QCoreApplication
#include <algorithm> // std::swap
#include <vector> // std::vector

/* Costruttore di GraphWidget, nell'initialization list oltre a
 * costruire la classe base utilizzando parent vado a inizializzare
 * il contatore di nodi generati a 0, il flag per il ridimensionamento a false
 * e original_size con la dimensione attuale del widget.
*/
GraphWidget::GraphWidget(QWidget *parent)
    : QWidget(parent),
      nodes_counter(0),
      resize_mode(false),
      original_size(size()) {

    setFixedSize(size()); // fisso la finestra, solo per l'inizio (a 30 nodi la sblocco)

    // azzero la matrice
    for (unsigned int i = 0; i < MAX_NODES; ++i)
        for (unsigned int j = 0; j < MAX_NODES; ++j)
            interactions[i][j] = 0;

    /* non e' necessario azzerare l'array di QPoint
     * perche' e' un tipo custom e viene chiamato il ctor di default per ogni cella */
}

GraphWidget::~GraphWidget() { } // distruttore vuoto

/* override del paintEvent, indispensabile per disegnare in quanto di norma
 * e' possibile utilizzare un QPainter solo all'interno di un paintEvent,
 * quindi il mio paintEvent andra' a invocare metodi che disegnano;
 * l'argomento non viene utilizzato quindi per evitare un warning non gli do un nome */
void GraphWidget::paintEvent(QPaintEvent *) {
    /* l'utente ha richiesto un ridimensionamento (possibile solo dopo aver raggiunto 30 nodi)
     * (oppure sto gestendo il focus della finestra, spiegato alla fine) */
    if (resize_mode) {
        /* calcolo la scala come rapporto tra dimensione iniziale della finestra e dimensione attuale,
         * ovviamente dato che ho bisogno di un qreal non posso semplicemente dividere due interi e
         * quindi ho bisogno di fare un cast esplicito prima della divisione */
        qreal sx = static_cast<qreal>(size().width()) / original_size.width();
        qreal sy = static_cast<qreal>(size().height()) / original_size.height();
        draw(sx, sy); // ridisegno tutto con la nuova scala
        resize_mode = false; // fine del ridimensionamento
        return; // esco dal paintEvent
    }

    /* il paintEvent si puo' vedere come se fosse un metodo chiamato ricorsivamente
     * (in realta' viene richiamato il metodo update() che a sua volta scatena un nuovo paintEvent),
     * il caso base della ricorsione e' quando si raggiungono i 30 nodi, cioe' nodes_counter == MAX_NODES */

    if (nodes_counter < MAX_NODES) { // caso passo della ricorsione
        bool T_0 = nodes_counter == 0; // se sono a T_0 ho 0 nodi

        T_N(T_0); // passo il bool a T_N che nel caso esegue T_0
        draw(); // disegno tutto con scala di default 1.0 1.0

        /* se non ho finito aspetto 1 secondo, non faccio la delay a T_0
         * perche' altrimenti non viene disegnato nulla subito */
        if (!T_0 && nodes_counter < MAX_NODES)
            delay();
        /* delay e' l'attesa di 1 secondo
         * durante la quale si eseguono le callback
         * presenti nell'event loop, per processare eventuali eventi in coda */
    }
}

/* generazione di nuovi nodi, il minimo e' specificato come argomento,
 * uso 2 come minimo in T_0 e 0 come minimo in T_N, con N != 0 */
void GraphWidget::add_nodes(unsigned int minimum) {

    unsigned int maximum = MAX_RANDOM_NODES; // uso la costante statica definita nella classe

    if (maximum > MAX_NODES - nodes_counter) // se il max e' maggiore del numero di nodi rimanenti per arrivare a 30
        maximum = MAX_NODES - nodes_counter; // allora riduco il massimo

    unsigned int new_nodes_count = QRandomGenerator::global()->bounded(minimum, maximum + 1);
    // generazione di un numero tra minimum incluso e maximum + 1 escluso

    unsigned int end = nodes_counter + new_nodes_count; // indice a cui non arrivare

    for (unsigned int i = nodes_counter; i < end; ++i) { // parto dall'indice nodes_counter e vado a generare nodi
        nodes[i] = gen_point(); // genero un nuovo nodo e lo salvo nell'array
        ++nodes_counter; // incremento il contatore di nodi generati
    }
}

/* generazione di un nodo non vicino ad altri nodi gia' esistenti */
QPoint GraphWidget::gen_point() const {
    int x_pos, y_pos; // variabili d'appoggio per le coordinate random
    bool ok; // se false allora continua il ciclo
    QPoint res; // cio' che alla fine viene restituito
    do {
        ok = true;
        /* ricordando che le coordinate nella computer grafica hanno l'origine in alto a sinistra...
         * comunque garantisco anche un minimo di bordo */
        x_pos = QRandomGenerator::global()->bounded(CIRCLE_RADIUS + 8, size().width() - CIRCLE_RADIUS - 7);
        y_pos = QRandomGenerator::global()->bounded(CIRCLE_RADIUS + 8, size().height() - CIRCLE_RADIUS - 7);
        // setto le nuove coordinate usando rx() e ry() che restituiscono reference
        res.rx() = x_pos;
        res.ry() = y_pos;
        // controllo che per ogni nodo esistente nessuno sia vicino a quello appena generato
        for (unsigned int i = 0; i < nodes_counter && ok; ++i)
            if (points_near(res, nodes[i])) // se sono vicini setto ok a false e ricomincia il ciclo
                ok = false;
    } while (!ok);
    return res;

}

/* metodo const che restituisce true se i due nodi passati per const ref sono vicini tra loro */
bool GraphWidget::points_near(const QPoint &p1, const QPoint &p2) const {
    float x_diff = (p2.x() - p1.x()) * (p2.x() - p1.x());
    float y_diff = (p2.y() - p1.y()) * (p2.y() - p1.y());
    float distance = sqrt(x_diff + y_diff);

    /* ho un'intersezione se la distanza e' minore del diametro
     * ma uso 6 volte il raggio e non 2 per non rendere vicini i punti,
     * questione di leggibilita' */

    return distance < (6 * CIRCLE_RADIUS);
}

/* metodo che viene invocato ogni secondo, permette la gestione sia di T_0 che di un T_N generale */
void GraphWidget::T_N(bool T_0) {
    if (T_0) { // tempo T_0
        add_nodes(); // semplicemente aggiungo minimo 2 nodi e poi esco dal metodo
        return;
    }

    // qui siamo da T_1 in poi

    add_nodes(0); // minimo 0 nodi
    add_edges(); // minimo 0 archi
    add_interactions(); // minimo 0 nuove interazioni
}

/* metodo che disegna tutto e prende come argomenti la scala per le ascisse e le ordinate */
void GraphWidget::draw(qreal sx, qreal sy) { // sx e sy di default 1.0
    QPainter painter(this); // istanza di QPainter usata per disegnare
    painter.scale(sx, sy); // imposto la scala prima di disegnare

    // utilizzo l'antialiasing per migliorare l'immagine
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    std::vector<unsigned int> done;
    /* mi serve per tenere il numero massimo attuale per ogni nodo, per colorare appropriatamente ogni secondo,
     * perche' nonostante sappia gia' qual e' il massimo per ogni nodo, avendolo salvato nella diagonale, devo
     * colorare ogni secondo in base a quello che ho sul grafo, altrimenti vedrei punti colorati
     * in anticipo in modo diverso rispetto ai relativi archi presenti in quel secondo;
     * la diagonale viene usata invece solo per ottimizzare la gestione del ridimensionamento della finestra,
     * poiche' quando posso ridimensionare la finestra so che e' gia' tutto sul grafo, quindi posso direttamente
     * utilizzare il numero massimo di interazioni per il colore da usare subito per disegnare il nodo */



    for (unsigned int i = 0; i < nodes_counter; ++i) { // per ogni nodo i

        done.push_back(0); // inizializzo il vector riempiendolo con 0

        painter.setPen(QPen(Qt::black, 1.2)); // tratto un pochino piu' pesante, contorno dei nodi
        if (resize_mode)
            painter.setBrush(QBrush(colorize(interactions[i][i]))); // ottimizzazione, subito il colore finale
        else
            painter.setBrush(QBrush(colorize(0))); // all'inizio colore di 0
        painter.drawEllipse(nodes[i], CIRCLE_RADIUS, CIRCLE_RADIUS); // disegno il nodo (cerchio)

        for (unsigned int j = 0; j < i && j < nodes_counter; ++j) // per ogni altro nodo j diverso da i
            if (interactions[i][j] != 0) { // se ci sono interazioni tra i e j
                if (!resize_mode) { // se non sono in modalita' resize
                    if (interactions[i][j] > done[i]) { // nuovo massimo per il nodo i, lo ridisegno
                        painter.setBrush(QBrush(colorize(interactions[i][j])));
                        painter.drawEllipse(nodes[i], CIRCLE_RADIUS, CIRCLE_RADIUS);

                        done[i] = interactions[i][j];
                    }

                    if (interactions[i][j] > done[j]) { // nuovo massimo per il nodo j, lo ridisegno
                        painter.setBrush(QBrush(colorize(interactions[i][j])));
                        painter.drawEllipse(nodes[j], CIRCLE_RADIUS, CIRCLE_RADIUS);

                        done[j] = interactions[i][j];
                    }
                }

                painter.setPen(QPen(Qt::black, 0.25)); // tratto leggero
                painter.drawLine(nodes[i], nodes[j]);

                QString text("+");
                text += QString::number(interactions[i][j]);

                painter.drawText(center_point(nodes[i], nodes[j]), text);
            }

    }

    if (nodes_counter < MAX_NODES)
        update(); // schedula un altro paintEvent, ricorsione
    else { // sblocco la finestra
        setMinimumSize(original_size); // per rendere sempre visibile il grafo
        setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    }

}

/* attesa di 1 secondo */
void GraphWidget::delay() {
    // https://stackoverflow.com/questions/3752742/how-do-i-create-a-pause-wait-function-using-qt
    QTime die_time = QTime::currentTime().addSecs(1); // in alternativa addMSecs(1000)
    while (QTime::currentTime() < die_time)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

/* override del resizeEvent per permettere la gestione del ridimensionamento */
void GraphWidget::resizeEvent(QResizeEvent *) {
    resize_mode = true;
    update(); // scateno un paintEvent
}

/* aggiunta di archi, minimo 0 */
void GraphWidget::add_edges() {
    unsigned int minimum = 0;
    unsigned int maximum = MAX_EDGES;

    unsigned int random_n = QRandomGenerator::global()->bounded(minimum, maximum + 1);

    // random_n non e' il nuovo numero garantito di archi

    unsigned int p1, p2; // indici corrispondenti ai punti per l'interazione

    for (unsigned int i = 0; i < random_n; ++i) {
        p1 = QRandomGenerator::global()->bounded(0, 30); // indice di riga
        do {
            p2 = QRandomGenerator::global()->bounded(0, 30); // indice di colonna
        } while (p2 == p1); // garantita la diversita'

        if (p2 > p1)
            std::swap(p1, p2); // non uso la parte superiore della matrice

        if (interactions[p1][p2] == 0) { // se non hanno archi
            /* se non e' uguale a 0 meglio cosi' e si va alla prossima iterazione del ciclo,
             * si aumenta la leggibilita' perche' non si vanno a creare esattamente random_n
             * archi ma di meno, poiche' random_n non e' il nuovo numero garantito di archi */

            interactions[p1][p2] = 1;

            // aggiorno il colore eventualmente
            if (interactions[p1][p1] == 0)
                interactions[p1][p1] = 1;

            // colore del secondo punto
            if (interactions[p2][p2] == 0)
                interactions[p2][p2] = 1;

        }
    }
}

/* incremento di interazioni, minimo 0 anche qui altrimenti non avrei coerenza col punto precedente,
 * poiche' questo metodo implica la generazione di nuovi archi nel momento in cui scelgo una coppia
 * di nodi senza interazioni, come scritto sul forum */
void GraphWidget::add_interactions() {
    unsigned int minimum = 0;
    unsigned int maximum = MAX_EDGES;

    unsigned int new_interactions_count = QRandomGenerator::global()->bounded(minimum, maximum + 1);

    unsigned int p1, p2;

    for (unsigned int i = 0; i < new_interactions_count; ++i) {
        p1 = QRandomGenerator::global()->bounded(0, 30); // indice di riga
        do {
            p2 = QRandomGenerator::global()->bounded(0, 30); // indice di colonna
        } while (p2 == p1); // garantita la diversita'

        if (p2 > p1)
            std::swap(p1, p2); // non uso la parte superiore della matrice

        ++interactions[p1][p2]; // incremento il numero di interazioni

        // colore
        if (interactions[p1][p1] < interactions[p1][p2])
            interactions[p1][p1] = interactions[p1][p2];

        if (interactions[p2][p2] < interactions[p1][p2])
            interactions[p2][p2] = interactions[p1][p2];
    }
}

/* metodo che restituisce il punto medio, dati due punti corrispondenti a centri di nodi */
QPoint GraphWidget::center_point(const QPoint &p1, const QPoint &p2) const {
    return QPoint((p1.x() + p2.x())/2, (p1.y() + p2.y())/2);
}

/* metodo che permette di avere una colormap: colori diversi in base all'argomento passato */
QColor GraphWidget::colorize(unsigned int n_interactions) const {
    qreal rv = qreal(n_interactions % 9) / 9;
    /* e' improbabile arrivare a 9 interazioni, ma per sicurezza uso il modulo,
     * altrimenti fromHsl potrebbe dare errore */
    return QColor::fromHsl(205 - (205 - 42) * rv, 200, 135);
}

/* override di event, per il motivo spiegato di seguito */
bool GraphWidget::event(QEvent *event) {
    /* sul sistema operativo Windows, poiche' l'ho testato anche su Windows 10 64-bit QtCreator 8.0.0
     *  oltre che sulla macchina virtuale, sembra che venga scatenato un paintEvent ogni volta che
     *  la finestra perde o ottiene il focus, quindi gestisco questa cosa controllando se
     *  nel momento in cui avviene un evento il tipo e' WindowActivate o WindowDeactivate,
     *  se e' cosi' allora scateno una resize, settando il mio flag resize_mode e chiamando update
     *  che scatena un altro paintEvent, questa volta verra' disegnato tutto poiche' viene gestito
     *  uguale a un ridimensionamento; questo meccanismo non e' necessario su Ubuntu invece */

    if (event->type() == QEvent::WindowActivate || event->type() == QEvent::WindowDeactivate) {
        resize_mode = true;
        update();
    }

    return QWidget::event(event);
}
