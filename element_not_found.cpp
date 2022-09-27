#include "element_not_found.h"

/**
 * @file element_not_found.cpp
 * @brief Definizione dell'eccezione custom element_not_found.
 * 
 * Questo file contiene la definizione di element_not_found,
 * in particolare la definizione del costruttore che prende un messaggio di errore.
 * 
 * @author 866020 Piacente Cristian
 */

/**
 * Costruttore della classe eccezione custom element_not_found che prende un messaggio di errore.
 * 
 * Questo messaggio di errore, di tipo std::string, viene passato al costruttore della classe base std::invalid_argument tramite l'initialization list,
 * che a sua volta per risalire nella gerarchia sara' passato alla classe std::logic_error e infine a std::exception.
 * 
 * Nel caso in cui venga lanciata un'eccezione si puo' ottenere il messaggio di errore con il metodo what().
 * 
 * La classe element_not_found deriva da std::invalid_argument (che deriva da std::logic_error) e non da std::runtime_error perche'
 * non si tratta di un errore a runtime, in quanto un errore a runtime e' qualcosa che non e' stato possibile prevenire da parte del programmatore,
 * mentre un errore logico avviene nel momento in cui non si rispettano le pre-condizioni di un metodo. 
 * 
 * Infatti, il metodo remove() della classe ordered_multiset ha come pre-condizione multiplicity(element) > 0 e se non si rispetta significa che
 * e' avvenuto un errore logico, in particolare e' stato l'argomento a causarlo quindi invalid_argument.
 * 
 */
element_not_found::element_not_found(const std::string &message) 
: std::invalid_argument(message) { }