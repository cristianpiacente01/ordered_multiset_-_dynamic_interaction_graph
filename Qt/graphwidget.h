// 866020 Piacente Cristian c.piacente@campus.unimib.it

#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QWidget>

/* Classe derivata da QWidget, rappresenta un dynamic interaction graph
*/
class GraphWidget : public QWidget
{
    Q_OBJECT

public:
    GraphWidget(QWidget *parent = nullptr); // costruttore
    ~GraphWidget(); // distruttore (vuoto)

    /* Ho lasciato nell'interfaccia pubblica della classe le costanti statiche e
     * i metodi const, in quanto non cambiano lo stato della classe e un potenziale
     * utente non puo' quindi fare danni nel caso volesse utilizzare i metodi di questa classe.
    */

    const static int MAX_NODES = 30; // numero massimo di nodi
    const static int MAX_RANDOM_NODES = 6; // numero massimo di nodi generati ogni secondo
    const static int MAX_EDGES = 4; // numero massimo di archi (e interazioni) generati ogni secondo
    const static int CIRCLE_RADIUS = 7; // raggio di ogni cerchio rappresentante un nodo

    QPoint gen_point() const; // genera e restituisce un punto che non e' vicino a un altro esistente

    bool points_near(const QPoint &p1, const QPoint &p2) const;
    // restituisce true se i due punti sono vicini

    QPoint center_point(const QPoint &p1, const QPoint &p2) const;
    // restituisce il punto medio rispetto ai due centri

    QColor colorize(unsigned int n_interactions) const;
    // come da traccia, ispirata da stackoverflow, restituisce un colore in base al numero di interazioni

private:
    unsigned int nodes_counter; // contatore che mi dice quanti nodi sono stati creati

    unsigned int interactions[MAX_NODES][MAX_NODES];
    /* matrice simmetrica, uso solo la parte inferiore per avere coppie distinte
     * e la diagonale per il numero massimo di interazioni
     * (quest'ultima, per i colori, usata solo per ottimizzare la resize) */

    QPoint nodes[MAX_NODES]; // array di nodi generati

    bool resize_mode; // flag per la gestione di un ridimensionamento

    const QSize original_size; // dimensione originale della finestra


    void T_N(bool T_0); // funzione eseguita ogni secondo, l'argomento e' true se sono al tempo T_0

    void draw(qreal sx = 1.0, qreal sy = 1.0);
    // funzione che disegna tutto, gli argomenti sono la scala rispetto alla dimensione originale

    void delay(); // esegue un delay di 1 secondo


    void add_nodes(unsigned int minimum = 2); // permette l'aggiunta di nodi
    void add_edges(); // aggiunge archi tra nodi senza interazioni
    void add_interactions(); // esegue l'incremento di interazioni, eventualmente implica la generazione di nuovi archi

    void paintEvent(QPaintEvent *event); // override di paintEvent
    void resizeEvent(QResizeEvent *event); // override di resizeEvent

    bool event(QEvent *event); // event invece mi serve per gestire la finestra su sistemi Windows


};
#endif // GRAPHWIDGET_H
