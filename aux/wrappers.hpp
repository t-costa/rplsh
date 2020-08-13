#ifndef rplsh_wrappers_hpp
#define rplsh_wrappers_hpp

#include <cstdlib>

template <typename Tin, typename Tout>
class seq_wrapper {
public:
  virtual Tout compute(Tin& input) = 0;
  // user should implement also the following methods
  // in order to generate data paralel skeletons
  // for map:    type_out op(type_in t)
  // for reduce: type op(type t1, type t2)
  // otherwise it will not compile
};

//TODO: forse conviene aggiungere a tutti i wrapper
//degli hooks per init/cleanup alla fine...
//non obbligatori e li lasci vuoti in caso...

//TODO: e se la map ha più wrappers dentro? funziona tutto??
template<typename Tin,
          typename Tout,
          typename Tin_el, //se è vector, preso come tipo interno, altrimenti è = Tout
          typename Tout_el>  //Tin è per forza vector (o container comunque)
          //typename T>
class map_stage_wrapper {

  //lascio la compute perchè così lo stadio può essere
  //messo dentro un qualsiasi pattern (farm, pipe, seq...)
  //TODO: devo rivedere come vengono i presi/come
  //fa rplsh a capire dal nodo cosa ci deve cacciare
  virtual Tout compute(Tin& input) = 0;

  virtual Tout_el op(const Tin_el& input) = 0;

  //input lo prende per potersi creare il res di output,
  //in teoria non gli dovrebbe servire altro, perchè
  //è all'inizio della funzione e non c'è materialmente altro!
  virtual Tout begin_hook(Tin input) {}

  //per ora non ce lo metto, ma sarebbe una funzione di
  //cleanup alla fine, quindi forse prende Tin per farci la delete?
  //comunque gli unici argomenti che potrebbe sfruttare sono quelli
  //già presenti nella classe, funzione o globale. Non dovrebbe mai
  //servire un tipo nuovo. Il return è void perchè non deve restituire
  //niente di logica per la compute (dovrebbe essere tutto in op),
  //però potrebbe essere che in par uso new, e quindi serve la delete,
  //ma nel seq no e non metterei niente dentro... => quindi ci sarebbe
  //comunque roba che dovrebbe fare da 0 rplsh...
  //virtual void end_hook(T input) {}
};

//stessi discorsi che per la map
template<typename Tin,
          typename Tout,
          typename Tin_el,
          typename Tout_el>
          //typename T>
class reduce_stage_wrapper {

  virtual Tout compute(Tin& input) = 0;

  virtual Tout_el op(Tin_el& input1, Tin_el& input2) = 0;

  virtual Tout begin_hook(Tin input) {}

  //virtual void end_hook(T input) {}

protected:
  Tout identity{};
};

template <typename Tout>
class source {
public:
  virtual bool has_next() = 0;
  virtual Tout* next() = 0;
};

template <typename Tin>
class drain {
public:
  virtual void process(Tin * x) = 0;
};

/*template <typename T>
class composable {
public:
    virtual T&& operator[](size_t idx);
    virtual T& operator[](size_t idx);
}*/

// class type representing a set of values of type T
// that could be splitted in several sub-composable sets
// -> similiar concept to the TBB Range
template <typename T>
class composable {
public:
    virtual bool is_empty() const       = 0;
    virtual std::size_t size() const    = 0;
    virtual bool is_splittable() const  = 0;
};

#endif
