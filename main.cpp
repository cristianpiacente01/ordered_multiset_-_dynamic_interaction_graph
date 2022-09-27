#include "ordered_multiset.h" // ordered_multiset (, element_not_found, std::runtime_error, std::ostream)
#include <iostream> // std::cout, std::streambuf
#include <string> // std::string
#include <cassert> // assert
#include <cmath> // std::abs, std::fabs
#include <limits> // std::numeric_limits
#include <sstream> // std::stringstream
#include <vector> // std::vector
#include <algorithm> // std::transform, std::is_permutation, std::reverse
#include <fstream> // std::ofstream

/**
 * @file main.cpp
 * @brief File contenente diversi test per la classe ordered_multiset (struttura e dettagli in relazione).
 * 
 * Tipi di dato testati e numero di combinazioni di funtori usati per ognuno:
    - int (6)
    - char (4)
    - float (4)
    - std::string (4)
    - point (2)
    - person (2)
    - fraction (2)
    - ordered_multiset (2)
 * 
 * @author 866020 Piacente Cristian
 */



///////////////////////////////////////////////////////////////////////////
// Tag e macro se si desidera stampare su file al posto della console
///////////////////////////////////////////////////////////////////////////

#define OUTPUT_TO_FILE // commentare questa riga per non stampare su file

/* directory utilizzata se OUTPUT_TO_FILE e' definito, deve esistere gia'
e la stringa deve terminare con uno slash */
#define OUTPUT_DIR "./tests_output/"

///////////////////////////////////////////////////////////////////////////
// Macro utilizzate per la stampa colorata, per console Linux
///////////////////////////////////////////////////////////////////////////

#ifdef __unix__
    #ifndef OUTPUT_TO_FILE
        #define RESET "\033[0m"
        #define RED "\033[31m"
        #define GREEN "\033[32m"
        #define BLUE "\033[34m"
        #define BOLD "\033[1m"
    #endif
#endif

#ifndef RESET
    #define RESET ""
    #define RED ""
    #define GREEN ""
    #define BLUE ""
    #define BOLD ""
#endif

///////////////////////////////////////////////////////////////////////////
// Metodi di comodo
///////////////////////////////////////////////////////////////////////////

int to_Z_4(int a) { // restituisce la conversione nell'equivalente classe di resto in Z_4 (cioe' modulo 4, 0 o 1 o 2 o 3)
    a = a % 4;
    if (a < 0) // l'operator% sugli interi negativi non sembra trasformarli in positivi quindi dopo il modulo eventualmente li incremento di 4
        a += 4;
    return a;
}

void convert_to_upper_str(std::string &str) {
    std::transform(str.begin(), str.end(), str.begin(), ::toupper); // C++11
}

bool equal_strings_ignore_case(const std::string &str1, const std::string &str2) {
    std::string a_tmp(str1);
    std::string b_tmp(str2);
    convert_to_upper_str(a_tmp);
    convert_to_upper_str(b_tmp);
    return a_tmp == b_tmp;
}

std::ostream &print_test_n(unsigned int test_number) {

    std::cout << BOLD << BLUE << "[Test #" << test_number << "] " << RESET;

    return std::cout;
}

std::string colored_bool(bool b) { 
    std::string result;
    result += BOLD;
    if (b) {
        result += GREEN;
        result += "true";
    }
    else {
        result += RED;
        result += "false";
    }
    result += RESET;
    return result;
}

std::string passed() {
    std::string result;
    result += BOLD;
    result += GREEN;
    result += "OK!";
    result += RESET;
    return result;
}

void remove_null_chars(std::string &str) {
    str.erase(std::find(str.begin(), str.end(), char()), str.end());
    /* mi serve rimuovere il carattere null terminator, ossia il char di default char(), per evitare problemi nell'operator== tra std::string,
        infatti "ciao\0" e "ciao" sono stringhe diverse per l'operator== */
}

template <typename T, typename Cmp, typename Eql>
bool check_ostream_op(const ordered_multiset<T, Cmp, Eql> &om, const std::string &expected) {
    std::string expected_str(expected);
    remove_null_chars(expected_str);

    std::stringstream ss;
    ss << om;
    std::string ss_out(ss.str());
    remove_null_chars(ss_out);

    return ss_out == expected_str;
}

template <typename Eql, typename Iterator, typename T>
unsigned int count_element_occs(Iterator begin, Iterator end, const T &element) {
    unsigned int result = 0;
    Eql eql;
    for (; begin != end; ++begin)
        if (eql(element, *begin))
            ++result;
    
    return result;
}

template <typename Eql, typename T>
bool element_is_in_vec(const std::vector<T> &vec, const T &element) {
    return count_element_occs<Eql>(vec.begin(), vec.end(), element) > 0;
}

// pre-condizione dst vuoto
template <typename Compare, typename Equals, typename Iterator, typename T>
void extract_sorted_unique_elements(Iterator begin, Iterator end, std::vector<T> &dst) { // ricrea la logica del multiset ordinato ma solo per la add di nuovi elementi
    Compare cmp;

    typename std::vector<T>::const_iterator begin_tmp, end_tmp;

    unsigned int correct_index;

    for (; begin != end; ++begin) {
        if (!element_is_in_vec<Equals>(dst, *begin)) {
            correct_index = 0;
            begin_tmp = dst.begin();
            end_tmp = dst.end();
            while (begin_tmp != end_tmp && cmp(*begin_tmp, *begin)) {
                ++correct_index;
                ++begin_tmp;
            }
            dst.insert(dst.begin() + correct_index, *begin);
        }
    }
}

template <typename Compare, typename Equals, typename T>
std::string vector_to_ordered_str(const std::vector<T> &vec, bool not_reverse = true) { // nota: la stringa finisce con uno spazio
    std::stringstream ss;

    std::vector<T> distinct_elements;
    
    extract_sorted_unique_elements<Compare, Equals>(vec.begin(), vec.end(), distinct_elements);
    if (!not_reverse) // capovolgo il vector
        std::reverse(distinct_elements.begin(), distinct_elements.end());
        

    for (typename std::vector<T>::size_type i = 0; i < distinct_elements.size(); ++i) {
        unsigned int count = count_element_occs<Equals>(vec.begin(), vec.end(), distinct_elements[i]);
        for (unsigned int j = 0; j < count; ++j)
            ss << distinct_elements[i] << " ";
    }

    return ss.str();
}

// pre-condizione dst vector vuoto
template <typename T>
void vec_from_indexes(const std::vector<T> &src, std::vector<T> &dst, bool even = true) { // di default uso gli indici pari, passando false quello dispari
    // riempie dst da src gia' esistente, se even == true allora usa gli indici pari es. 0 2 4 ... altrimenti quelli dispari 1 3 5 ... 
    typename std::vector<T>::size_type mod2 = even ? 0 : 1; // operatore ternario, se even allora un indice modulo 2 dev'essere uguale a 0 altrimenti 1 

    for (typename std::vector<T>::size_type i = 0; i < src.size(); ++i)
        if (i % 2 == mod2)
            dst.push_back(src[i]);

}

template <typename Compare, typename Equals, typename T>
std::string vec_to_multiset_str(const std::vector<T> &values) { // FONDAMENTALE PER I TEST, cosi' non devo passare un sacco di stringhe come argomenti
    if (values.begin() == values.end()) // size 0
        return "{}";

    // non utilizzo direttamente un multiset perche' devo testare che l'operator<< del multiset restituisca la stessa stringa di questo metodo e non avrebbe senso fare la stessa cosa
    std::string result = "{";
    std::stringstream ss; // per le coppie

    std::vector<T> unique_sorted_values;
    extract_sorted_unique_elements<Compare, Equals>(values.begin(), values.end(), unique_sorted_values);


    for (typename std::vector<T>::size_type i = 0; i < unique_sorted_values.size(); ++i) {
        ss << "<" << unique_sorted_values[i] << ", ";
        ss << count_element_occs<Equals>(values.begin(), values.end(), unique_sorted_values[i]);
        ss << ">";
        if (i != unique_sorted_values.size() - 1)
            ss << ", ";
    }

    std::string pairs = ss.str();

    result += pairs;
    result += "}";

    return result;
}

// pre-condizione vec vector vuoto
template <typename Size_type, typename T, typename Cmp, typename Eql>
void multiset_to_vec_of_mult(const ordered_multiset<T, Cmp, Eql> &om, std::vector<Size_type> &vec) { 
    // inserisce le cardinalita' per ogni elemento distinto in vec, ma senza usare il metodo multiplicity perche' non avrebbe senso per i test
    if (om.begin() == om.end()) return; // vuoto

    typename ordered_multiset<T, Cmp, Eql>::const_iterator begin = om.begin(), end = om.end();

    Eql eql;

    T last_val = *begin;
    Size_type occs = 1;
    ++begin;
    for (; begin != end; ++begin) {
        if (eql(*begin, last_val))
            ++occs;
        else {
            vec.push_back(occs);
            last_val = *begin;
            occs = 1;
        }
    }

    vec.push_back(occs);
}

// pre-condizione: val e' contenuto in vec
template <typename Equals, typename Type>
void remove_val_in_vec(std::vector<Type> &vec, const Type &val) { // rimuove l'ULTIMA occorrenza di val da vec (motivazione: problema con test di Z_4)
    Equals eql;

    typename std::vector<Type>::const_iterator begin = vec.end(), end = vec.begin();
    --begin;
    --end;

    for (; begin != end; --begin)
        if (eql(*begin, val)) {
            vec.erase(begin);
            return;
        }
}

// utilizzata per preparare l'eventuale stampa su file
// pre-condizione: out non ancora aperto
std::streambuf *prepare_output(const std::string &type_str, std::ofstream &out) { 
    // restituisce un puntatore al buffer di std::cout "originale" che servira' per l'eventuale ripristino
    #ifdef OUTPUT_TO_FILE
        std::string path = OUTPUT_DIR;
        path += type_str;
        path += "_tests.txt";

        out.open(path);
        if (out.is_open()) {
            std::streambuf *coutbuf = std::cout.rdbuf();
            std::cout.rdbuf(out.rdbuf());
            return coutbuf;
        }
    #endif

    return nullptr;
}

///////////////////////////////////////////////////////////////////////////
// Struct che impacchetta 4 argomenti per le chiamate ai test
///////////////////////////////////////////////////////////////////////////

template <typename T>
struct elements_related_args {
    T element_with_1_occ; // elemento con esattamente 1 occorrenza all'interno del primo vector
    T element_with_1_or_more_occs; // elemento con ALMENO 1 occorrenza
    T element_with_0_occs; // elemento NUOVO IN ENTRAMBI I VETTORI PASSATI COME ARGOMENTO A EXEC_UNIT_TEST

    unsigned int number_of_add; // numero di add da usare per l'elemento di default nei test

    void update(const T &one_occ, const T &one_or_more_occs, const T &zero_occs, unsigned int n_add) {
        element_with_1_occ = one_occ;
        element_with_1_or_more_occs = one_or_more_occs;
        element_with_0_occs = zero_occs;
        number_of_add = n_add;
    }
};

///////////////////////////////////////////////////////////////////////////
// IMPORTANTE: funzione templata che esegue un unit test
///////////////////////////////////////////////////////////////////////////


template <typename Compare, typename CompareOther, typename Equals, typename Type> // type a destra perche' si puo' dedurre
void exec_unit_test(unsigned int test_number /* numero del test */, const std::string &test_title, /* titolo del test*/
                    const std::vector<Type> &values_to_add, /* altro vector */
                    const elements_related_args<Type> &four_args, /* sopra nella definizione della struct vi e' una breve descrizione dei dati membro */
                    const std::vector<Type> &values_to_add4) {
                    /* altro vector, chiamato 4 perche' il secondo e terzo sono generati usando gli elementi in posizione pari e dispari del primo vector */

    Type val_1_occ = four_args.element_with_1_occ;
    Type val_2_occ = four_args.element_with_1_or_more_occs;
    Type new_val = four_args.element_with_0_occs;

    unsigned int n_times_add = four_args.number_of_add;

    typedef ordered_multiset<Type, Compare, Equals> multiset;
    typedef ordered_multiset<Type, CompareOther, Equals> multiset_other;

    typename multiset::value_type default_value = typename multiset::value_type();

    std::vector<Type> values_to_add2;
    vec_from_indexes(values_to_add, values_to_add2); // values_to_add2 riempito usando gli indici pari di values_to_add

    std::vector<Type> values_to_add3;
    vec_from_indexes(values_to_add, values_to_add3, false); // indici dispari di values_to_add
    
    Equals eql;

    std::stringstream ss;

    print_test_n(test_number) << BOLD << "Test " << test_number << RESET << ": " << test_title << std::endl << std::endl;

    print_test_n(test_number) << BLUE << "\tMetodi fondamentali" << RESET << std::endl;

    print_test_n(test_number) << "\tInizio costruttore di default" << std::endl;

    multiset om1;

    print_test_n(test_number) << "\t\tControllo della size uguale a 0... ";
    bool check = om1.size() == 0;
    std::cout << "Risultato: " << om1.size() << " ";
    assert(check);
    std::cout << passed() << std::endl;

    print_test_n(test_number) << "\t\tControllo che non sia contenuto nemmeno l'elemento " << default_value << "... ";
    check = !om1.contains(default_value) && om1.multiplicity(default_value) == 0;
    std::cout << "Risultato: " << colored_bool(check) << " ";
    assert(check);
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\t\tControllo l'operator== con un altro multiset vuoto dello stesso tipo... ";
    check = om1 == multiset();
    std::cout << "Risultato: " << colored_bool(check) << " ";
    assert(check);
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\t\tStampo il contenuto vuoto... ";
    check = check_ostream_op(om1, "{}");
    std::cout << "Risultato: " << om1 << " ";
    assert(check);
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\tFine costruttore di default" << std::endl << std::endl;



    print_test_n(test_number) << "\tInizio distruttore" << std::endl;

    print_test_n(test_number) << "\t\tCreazione oggetto allocato sullo heap e puntatore... ";
    multiset *om_ptr = new multiset;
    assert(om_ptr != nullptr);
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\t\tEseguo una delete... ";
    delete om_ptr;
    om_ptr = nullptr;
    assert(om_ptr == nullptr);
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\tFine distruttore" << std::endl << std::endl;



    print_test_n(test_number) << "\tInizio copy constructor" << std::endl;

    print_test_n(test_number) << "\t\tInizialmente aggiungo qualche elemento al primo multiset creato col ctor di default:" << std::endl;

    // argomento values_to_add
    
    typename multiset::size_type old_size;

    for (const typename multiset::value_type &val : values_to_add) { // "foreach", dato che possiamo usare costrutti C++11 nei test
        // in alternativa uso il const_iterator di std::vector o l'operator[] piu' avanti
        print_test_n(test_number) << "\t\t\tAggiungo l'elemento " << BOLD << val << RESET << "... ";
        old_size = om1.size();
        om1.add(val);
        check = om1.contains(val) && om1.size() == old_size + 1;
        assert(check);
        std::cout << passed() << std::endl; 
    }

    std::string expected_str1 = vec_to_multiset_str<Compare, Equals>(values_to_add);
    print_test_n(test_number) << "\t\tControllo che gli elementi siano stati inseriti nell'ordine corretto... ";
    check = check_ostream_op(om1, expected_str1) && om1.size() == values_to_add.size();
    std::cout << "Risultato: " << colored_bool(check) << " ";
    assert(check);
    std::cout << passed() << std::endl;

    print_test_n(test_number) << "\t\tStampo il nuovo contenuto del multiset... ";
    std::cout << "Risultato: " << om1 << " ";
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\t\tOra creo una copia indipendente dei dati utilizzando il cctor... ";
    multiset om2(om1);
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\t\tStampo il multiset appena creato... ";
    check = check_ostream_op(om2, expected_str1);
    std::cout << "Risultato: " << om2 << " ";
    assert(check);
    std::cout << passed() << std::endl;

    print_test_n(test_number) << "\t\tControllo con l'operator== i due multiset... ";
    check = om1 == om2;
    std::cout << "Risultato: " << colored_bool(check) << " ";
    assert(check);
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\t\tChiamo la clear sul primo multiset e poi lo stampo... ";
    om1.clear();
    check = om1.size() == 0 && check_ostream_op(om1, "{}");
    std::cout << "Risultato: " << om1 << " ";
    assert(check);
    std::cout << passed() << std::endl;

    print_test_n(test_number) << "\t\tStampo il secondo multiset verificando l'indipendenza dei dati... ";
    check = check_ostream_op(om2, expected_str1) && om2.size() == values_to_add.size();
    std::cout << "Risultato: " << om2 << " ";
    assert(check);
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\tFine copy constructor" << std::endl << std::endl;



    print_test_n(test_number) << "\tInizio operatore di assegnamento" << std::endl;

    print_test_n(test_number) << "\t\tUtilizzo l'operator= per assegnare al primo multiset un multiset vuoto creato al volo... ";
    om1 = multiset();
    check = check_ostream_op(om1, "{}");
    assert(check);
    std::cout << passed() << std::endl;

    print_test_n(test_number) << "\t\tUtilizzo l'operator= per assegnare al primo multiset i dati del secondo... ";
    om1 = om2;
    std::cout << passed() << std::endl;

    print_test_n(test_number) << "\t\tUtilizzo l'operator= per assegnare al primo multiset se' stesso (auto-assegnamento)... ";
    om1 = om1;
    check = check_ostream_op(om1, expected_str1) && om1 == om2;
    assert(check);
    std::cout << passed() << std::endl;

    print_test_n(test_number) << "\t\tStampo il primo multiset... ";
    std::cout << "Risultato: " << om1 << " ";
    std::cout << passed() << std::endl; 

    // argomento val_1_occ
    print_test_n(test_number) << "\t\tRimuovo l'unica occorrenza dell'elemento " << val_1_occ << " dal secondo multiset per poi verificare l'indipendenza... ";
    om2.remove(val_1_occ);
    check = !om2.contains(val_1_occ);
    assert(check);
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\t\tStampo il primo multiset... ";
    check = check_ostream_op(om1, expected_str1) && om1.size() == values_to_add.size();
    std::cout << "Risultato: " << om1 << " ";
    assert(check);
    std::cout << passed() << std::endl;

    std::vector<Type> tmp_utility_vec(values_to_add);
    // argomento val_1_occ
    remove_val_in_vec<Equals>(tmp_utility_vec, val_1_occ);

    std::string expected_str2 = vec_to_multiset_str<Compare, Equals>(tmp_utility_vec);
    print_test_n(test_number) << "\t\tStampo il secondo multiset... ";
    check = check_ostream_op(om2, expected_str2) && om2.size() == om1.size() - 1;
    std::cout << "Risultato: " << om2 << " ";
    assert(check);
    std::cout << passed() << std::endl; 

    assert(!(om1 == om2));

    std::vector<Type> tmp_utility_vec2(values_to_add);
    // argomento val_2_occ
    remove_val_in_vec<Equals>(tmp_utility_vec2, val_2_occ);
    print_test_n(test_number) << "\t\tRimuovo un'occorrenza dell'elemento " << val_2_occ << " dal primo multiset per poi verificare l'indipendenza... ";
    typename multiset::size_type old_multiplicity = om1.multiplicity(val_2_occ);
    om1.remove(val_2_occ);
    check = om1.multiplicity(val_2_occ) == old_multiplicity - 1;
    assert(check);
    std::cout << passed() << std::endl; 

    std::string expected_str3 = vec_to_multiset_str<Compare, Equals>(tmp_utility_vec2);    

    print_test_n(test_number) << "\t\tStampo il primo multiset... ";
    check = check_ostream_op(om1, expected_str3) && om1.size() == values_to_add.size() - 1;
    std::cout << "Risultato: " << om1 << " ";
    assert(check);
    std::cout << passed() << std::endl;

    print_test_n(test_number) << "\t\tStampo il secondo multiset... ";
    check = check_ostream_op(om2, expected_str2) && om2.size() == om1.size();
    std::cout << "Risultato: " << om2 << " ";
    assert(check);
    std::cout << passed() << std::endl; 

    assert(!(om1 == om2) || (om1.size() == 0 && om2.size() == 0)); // potrebbero essere uguali se il primo vettore (values_to_add) aveva solo un elemento

    print_test_n(test_number) << "\tFine operatore di assegnamento" << std::endl;

    print_test_n(test_number) << BLUE << "\tFine metodi fondamentali" << RESET << std::endl << std::endl;

    

    print_test_n(test_number) << BLUE << "\tInizio test interfaccia pubblica" << RESET << std::endl;

    print_test_n(test_number) << "\tInizio costruttore che prende una coppia di iteratori" << std::endl;

    print_test_n(test_number) << "\t\tGli elementi che saranno aggiunti sono ";
    for (typename std::vector<Type>::const_iterator begin = values_to_add2.begin(), end = values_to_add2.end(); begin != end; ++begin)
        std::cout << *begin << " ";
    std::cout << std::endl;

    std::string expected_str4 = vec_to_multiset_str<Compare, Equals>(values_to_add2);
    print_test_n(test_number) << "\t\tCreazione multiset usando due iteratori... ";
    multiset om3(values_to_add2.begin(), values_to_add2.end());
    check = check_ostream_op(om3, expected_str4) && om3.size() == values_to_add2.size();
    std::cout << "Risultato: " << om3 << " ";
    assert(check);
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\t\tCreazione multiset usando due iteratori uguali, quindi che risultera' vuoto... ";
    multiset om4(values_to_add2.begin(), values_to_add2.begin());
    check = check_ostream_op(om4, "{}") && om4.size() == 0;
    std::cout << "Risultato: " << om4 << " ";
    assert(check);
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\tFine costruttore che prende una coppia di iteratori" << std::endl << std::endl;



    print_test_n(test_number) << "\tInizio metodo clear" << std::endl;

    print_test_n(test_number) << "\t\tUtilizzo la clear su un multiset non vuoto, ossia quello costruito con due iteratori" << std::endl;

    print_test_n(test_number) << "\t\tPrima della clear: " << om3 << std::endl;

    print_test_n(test_number) << "\t\tDopo la clear: ";
    om3.clear();
    check = check_ostream_op(om3, "{}") && om3.size() == 0;
    std::cout << om3 << " ";
    assert(check);
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\t\tUtilizzo la clear su un multiset vuoto, ossia quello costruito con due iteratori uguali" << std::endl;

    print_test_n(test_number) << "\t\tPrima della clear: " << om4 << std::endl;

    print_test_n(test_number) << "\t\tDopo la clear: ";
    om4.clear();
    check = check_ostream_op(om4, "{}") && om4.size() == 0;
    std::cout << om4 << " ";
    assert(check);
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\tFine metodo clear" << std::endl << std::endl;



    print_test_n(test_number) << "\tInizio metodo swap" << std::endl;

    print_test_n(test_number) << "\t\tRiutilizzo due multiset creati in precedenza qui sotto stampati:" << std::endl;

    check = check_ostream_op(om1, expected_str3);
    print_test_n(test_number) << "\t\tPrimo multiset: " << om1 << std::endl;
    assert(check);

    check = check_ostream_op(om2, expected_str2);
    print_test_n(test_number) << "\t\tSecondo multiset: " << om2 << std::endl;
    assert(check);    

    print_test_n(test_number) << "\t\tChiamo la swap... ";
    typename multiset::size_type old_size1 = om1.size();
    typename multiset::size_type old_size2 = om2.size();
    om1.swap(om2);
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\t\tControllo che i due contenuti siano stati scambiati... ";
    check = check_ostream_op(om1, expected_str2) 
         && check_ostream_op(om2, expected_str3) 
         && om1.size() == old_size2 && om2.size() == old_size1 && (!(om1 == om2) || (om1.size() == 0 && om2.size() == 0));
        // l'ultima condizione messa in or serve se il primo vettore passato ha solo un elemento
    assert(check);
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\t\tStampo i due multiset dopo la swap qui sotto:" << std::endl;

    print_test_n(test_number) << "\t\tPrimo multiset: " << om1 << std::endl;

    print_test_n(test_number) << "\t\tSecondo multiset: " << om2 << std::endl;

    print_test_n(test_number) << "\t\tOra scambio il primo multiset con un multiset vuoto creato in precedenza... ";
    om1.swap(om4);
    check = check_ostream_op(om1, "{}") && om1.size() == 0 && check_ostream_op(om4, expected_str2);
    assert(check);
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\t\tStampo qui sotto i due multiset:" << std::endl;

    print_test_n(test_number) << "\t\tMultiset che era vuoto: " << om4 << std::endl;

    print_test_n(test_number) << "\t\tMultiset che era riempito: " << om1 << std::endl;

    print_test_n(test_number) << "\t\tLi riporto allo stato originale con un'altra swap... ";
    om1.swap(om4);
    check = check_ostream_op(om4, "{}") && om4.size() == 0 && check_ostream_op(om1, expected_str2);
    assert(check);
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\t\tMultiset di nuovo vuoto: " << om4 << std::endl;

    print_test_n(test_number) << "\t\tMultiset di nuovo riempito: " << om1 << std::endl;

    print_test_n(test_number) << "\t\tInfine scambio due multiset vuoti tra di loro e li stampo... ";
    om4.swap(om3);
    check = check_ostream_op(om3, "{}") && om3.size() == 0 && check_ostream_op(om4, "{}") && om4.size() == 0 && om3 == om4;
    std::cout << om3 << " " << om4 << " ";
    assert(check);
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\tFine metodo swap" << std::endl << std::endl;



    print_test_n(test_number) << "\tInizio metodo size" << std::endl;

    print_test_n(test_number) << "\t\tChiamo la size su un multiset vuoto, mi aspetto " << 0 << "... ";

    check = om3.size() == 0;
    std::cout << "Risultato: " << om3.size() << " ";
    assert(check);
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\t\tChiamo la size sul multiset utilizzato in precedenza che stampo qui sotto:" << std::endl;

    print_test_n(test_number) << "\t\tMultiset: " << om1 << std::endl;

    print_test_n(test_number) << "\t\tMi aspetto " << old_size2 << "... ";

    check = om1.size() == old_size2;
    std::cout << "Risultato: " << om1.size() << " ";
    assert(check);
    std::cout << passed() << std::endl; 

    typename multiset::size_type om1_size = om1.size();

    // argomento new_val
    print_test_n(test_number) << "\t\tAggiungo l'elemento " << new_val << " e verifico che la size venga incrementata, mi aspetto " << (om1_size + 1) << "... ";

    om1.add(new_val);
    check = om1.size() == om1_size + 1;
    std::cout << "Risultato: " << om1.size() << " ";
    assert(check);
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\t\tRimuovo l'elemento appena aggiunto e verifico che la size venga decrementata, mi aspetto " << om1_size << "... ";

    om1.remove(new_val);
    check = om1.size() == om1_size;
    std::cout << "Risultato: " << om1.size() << " ";
    assert(check);
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\tFine metodo size" << std::endl << std::endl;



    print_test_n(test_number) << "\tInizio metodo add" << std::endl;

    print_test_n(test_number) << "\t\tUtilizzo un multiset vuoto: " << om4 << std::endl; 

    // argomento n_times_add
    print_test_n(test_number) << "\t\tAggiungo " << n_times_add << " volte l'elemento " << default_value << "... ";

    std::vector<Type> tmp_utility_vec3;

    for (typename multiset::size_type i = 0; i < n_times_add; ++i) {
        om4.add(default_value);
        tmp_utility_vec3.push_back(default_value);
    }

    std::cout << passed() << std::endl; 
    
    ss << "{<" << default_value << ", " << n_times_add << ">}";

    std::string str_expected(ss.str());

    ss.str("");
    ss.clear();

    print_test_n(test_number) << "\t\tVerifico la correttezza del contenuto, mi aspetto " << str_expected << "... ";

    check = check_ostream_op(om4, str_expected) && om4.size() == n_times_add;
    std::cout << "Risultato: " << om4 << " ";
    assert(check);
    std::cout << passed() << std::endl;

    typename multiset::size_type count_default_values = 0;

    print_test_n(test_number) << "\t\tAggiungo altri elementi:" << std::endl;
    for (typename std::vector<Type>::size_type i = 0; i < values_to_add3.size(); ++i) {
        tmp_utility_vec3.push_back(values_to_add3[i]);
        print_test_n(test_number) << "\t\t\tAggiungo l'elemento " << BOLD << values_to_add3[i] << RESET << "... ";
        typename multiset::size_type old_size = om4.size();
        typename multiset::size_type old_mult = om4.multiplicity(values_to_add3[i]);
        if (eql(values_to_add3[i], default_value))
            ++count_default_values;
        om4.add(values_to_add3[i]);
        check = om4.contains(values_to_add3[i]) && om4.size() == old_size + 1 && om4.multiplicity(values_to_add3[i]) == old_mult + 1;
        assert(check);
        std::cout << passed() << std::endl; 
    }

    std::string expected_str5 = vec_to_multiset_str<Compare, Equals>(tmp_utility_vec3);

    print_test_n(test_number) << "\t\tControllo che gli elementi siano stati inseriti nell'ordine corretto... ";
    check = check_ostream_op(om4, expected_str5) && om4.size() == values_to_add3.size() + n_times_add;
    std::cout << "Risultato: " << colored_bool(check) << " ";
    assert(check);
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\t\tStampo il nuovo contenuto del multiset qui sotto:" << std::endl;
    print_test_n(test_number) << "\t\t" << om4 << " ";
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\tFine metodo add" << std::endl << std::endl;



    print_test_n(test_number) << "\tInizio metodo remove" << std::endl;

    print_test_n(test_number) << "\t\tUtilizzo lo stesso multiset di prima, rimuovo " << (n_times_add + count_default_values - 1) << " occorrenze di " << default_value << "... ";

    for (typename multiset::size_type i = 0; i < (n_times_add + count_default_values - 1); ++i) {
        om4.remove(default_value);
        remove_val_in_vec<Equals>(tmp_utility_vec3, default_value);
    }

    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\t\tStampo il nuovo contenuto del multiset qui sotto:" << std::endl;
    print_test_n(test_number) << "\t\t" << om4 << " ";
    std::cout << passed() << std::endl; 

    std::string expected_str6 = vec_to_multiset_str<Compare, Equals>(tmp_utility_vec3);

    print_test_n(test_number) << "\t\tVerifico che ci sia una sola occorrenza di " << default_value << "... ";
    check = om4.multiplicity(default_value) == 1 && check_ostream_op(om4, expected_str6);
    std::cout << "Risultato: " << colored_bool(check) << " ";
    assert(check);
    std::cout << passed() << std::endl; 
    
    remove_val_in_vec<Equals>(tmp_utility_vec3, default_value);
    std::string expected_str7 = vec_to_multiset_str<Compare, Equals>(tmp_utility_vec3);

    print_test_n(test_number) << "\t\tElimino l'ultima occorrenza di " << default_value << "... ";
    om4.remove(default_value);
    check = om4.multiplicity(default_value) == 0 && check_ostream_op(om4, expected_str7);
    assert(check);
    std::cout << passed() << std::endl;

    print_test_n(test_number) << "\t\tStampo il nuovo contenuto qui sotto:" << std::endl;
    print_test_n(test_number) << "\t\t" << om4 << " ";
    std::cout << passed() << std::endl;

    print_test_n(test_number) << "\t\t(Mi aspetto l'eccezione custom) Provo ad eliminare l'elemento " << new_val << " non presente:" << std::endl;

    try {
        om4.remove(new_val);
        throw std::runtime_error("Errore: non si dovrebbe arrivare a questo punto poiche' la remove deve fallire.");
    } catch (const element_not_found &e) {
        print_test_n(test_number) << "\t\t" << BOLD << GREEN << "Eccezione custom catturata: " << RESET << e.what() << std::endl;
    }

    print_test_n(test_number) << "\tFine metodo remove" << std::endl << std::endl;



    print_test_n(test_number) << "\tInizio metodo multiplicity" << std::endl;

    print_test_n(test_number) << "\t\tUso l'ultimo multiset utilizzato, controllo che per ogni elemento il metodo multiplicity restituisca il numero di occorrenze corretto:" << std::endl;

    std::vector<Type> elements_in_order;
    extract_sorted_unique_elements<Compare, Equals>(om4.begin(), om4.end(), elements_in_order);

    std::vector<typename multiset::size_type> elements_in_order_mult;
    multiset_to_vec_of_mult(om4, elements_in_order_mult);
    
    for (typename std::vector<Type>::size_type i = 0; i < elements_in_order.size(); ++i) {
        print_test_n(test_number) << "\t\t\tMolteplicita' dell'elemento " << elements_in_order[i] << " aspettata " << elements_in_order_mult[i] << ", ho " << om4.multiplicity(elements_in_order[i]) << " ";
        check = om4.multiplicity(elements_in_order[i]) == elements_in_order_mult[i];
        assert(check);
        std::cout << passed() << std::endl; 
    }

    print_test_n(test_number) << "\t\tMolteplicita' dell'elemento " << default_value << " non presente... ";
    check = om4.multiplicity(default_value) == 0;
    std::cout << "Risultato: " << om4.multiplicity(default_value) << " ";
    assert(check);
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\tFine metodo multiplicity" << std::endl << std::endl;



    print_test_n(test_number) << "\tInizio operator==" << std::endl;

    print_test_n(test_number) << "\t\tA partire dall'ultimo multiset utilizzato creo un altro multiset ma con policy di ordinamento diversa:" << std::endl;

    print_test_n(test_number) << "\t\t\tStampo il multiset che vado ad utilizzare:" << std::endl;

    print_test_n(test_number) << "\t\t\t" << om4 << " ";
    check = check_ostream_op(om4, expected_str7);
    assert(check);
    std::cout << passed() << std::endl;

    print_test_n(test_number) << "\t\t\tCreo un nuovo multiset con ordinamento diverso, utilizzando il costruttore che prende due iteratori... ";

    multiset_other om4_diff_order(om4.begin(), om4.end());

    tmp_utility_vec3.clear(); // per evitare problemi di ordinamento nei test
    for (typename multiset::const_iterator begin = om4.begin(), end = om4.end(); begin != end; ++begin)
        tmp_utility_vec3.push_back(*begin); 

    std::string expected_str8 = vec_to_multiset_str<CompareOther, Equals>(tmp_utility_vec3);
    check = check_ostream_op(om4_diff_order, expected_str8);
    assert(check);
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\t\tNuovo multiset con ordinamento diverso creato, lo stampo qui sotto:" << std::endl;

    print_test_n(test_number) << "\t\t" << om4_diff_order << std::endl;

    print_test_n(test_number) << "\t\tUtilizzo l'operator== tra i due multiset ordinati in modo diverso... ";

    check = om4 == om4_diff_order;
    std::cout << "Risultato: " << colored_bool(check) << " ";
    assert(check);
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\t\tAggiungo l'elemento " << default_value << " al nuovo multiset e verifico che siano diversi... ";

    om4_diff_order.add(default_value);
    tmp_utility_vec3.push_back(default_value);

    std::string expected_str9 = vec_to_multiset_str<CompareOther, Equals>(tmp_utility_vec3);
    check = !(om4 == om4_diff_order)
        && check_ostream_op(om4_diff_order, expected_str9);
    assert(check);
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\t\tStampo il nuovo multiset qui sotto:" << std::endl;

    print_test_n(test_number) << "\t\t" << om4_diff_order << std::endl;

    print_test_n(test_number) << "\t\tSvuoto quest'ultimo con la clear... ";

    om4_diff_order.clear();

    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\t\tStampo il multiset... ";
    check = check_ostream_op(om4_diff_order, "{}");
    std::cout << "Risultato: " << om4_diff_order << " ";
    assert(check);
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\t\tVerifico l'uguaglianza con un multiset vuoto di ordinamento uguale... ";

    multiset_other other_empty;
    check = om4_diff_order == other_empty;
    std::cout << "Risultato: " << colored_bool(check) << " ";
    assert(check);
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\t\tVerifico l'uguaglianza con un multiset vuoto di ordinamento diverso... ";

    multiset multiset_empty;
    check = om4_diff_order == multiset_empty;
    std::cout << "Risultato: " << colored_bool(check) << " ";
    assert(check);
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\t\tVerifico l'uguaglianza tra due multiset vuoti di ordinamento uguale... ";

    multiset_other other_empty2(other_empty); // giusto per usare anche il cctor
    check = other_empty == multiset_other() && other_empty == other_empty2 && other_empty2.size() == 0 && other_empty.size() == 0;
    std::cout << "Risultato: " << colored_bool(check) << " ";
    assert(check);
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\t\tVerifico l'uguaglianza tra due multiset vuoti di ordinamento diverso... ";

    check = other_empty == multiset_empty && multiset_empty == om4_diff_order;
    std::cout << "Risultato: " << colored_bool(check) << " ";
    assert(check);
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\tFine operator==" << std::endl << std::endl;



    print_test_n(test_number) << "\tInizio metodo contains" << std::endl;

    print_test_n(test_number) << "\t\tCreo un nuovo multiset con i seguenti valori: ";

    // argomento values_to_add4
    
    for (typename std::vector<Type>::const_iterator begin = values_to_add4.begin(), end = values_to_add4.end(); begin != end; ++begin)
        std::cout << *begin << " ";
    std::cout << std::endl;

    multiset om5(values_to_add4.begin(), values_to_add4.end());

    typename std::vector<Type> unique_values;

    extract_sorted_unique_elements<Compare, Equals>(om5.begin(), om5.end(), unique_values);

    print_test_n(test_number) << "\t\tStampo il nuovo contenuto qui sotto:" << std::endl;
    print_test_n(test_number) << "\t\t" << om5 << " ";
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\t\tControllo che la contains dia true per ogni elemento distinto del multiset:" << std::endl;

    for (typename std::vector<Type>::size_type i = 0; i < unique_values.size(); ++i) {
        print_test_n(test_number) << "\t\t\tContains dell'elemento " << unique_values[i] <<  "... ";
        check = om5.contains(unique_values[i]);
        std::cout << "Risultato: " << colored_bool(check) << " ";
        assert(check);
        std::cout << passed() << std::endl; 
    }

    print_test_n(test_number) << "\t\tControllo che la contains dia false per un elemento non contenuto:" << std::endl;

    print_test_n(test_number) << "\t\t\tContains dell'elemento " << new_val << "... ";
    check = !om5.contains(new_val);
    std::cout << "Risultato: " << colored_bool(!check) << " ";
    assert(check);
    std::cout << passed() << std::endl; 


    print_test_n(test_number) << "\tFine metodo contains" << std::endl << std::endl;



    print_test_n(test_number) << "\tInizio stampa con iteratori" << std::endl;

    print_test_n(test_number) << "\t\tUso l'ultimo multiset utilizzato, stampo gli elementi in ordine del multiset ordinato tramite gli iteratori:" << std::endl;

    print_test_n(test_number) << "\t\t";

    typename std::vector<Type> values_from_iterator;

    for (typename multiset::const_iterator begin = om5.begin(), end = om5.end(); begin != end; ++begin) {
        ss << *begin << " ";
        values_from_iterator.push_back(*begin);
    }

    std::string output_str = ss.str();

    ss.str("");
    ss.clear();

    std::cout << output_str << " ";

    std::string expected_iterator_str = vector_to_ordered_str<Compare, Equals>(values_to_add4);
    check = output_str == expected_iterator_str;
    assert(check);
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\t\tOra di nuovo ma stampo partendo dalla fine, per utilizzare il decremento nell'iteratore bidirectional:" << std::endl;

    print_test_n(test_number) << "\t\t";

    values_from_iterator.clear();

    for (typename multiset::const_iterator begin = (--om5.end()), end = (--om5.begin()); begin != end; --begin) {
        ss << *begin << " ";
        values_from_iterator.push_back(*begin);
    }

    output_str = ss.str();

    ss.str("");
    ss.clear();

    std::cout << output_str << " ";

    expected_iterator_str = vector_to_ordered_str<Compare, Equals>(values_to_add4, false);
    check = output_str == expected_iterator_str;
    assert(check);
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\t\tControllo che gli elementi restituiti dall'iteratore costituiscono una permutazione degli elementi inseriti in precedenza... ";

    check = std::is_permutation(values_from_iterator.begin(), values_from_iterator.end(), values_to_add4.begin(), Equals());
    std::cout << "Risultato: " << colored_bool(check) << " ";
    assert(check);
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\t\tStampo con gli iteratori un multiset vuoto:" << std::endl;

    print_test_n(test_number) << "\t\t";

    for (typename multiset::const_iterator begin = multiset_empty.begin(), end = multiset_empty.end(); begin != end; ++begin)
        ss << *begin << " ";

    output_str = ss.str();

    ss.str("");
    ss.clear();

    std::cout << output_str << " ";

    check = output_str == "";
    assert(check);
    std::cout << passed() << std::endl;
    

    print_test_n(test_number) << "\tFine stampa con iteratori" << std::endl << std::endl;

    print_test_n(test_number) << BLUE << "\tFine interfaccia pubblica" << RESET << std::endl << std::endl;



    print_test_n(test_number) << BLUE << "\tInizio const correctness" << RESET << std::endl;    

    print_test_n(test_number) << "\tMetodi utilizzabili (da parte dell'utente o meno, come il distruttore) su un ordered_multiset costante:" << std::endl;

    print_test_n(test_number) << "\t\tCtor di default... ";
    const multiset const_fixture1;
    check = const_fixture1.size() == 0;
    assert(check);
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\t\tCostruttore che prende una coppia di iteratori... ";
    const multiset const_fixture2(values_to_add4.begin(), values_to_add4.end());
    check = const_fixture2.size() == values_to_add4.size();
    assert(check);
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\t\tDistruttore... ";
    const multiset* ptr_const_multiset = new const multiset;
    delete ptr_const_multiset;
    ptr_const_multiset = nullptr;
    assert(ptr_const_multiset == nullptr);
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\t\tCopy constructor... ";
    const multiset const_fixture3(const_fixture2);
    check = const_fixture3 == const_fixture2;
    assert(check);
    std::cout << passed() << std::endl; 

    //no operator=, es. const_fixture3 = const_fixture1
    //no clear, es. const_fixture3.clear()
    //no swap, es. const_fixture3.swap(const_fixture1)

    print_test_n(test_number) << "\t\tSize... ";
    check = const_fixture3.size() == const_fixture2.size();
    assert(check);
    std::cout << passed() << std::endl; 

    //no add, es. const_fixture3.add(val)
    //no remove, es. const_fixture3.remove(val)

    print_test_n(test_number) << "\t\tMultiplicity... ";
    check = const_fixture3.multiplicity(new_val) == 0;
    assert(check);
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\t\tOperatore di uguaglianza operator==... ";
    check = const_fixture3 == const_fixture2;
    assert(check);
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\t\tContains... ";
    check = !const_fixture3.contains(new_val);
    assert(check);
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\t\tBegin... ";
    typename multiset::const_iterator const_begin = const_fixture3.begin();
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\t\tEnd... ";
    typename multiset::const_iterator const_end = const_fixture3.end();
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\t\tOperatore di stream operator<<... ";
    std::cout << const_fixture3 << " ";
    std::cout << passed() << std::endl; 

    print_test_n(test_number) << "\tFine metodi utilizzabili per multiset costanti" << std::endl << std::endl;

    print_test_n(test_number) << "\tI metodi non utilizzabili dell'interfaccia pubblica per multiset costanti sono:" << std::endl;
    print_test_n(test_number) << "\toperator=, clear, swap, add, remove." << std::endl;

    print_test_n(test_number) << BLUE << "\tFine const correctness" << RESET << std::endl << std::endl;

    print_test_n(test_number) << BOLD << "Fine test " << test_number << RESET << std::endl << std::endl;
}

///////////////////////////////////////////////////////////////////////////
// Sezione dedicata al tipo int
///////////////////////////////////////////////////////////////////////////

struct int_equals { // classico funtore di uguaglianza
    bool operator()(int a, int b) const {
        return a == b;
    }
};

struct int_Z_4_equals { 
    /* due elementi sono uguali se sono uguali in Z_4 (insieme delle classi di resto modulo 4), 
    quindi avro' un multiset con dentro al massimo quattro elementi
    poiche' gli elementi di Z_4 sono la classe di 0, la classe di 1, la classe di 2 e la classe di 3 */
    bool operator()(int a, int b) const {
        return to_Z_4(a) == to_Z_4(b);
    }
};

struct int_abs_equals { // confronto in valore assoluto
    bool operator()(int a, int b) const {
        return std::abs(a) == std::abs(b);
    }
};


struct int_ascending { // ordine crescente "standard"
    bool operator()(int a, int b) const {
        return a < b;
    }
};

struct int_descending { // ordine decrescente "standard"
    bool operator()(int a, int b) const {
        return a > b;
    }
};

struct int_Z_4_ascending { // ordine crescente in Z_4
    bool operator()(int a, int b) const {
        return to_Z_4(a) < to_Z_4(b);
    }
};

struct int_Z_4_descending { // ordine decrescente in Z_4
    bool operator()(int a, int b) const {
        return to_Z_4(a) > to_Z_4(b);
    }
};

struct int_abs_ascending { // ordine crescente per valore assoluto
    bool operator()(int a, int b) const {
        return std::abs(a) < std::abs(b);
    }
};

struct int_abs_descending { // ordine decrescente per valore assoluto
    bool operator()(int a, int b) const {
        return std::abs(a) > std::abs(b);
    }
};



void int_tests(unsigned int &total_executed_tests) {
    typedef int T;
    std::string type_str = "int";
    std::ofstream out;
    std::streambuf *old_cout_buf = prepare_output(type_str, out);

    typedef std::vector<T> v;

    elements_related_args<T> four_args;

    unsigned int test_number = 0;

    print_test_n(test_number) << BOLD << "Inizio unit test per il tipo " << type_str << RESET << std::endl << std::endl;

    four_args.update(6, -4, 31, 30);
    exec_unit_test<int_ascending, int_descending, int_equals>(++test_number, "ordine crescente con uguaglianza \"standard\"",
    v{-1, -4, 0, 0, -4, -1, 4, 6, -2, -10, -7, 3, 42, -1}, four_args, v{1, 2, 1, 1, 1, 9, 1, 7, 10, 10});

    four_args.update(-1, 10, 60, 28);
    exec_unit_test<int_descending, int_ascending, int_equals>(++test_number, "ordine decrescente con uguaglianza \"standard\"",
    v{4, 2, 8, 8, 2, 9, 20, -1, -4, -4, -5, 10, 50, 51}, four_args, v{39, 2, -2, -2, -5, -5, 40, 30, 30, 20, 7, 1, 9, 41, -75});

    four_args.update(3, 1, 0, 5); // 0 non e' contenuto in entrambi i vettori
    exec_unit_test<int_Z_4_ascending, int_Z_4_descending, int_Z_4_equals>(++test_number, "ordine crescente in Z_4",
    v{1, 2, 3, 5, 2, 9, -3, -7, -2, 9}, four_args, v{2, 3, 1, 5, 2, 2, 6, -2, -5, -7});
    /* primo vector in Z_4 (es. 5 ≡ 1 mod 4): 1, 2, 3, 1, 2, 1, 1, 1, 2, 1 */
    /* secondo vector in Z_4:                 2, 3, 1, 1, 2, 2, 2, 2, 3, 1 */

    four_args.update(396, -2, 3, 9); // 3 non e' contenuto in entrambi i vettori
    exec_unit_test<int_Z_4_descending, int_Z_4_ascending, int_Z_4_equals>(++test_number, "ordine decrescente in Z_4",
    v{2, 1, 2, 6, 5, 2, -2, -6, 9, 8}, four_args, v{8, 2, 9, 4, -2, -6, -3, -6, -12, -42, 30});
    /* primo vector in Z_4:    2, 1, 2, 2, 1, 2, 2, 2, 1, 0    */
    /* secondo vector in Z_4:  0, 2, 1, 0, 2, 2, 1, 2, 0, 2, 2 */

    four_args.update(-29, 49, 100, 4); 
    exec_unit_test<int_abs_ascending, int_abs_descending, int_abs_equals>(++test_number, "ordine crescente per valore assoluto",
    v{0, 0, -1, 1, 24, 42, -24, 49, 49, -50, 29}, four_args, v{3, 2, 4, 3, -3, -3, -2, 9, -9, 1});

    four_args.update(5, -90, -2, 7); 
    exec_unit_test<int_abs_descending, int_abs_ascending, int_abs_equals>(++test_number, "ordine decrescente per valore assoluto",
    v{39, 90, -20, 27, 48, -5, 3, 9, 20, -45, 90, 150, 1}, four_args, v{-19, -29, 50, -25, 90, 1, -7});

    total_executed_tests += 6;

    print_test_n(test_number) << BOLD << "Fine unit test per il tipo " << type_str << RESET << std::endl << std::endl;

    if (old_cout_buf != nullptr) {
        std::cout.rdbuf(old_cout_buf);
        std::cout << "[" << type_str << "] Fine unit test, output salvato nella directory " << OUTPUT_DIR << std::endl;
    }
}



///////////////////////////////////////////////////////////////////////////
// Sezione dedicata al tipo char
///////////////////////////////////////////////////////////////////////////

struct char_equals { // classico funtore di uguaglianza
    bool operator()(char a, char b) const {
        return a == b;
    }
};

struct char_ignore_case_equals { // confronto case insensitive
    bool operator()(char a, char b) const {
        return toupper(a) == toupper(b);
    }
};


struct char_ascending { // ordine lessicografico crescente (le maiuscole vengono prima nella tabella ASCII rispetto alle minuscole)
    bool operator()(char a, char b) const {
        return a < b;
    }
};

struct char_descending { // ordine lessicografico decrescente
    bool operator()(char a, char b) const {
        return a > b;
    }
};

void char_tests(unsigned int &total_executed_tests) {
    typedef char T;
    std::string type_str = "char";
    std::ofstream out;
    std::streambuf *old_cout_buf = prepare_output(type_str, out);

    typedef std::vector<T> v;

    elements_related_args<T> four_args;

    unsigned int test_number = 0;

    print_test_n(test_number) << BOLD << "Inizio unit test per il tipo " << type_str << RESET << std::endl << std::endl;

    four_args.update('S', '5', 's', 12);
    exec_unit_test<char_ascending, char_descending, char_equals>(++test_number, "ordine lessicografico crescente, case sensitive",
    v{'a', 'z', 'B', 'b', 'g', '0', '1', 'b', '5', '5', 'S'}, four_args, v{'k', 'c', 'K', 'R', 'g', 'j', 'G', 'y', 'S', 'x', 'X'});

    four_args.update('R', 'z', 'F', 10);
    exec_unit_test<char_descending, char_ascending, char_equals>(++test_number, "ordine lessicografico decrescente, case sensitive",
    v{'a', 'z', '9', '8', '7', '7', 'A', 'R', 'E'}, four_args, v{'a', 's', 'z', 'Z', 'v', 'u', '2', '(', ')', '/', '%', '!', '?', '#', '#', '#'});

    four_args.update('J', 'x', 'k', 16);
    exec_unit_test<char_ascending, char_descending, char_ignore_case_equals>(++test_number, "ordine lessicografico crescente, case insensitive",
    v{'H', 'j', 'h', 'x', 'X', 'c', '=', 'C', '-', '+'}, four_args, v{'l', 'I', 'i', 'L', 's', 'd', '"', '*', 'r'});

    four_args.update('y', 'a', 'x', 1);
    exec_unit_test<char_descending, char_ascending, char_ignore_case_equals>(++test_number, "ordine lessicografico decrescente, case insensitive",
    v{'Y', 's', 'z', 'A', 'A', 'A', 'B'}, four_args, v{'r', 'Y', 'h', 's', '1', '7', '3', '9', 'R'});

    total_executed_tests += 4;

    print_test_n(test_number) << BOLD << "Fine unit test per il tipo " << type_str << RESET << std::endl << std::endl;

    if (old_cout_buf != nullptr) {
        std::cout.rdbuf(old_cout_buf);
        std::cout << "[" << type_str << "] Fine unit test, output salvato nella directory " << OUTPUT_DIR << std::endl;
    }
}


///////////////////////////////////////////////////////////////////////////
// Sezione dedicata al tipo float
///////////////////////////////////////////////////////////////////////////

struct float_equals { // funtore di uguaglianza, per i float e i double bisogna usare abs(a - b) < epsilon a causa dell'approssimazione
    bool operator()(float a, float b) const {
        return std::fabs(a - b) < std::numeric_limits<float>::epsilon();
    }
};

struct float_abs_equals { // confronto in valore assoluto
    bool operator()(float a, float b) const {
        a = std::fabs(a);
        b = std::fabs(b);
        return std::fabs(a - b) < std::numeric_limits<float>::epsilon();
    }
};


struct float_ascending { // ordine crescente
    bool operator()(float a, float b) const {
        return a < b;
    }
};

struct float_descending { // ordine decrescente
    bool operator()(float a, float b) const {
        return a > b;
    }
};

struct float_abs_ascending { // ordine crescente per valore assoluto
    bool operator()(float a, float b) const {
        return std::fabs(a) < std::fabs(b);
    }
};

struct float_abs_descending { // ordine decrescente per valore assoluto
    bool operator()(float a, float b) const {
        return std::fabs(a) > std::fabs(b);
    }
};

void float_tests(unsigned int &total_executed_tests) {
    typedef float T;
    std::string type_str = "float";
    std::ofstream out;
    std::streambuf *old_cout_buf = prepare_output(type_str, out);

    typedef std::vector<T> v;

    elements_related_args<T> four_args;

    unsigned int test_number = 0;

    print_test_n(test_number) << BOLD << "Inizio unit test per il tipo " << type_str << RESET << std::endl << std::endl;

    four_args.update(5.2f, -12.0f, 5.1f, 5);
    exec_unit_test<float_ascending, float_descending, float_equals>(++test_number, "ordine crescente con uguaglianza \"standard\"",
    v{5.3f, 5.2f, 5.0f, -2.0f, -3.0f, -12.0f, -12.0f, 12.0f}, four_args, v{6.7f, 3.14f, -3.14f, 3.14f, 5.6f, -4.0f});

    four_args.update(-4.20f, 4.2f, -4.202f, 7);
    exec_unit_test<float_descending, float_ascending, float_equals>(++test_number, "ordine decrescente con uguaglianza \"standard\"",
    v{-8.0f, -4.201f, -4.203f, -4.20f, 4.2f, 4.2f, 8.0f}, four_args, v{-6.0f, -2.0f, 4.0f, 7.0f, 6.9f, 7.1f- -4.0f});

    four_args.update(-5.43f, 9.4f, 9.4001f, 6);
    exec_unit_test<float_abs_ascending, float_abs_descending, float_abs_equals>(++test_number, "ordine crescente per valore assoluto",
    v{5.4f, 9.4f, -9.4f, -9.40001f, 9.40001f, 5.41f, 5.43f, -4.0f}, four_args, v{2.71f, -3.46f, 3.46f, -20.0f, 24.3f, -25.0f});
    
    four_args.update(2.4f, -25.1f, 39.07f, 9);
    exec_unit_test<float_abs_descending, float_abs_ascending, float_abs_equals>(++test_number, "ordine decrescente per valore assoluto",
    v{2.4f, 8.0f, -8.0f, 25.0f, -49.0f, 25.1f, -25.0f}, four_args, v{38.4f, 29.5, 29.6f, 29.7f, -39.0f, -39.1f, 39.05f, 39.06f});
    
    total_executed_tests += 4;

    print_test_n(test_number) << BOLD << "Fine unit test per il tipo " << type_str << RESET << std::endl << std::endl;

    if (old_cout_buf != nullptr) {
        std::cout.rdbuf(old_cout_buf);
        std::cout << "[" << type_str << "] Fine unit test, output salvato nella directory " << OUTPUT_DIR << std::endl;
    }
}



///////////////////////////////////////////////////////////////////////////
// Sezione dedicata al tipo std::string
///////////////////////////////////////////////////////////////////////////

struct string_equals { // classico funtore di uguaglianza
    bool operator()(const std::string &a, const std::string &b) const {
        return a == b;
    }
};

struct string_ignore_case_equals { // confronto case insensitive
    bool operator()(const std::string &a, const std::string &b) const {
        return equal_strings_ignore_case(a, b);
    }
};


struct string_ascending { // ordine lessicografico crescente
    bool operator()(const std::string &a, const std::string &b) const {
        return a < b;
    }
};

struct string_descending { // ordine lessicografico decrescente
    bool operator()(const std::string &a, const std::string &b) const {
        return a > b;
    }
};

void string_tests(unsigned int &total_executed_tests) {
    typedef std::string T;
    std::string type_str = "std::string";
    std::ofstream out;
    std::streambuf *old_cout_buf = prepare_output("string", out); // i : potrebbero avere problemi nel nome del file

    typedef std::vector<T> v;

    elements_related_args<T> four_args;

    unsigned int test_number = 0;

    print_test_n(test_number) << BOLD << "Inizio unit test per il tipo " << type_str << RESET << std::endl << std::endl;
    
    four_args.update("b", "aaa", "aaaa", 42);
    exec_unit_test<string_ascending, string_descending, string_equals>(++test_number, "ordine lessicografico crescente, case sensitive",
    v{"ciao", "aaa", "AAA", "aaa", "aaab", "b", "ba", "c"}, four_args, v{"OOOo", "OOOO", "a", "aa", "Ab", "ab", "zzzz"});

    four_args.update("z", "ciao", "zz", 2);
    exec_unit_test<string_descending, string_ascending, string_equals>(++test_number, "ordine lessicografico decrescente, case sensitive",
    v{"bbb", "cccciao", "ciao", "cia", "a", "zzzzz", "z"}, four_args, v{"1", "2", "A", "AA", "Ac", "AAe", "AAz", "Z", "@", "#"});

    four_args.update("AAA", "aBA", "b", 34);
    exec_unit_test<string_ascending, string_descending, string_ignore_case_equals>(++test_number, "ordine lessicografico crescente, case insensitive",
    v{"aaa", "aba", "abb", "abba", "AbA", "ABA"}, four_args, v{"aaaa", "AAAA", "aaAa", "AaaA", "aaaz", "Z", "a"});

    four_args.update("A", "2", "3", 4);
    exec_unit_test<string_descending, string_ascending, string_ignore_case_equals>(++test_number, "ordine lessicografico decrescente, case insensitive",
    v{"testing", "TEST", "TesT", "Testin", "t", "T", "a", "z", "1", "2"}, four_args, v{"just testing", "strings", "here", "JuSt", "JUST", "ju", "j", "J"});

    total_executed_tests += 4;

    print_test_n(test_number) << BOLD << "Fine unit test per il tipo " << type_str << RESET << std::endl << std::endl;

    if (old_cout_buf != nullptr) {
        std::cout.rdbuf(old_cout_buf);
        std::cout << "[" << type_str << "] Fine unit test, output salvato nella directory " << OUTPUT_DIR << std::endl;
    }
}



///////////////////////////////////////////////////////////////////////////
// Sezione dedicata al tipo point
///////////////////////////////////////////////////////////////////////////

struct point {
    int x;
    int y;

    point() : x(0), y(0) { }

    point(int x_val, int y_val) : x(x_val), y(y_val) { }
};

std::ostream &operator<<(std::ostream &os, const point &p) {
    os << "(" << p.x << ", " << p.y << ")";
    return os;
}

struct point_equals { // funtore di uguaglianza
    bool operator()(const point &a, const point &b) const {
        return a.x == b.x && a.y == b.y;
    }
};

struct point_ascending { // ordine crescente
    bool operator()(const point &a, const point &b) const {
        return (a.x * a.x + a.y * a.y) < (b.x * b.x + b.y * b.y); // utilizzo x^2 + y^2
    }
};

struct point_descending { // ordine decrescente
    bool operator()(const point &a, const point &b) const {
        return (a.x * a.x + a.y * a.y) > (b.x * b.x + b.y * b.y);
    }
};

void point_tests(unsigned int &total_executed_tests) {
    typedef point T;
    std::string type_str = "point";
    std::ofstream out;
    std::streambuf *old_cout_buf = prepare_output(type_str, out);

    typedef std::vector<T> v;

    elements_related_args<T> four_args;

    unsigned int test_number = 0;

    print_test_n(test_number) << BOLD << "Inizio unit test per il tipo CUSTOM " << type_str << RESET << std::endl << std::endl;
    
    four_args.update(point(0, 0), point(1, 2), point(1, 1), 6);
    exec_unit_test<point_ascending, point_descending, point_equals>(++test_number, "x^2 + y^2 in ordine crescente",
    v{point(0, 0), point(1, 2), point(1, 2), point(-1, -2), point(-5, -10)}, four_args, v{point(0, 1), point(1, 0), point(5, 4), point(6, 2)});

    four_args.update(point(7, 1), point(2, 1), point(0, 0), 9);
    exec_unit_test<point_descending, point_ascending, point_equals>(++test_number, "x^2 + y^2 in ordine decrescente",
    v{point(8, 2), point(2, 1), point(-2, -1), point(1, 2), point(4, 3), point(7, 1)}, four_args, v{point(7, 2), point(2, 4), point(-2, -4), point(1, 2)});

    total_executed_tests += 2;

    print_test_n(test_number) << BOLD << "Fine unit test per il tipo CUSTOM " << type_str << RESET << std::endl << std::endl;

    if (old_cout_buf != nullptr) {
        std::cout.rdbuf(old_cout_buf);
        std::cout << "[" << type_str << "] Fine unit test, output salvato nella directory " << OUTPUT_DIR << std::endl;
    }
}



///////////////////////////////////////////////////////////////////////////
// Sezione dedicata al tipo person
///////////////////////////////////////////////////////////////////////////

struct person {
    std::string name;
    std::string surname;
    unsigned int age;

    person() : name(""), surname(""), age(0) { }

    person(const std::string &n, const std::string &s, unsigned int a) : name(n), surname(s), age(a) { }
};

std::ostream &operator<<(std::ostream &os, const person &p) {
    os << "{\"name\": " << p.name << ", \"surname\": " << p.surname << ", \"age\": " << p.age << "}";
    return os;
}

struct person_equals { // funtore di uguaglianza
    bool operator()(const person &a, const person &b) const {
        return equal_strings_ignore_case(a.name, b.name) && equal_strings_ignore_case(a.surname, b.surname) && a.age == b.age;
    }
};

struct person_age_ascending { // eta' in ordine crescente
    bool operator()(const person &a, const person &b) const {
        return a.age < b.age; 
    }
};

struct person_age_descending { // eta' in ordine decrescente
    bool operator()(const person &a, const person &b) const {
        return a.age > b.age; 
    }
};

void person_tests(unsigned int &total_executed_tests) {
    typedef person T;
    std::string type_str = "person";
    std::ofstream out;
    std::streambuf *old_cout_buf = prepare_output(type_str, out);

    typedef std::vector<T> v;

    elements_related_args<T> four_args;

    unsigned int test_number = 0;

    print_test_n(test_number) << BOLD << "Inizio unit test per il tipo CUSTOM " << type_str << RESET << std::endl << std::endl;
    
    four_args.update(person("CRISTIAN", "Piacente", 21), person("john", "doe", 12), person("random", "person", 42), 6);
    exec_unit_test<person_age_ascending, person_age_descending, person_equals>(++test_number, "eta' in ordine crescente",
    v{person("john", "doe", 12), person("cristian", "piacente", 21), person("john", "doe", 12), person("jane", "doe", 15), person("joe", "sixpack", 19), person("joe", "sixpack", 19), person("joe", "citizen", 40)},
    four_args, 
    v{person("joe", "johns", 28), person("john", "public", 28), person("john", "cena", 45), person("cristian", "p", 99)});

    four_args.update(person("MarCo", "BianChi", 40), person("cat", "GATTO", 36), person("some", "stranger", 42), 4);
    exec_unit_test<person_age_descending, person_age_ascending, person_equals>(++test_number, "eta' in ordine decrescente",
    v{person("paolo", "rossi", 50), person("marco", "bianchi", 40), person("simone", "verdi", 40), person("matteo", "salvo", 36), person("cat", "gatto", 36), person("cat", "gatto", 36), person("roberto", "fantasia", 25), person("nonneho", "piu", 36)},
    four_args, 
    v{person("francesco", "rossi", 23), person("gianfranco", "doe", 23), person("rock", "roccia", 4), person("sasso", "raro", 25)});

    total_executed_tests += 2;

    print_test_n(test_number) << BOLD << "Fine unit test per il tipo CUSTOM " << type_str << RESET << std::endl << std::endl;

    if (old_cout_buf != nullptr) {
        std::cout.rdbuf(old_cout_buf);
        std::cout << "[" << type_str << "] Fine unit test, output salvato nella directory " << OUTPUT_DIR << std::endl;
    }
}



///////////////////////////////////////////////////////////////////////////
// Sezione dedicata al tipo fraction
///////////////////////////////////////////////////////////////////////////

struct fraction {
    int num;
    int den;

    fraction() : num(0), den(1) { }

    fraction(int n, int d) : num(n), den(d) { }
};

std::ostream &operator<<(std::ostream &os, const fraction &f) {
    os << f.num << "/" << f.den;
    return os;
}

struct fraction_equals { // funtore di uguaglianza
    bool operator()(const fraction &a, const fraction &b) const {
        float a_float = static_cast<float>(a.num) / a.den;
        float b_float = static_cast<float>(b.num) / b.den;

        return std::fabs(a_float - b_float) < std::numeric_limits<float>::epsilon();
    }
};


struct fraction_ascending { // ordine crescente
    bool operator()(const fraction &a, const fraction &b) const {
        float a_float = static_cast<float>(a.num) / a.den;
        float b_float = static_cast<float>(b.num) / b.den;

        return a_float < b_float;
    }
};

struct fraction_descending { // ordine decrescente
    bool operator()(const fraction &a, const fraction &b) const {
        float a_float = static_cast<float>(a.num) / a.den;
        float b_float = static_cast<float>(b.num) / b.den;

        return a_float > b_float;
    }
};

void fraction_tests(unsigned int &total_executed_tests) {
    typedef fraction T;
    std::string type_str = "fraction";
    std::ofstream out;
    std::streambuf *old_cout_buf = prepare_output(type_str, out);

    typedef std::vector<T> v;

    elements_related_args<T> four_args;

    unsigned int test_number = 0;

    print_test_n(test_number) << BOLD << "Inizio unit test per il tipo CUSTOM " << type_str << RESET << std::endl << std::endl;
    
    four_args.update(fraction(123, 123), fraction(1, 2), fraction(1, 4), 26);
    exec_unit_test<fraction_ascending, fraction_descending, fraction_equals>(++test_number, "valore della frazione in ordine crescente",
    v{fraction(1, 1), fraction(0, 5), fraction(90, 2), fraction(93, 2), fraction(92, 2), fraction(1, 5), fraction(1, 2), fraction(1, 2), fraction(1, 3)},
    four_args, 
    v{fraction(6, 3), fraction(12, 6), fraction(8, 2), fraction(9, 2), fraction(9, 2)});

    four_args.update(fraction(400, 300), fraction(-20, -40), fraction(0, 999), 25);
    exec_unit_test<fraction_descending, fraction_ascending, fraction_equals>(++test_number, "valore della frazione in ordine decrescente",
    v{fraction(8, 2), fraction(16, 4), fraction(20, 5), fraction(4, 3), fraction(5, 3), fraction(8, 9), fraction(16, 18), fraction(1, 2), fraction(2, 4)},
    four_args, 
    v{fraction(3, 4), fraction(2, 4), fraction(1, 4), fraction(-5, 3), fraction(8, -4), fraction(-8, 4)});

    total_executed_tests += 2;

    print_test_n(test_number) << BOLD << "Fine unit test per il tipo CUSTOM " << type_str << RESET << std::endl << std::endl;

    if (old_cout_buf != nullptr) {
        std::cout.rdbuf(old_cout_buf);
        std::cout << "[" << type_str << "] Fine unit test, output salvato nella directory " << OUTPUT_DIR << std::endl;
    }
}



///////////////////////////////////////////////////////////////////////////
// Sezione dedicata al tipo ordered_multiset
///////////////////////////////////////////////////////////////////////////

struct ordered_multiset_equals { 
    /* i due multiset usano lo stesso tipo di funtore di ordinamento
    perche' se lo avessi cambiato sul secondo avrei avuto due tipi diversi 
    e in un multiset non ci possono essere elementi di tipo diverso */
    template <typename T, typename Cmp, typename Eql>
    bool operator()(const ordered_multiset<T, Cmp, Eql> &om1, const ordered_multiset<T, Cmp, Eql> &om2) const {
        return om1 == om2;
    }
};


struct ordered_multiset_ascending { // numero totale di elementi in ordine crescente
    template <typename T, typename Cmp, typename Eql>
    bool operator()(const ordered_multiset<T, Cmp, Eql> &om1, const ordered_multiset<T, Cmp, Eql> &om2) const {
        return om1.size() < om2.size();
    }
};

struct ordered_multiset_descending { // numero totale di elementi in ordine decrescente
    template <typename T, typename Cmp, typename Eql>
    bool operator()(const ordered_multiset<T, Cmp, Eql> &om1, const ordered_multiset<T, Cmp, Eql> &om2) const {
        return om1.size() > om2.size();
    }
};

void ordered_multiset_tests(unsigned int &total_executed_tests) {
    typedef ordered_multiset<int, int_ascending, int_equals> T;
    std::string type_str = "ordered_multiset<int, int_ascending, int_equals>";
    std::ofstream out;
    std::streambuf *old_cout_buf = prepare_output("ordered_multiset", out); // piu' leggibile

    typedef std::vector<int> v; // int vector
    typedef std::vector<T> o_v; // ordered_multiset vector
    

    elements_related_args<T> four_args;

    unsigned int test_number = 0;

    v first_vec{1, 8, -2, -5, -5, -5, -20, -4, 6, 3};
    v second_vec, third_vec;
    vec_from_indexes(first_vec, second_vec);
    vec_from_indexes(first_vec, third_vec, false);
    
    T om1(first_vec.begin(), first_vec.end()), om2(second_vec.begin(), second_vec.end()), om3(third_vec.begin(), third_vec.end());

    first_vec.clear(); second_vec.clear(); third_vec.clear(); // riutilizzo gli stessi vector
    first_vec.assign({-3, -3, 4, 2, 10, 29, 38, -25, 23, -3});
    vec_from_indexes(first_vec, second_vec);
    vec_from_indexes(first_vec, third_vec, false);

    remove_val_in_vec<int_equals>(third_vec, -3); // per rendere om6 piu' corto
    remove_val_in_vec<int_equals>(third_vec, 38);
    remove_val_in_vec<int_equals>(third_vec, 23);

    T om4(first_vec.begin(), first_vec.end()), om5(second_vec.begin(), second_vec.end()), om6(third_vec.begin(), third_vec.end());
    om5.clear(); // non lo uso in realta'

    print_test_n(test_number) << BOLD << "Inizio unit test per il tipo CUSTOM " << type_str << RESET << std::endl << std::endl;
    
    four_args.update(om2, om1, om5, 6);
    exec_unit_test<ordered_multiset_ascending, ordered_multiset_descending, ordered_multiset_equals>(++test_number, "numero totale di elementi in ordine crescente",
    o_v{om1, om2, om1, om1, om4, om6, om6},
    four_args, 
    o_v{om3, om3, om3, om2, om6});

    first_vec.clear(); second_vec.clear(); third_vec.clear();

    first_vec.assign({-5, -5, 2, 2, 2, 2, 0, 49, 39, 29, -52});
    vec_from_indexes(first_vec, second_vec);
    vec_from_indexes(first_vec, third_vec, false);

    T om7(first_vec.begin(), first_vec.end()), om8(second_vec.begin(), second_vec.end()), om9(third_vec.begin(), third_vec.end());

    first_vec.clear(); second_vec.clear(); third_vec.clear();
    first_vec.assign({8, 2, 8, 8, 8, 4, -4, -20, -24, 4, 9, 1});
    vec_from_indexes(first_vec, second_vec);
    vec_from_indexes(first_vec, third_vec, false);

    remove_val_in_vec<int_equals>(second_vec, -4); // per rendere om8 piu' corto
    remove_val_in_vec<int_equals>(second_vec, 4);

    T om10(first_vec.begin(), first_vec.end()), om11(second_vec.begin(), second_vec.end()), om12(third_vec.begin(), third_vec.end());

    T om13; // costruttore di default

    four_args.update(om13, om11, om12, 31);
    exec_unit_test<ordered_multiset_descending, ordered_multiset_ascending, ordered_multiset_equals>(++test_number, "numero totale di elementi in ordine decrescente",
    o_v{om8, om9, om7, om7, om10, om11, om11, om13},
    four_args, 
    o_v{om10, om11, om11, om7, om7});

    total_executed_tests += 2;

    print_test_n(test_number) << BOLD << "Fine unit test per il tipo CUSTOM " << type_str << RESET << std::endl << std::endl;

    if (old_cout_buf != nullptr) {
        std::cout.rdbuf(old_cout_buf);
        std::cout << "[" << type_str << "] Fine unit test, output salvato nella directory " << OUTPUT_DIR << std::endl;
    }
}



///////////////////////////////////////////////////////////////////////////
// Main
///////////////////////////////////////////////////////////////////////////

int main() {

    // Contatore per sapere alla fine quanti test sono stati eseguiti in tutto

    unsigned int total_executed_tests = 0; 


    // Chiamate ai unit tests

    int_tests(total_executed_tests);

    char_tests(total_executed_tests);

    float_tests(total_executed_tests);

    string_tests(total_executed_tests);

    point_tests(total_executed_tests);

    person_tests(total_executed_tests);

    fraction_tests(total_executed_tests);

    ordered_multiset_tests(total_executed_tests);

    std::cout << BOLD << GREEN << "[Report] " << RESET;
    std::cout << BOLD << "In totale sono stati eseguiti " << BLUE << total_executed_tests << RESET << BOLD << " test." << RESET << std::endl;

    return 0;
}