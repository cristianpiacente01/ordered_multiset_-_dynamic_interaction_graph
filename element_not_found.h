#ifndef ELEMENT_NOT_FOUND_H
#define ELEMENT_NOT_FOUND_H

#include <stdexcept>
#include <string>

/**
 * @file element_not_found.h
 * @brief Dichiarazione dell'eccezione custom element_not_found.
 * 
 * Questo file contiene la dichiarazione di element_not_found,
 * cioe' l'eccezione custom lanciata nel caso in cui si tenta di rimuovere
 * da un multiset ordinato un elemento non presente.
 * 
 * @author 866020 Piacente Cristian
 */

/**
 * @brief Classe corrispondente all'eccezione custom, che deriva da std::invalid_argument 
 * 
 */
class element_not_found : public std::invalid_argument {
public:
	/**
	 * @brief Costruttore che prende un messaggio di errore di tipo std::string
	 * 
	 * @param message messaggio di errore di tipo std::string
	 */
	element_not_found(const std::string &message);
};

#endif