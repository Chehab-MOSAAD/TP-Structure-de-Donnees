#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "skiplist.h"
#include "rng.h"

// Structure de noeud de la SkipList
struct s_Node{
    unsigned int level;
    int value; // valeur stockée dans le noeud
    struct s_Node **next; // tableau des pointeurs vers les noeuds suivants
    struct s_Node *prev; // pointeur vers le noeud précédent
};

// Structure de la SkipList
struct s_SkipList{
    unsigned int size; // nombre d'éléments dans la SkipList
    unsigned int max_level; // niveau maximum de la SkipList
    Node *sentinel; // noeud sentinelle
    RNG prob; // loi de probabilité qui donne le niveau à chaque noeud
};

// Cette fonction crée une nouvelle SkipList avec un élément sentinelle initialisé pour marquer le début et la fin de la SkipList.
SkipList skiplist_create(int nblevels) {
    Node *sentinel = malloc(sizeof(Node)); // Alloue de la mémoire pour la sentinelle de la SkipList.
    sentinel->next = malloc(nblevels * sizeof(Node*)); // Alloue de la mémoire pour les pointeurs vers les éléments suivants de la sentinelle, selon le nombre de niveaux souhaité.
    for(int i = 0; i < nblevels; ++i) { // Initialise tous les pointeurs suivants à la sentinelle elle-même, car il n'y a pas encore d'autres éléments dans la SkipList.
        sentinel->next[i] = sentinel;
    }
    sentinel->prev = sentinel; // Initialise le pointeur précédent de la sentinelle à la sentinelle elle-même.
    SkipList list = malloc(sizeof(struct s_SkipList)); // Alloue de la mémoire pour la nouvelle SkipList.
    sentinel->level = nblevels; // Initialise le niveau de la sentinelle au nombre de niveaux spécifié.
    list->sentinel = sentinel; // Initialise la sentinelle de la SkipList avec la sentinelle que nous avons créée.
    list->size = 0; // Initialise la taille de la SkipList à 0 car il n'y a pas encore d'éléments dans la SkipList.
    list->max_level = nblevels; // Initialise le niveau maximum de la SkipList au nombre de niveaux spécifié.
    list->prob = rng_initialize(0); // Initialise le générateur de nombres aléatoires utilisé pour générer les niveaux de chaque élément de la SkipList.
    return list; // Retourne un pointeur vers la nouvelle SkipList créée.
}

// Cette fonction détruire et libére la memoire utilisée par une SkipList.
void skiplist_delete(SkipList d) {
    Node *e = (d)->sentinel->next[0], *toDelete; // Initialise un pointeur d'élément à l'élément suivant de la sentinelle de la SkipList, ainsi qu'un pointeur temporaire pour stocker l'élément à supprimer.
    while (e != (d)->sentinel) { // Tant que nous n'avons pas atteint la sentinelle de la SkipList, supprime chaque élément un par un.
        toDelete = e; // Stocke l'élément à supprimer dans le pointeur temporaire.
        e = e->next[0]; // Déplace le pointeur d'élément au suivant.
        free(toDelete->next); // Libère l'espace mémoire alloué pour les pointeurs next de l'élément à supprimer.
        free(toDelete); // Libère l'espace mémoire alloué pour l'élément à supprimer.
    }
    free(e->next); // Libère l'espace mémoire alloué pour les pointeurs next de la sentinelle.
    free(e); // Libère l'espace mémoire alloué pour la sentinelle.
    free(d); // Libère l'espace mémoire alloué pour la SkipList.
    d = NULL; // Définit le pointeur de la SkipList à NULL pour éviter les problèmes de référence mémoire.
}

// Cette fonction renvoie la taille de la SkipList.
unsigned int skiplist_size(SkipList d) {
    assert(d); // Vérifie si la SkipList est valide.
    return d->size; // Retourne la taille de la SkipList.
}

// Cette fonction renvoie la valeur de l'élément à un indice spécifié dans une SkipList.
int skiplist_ith(SkipList d, unsigned int i) {
    assert(i < d->size); // Vérifie si l'indice est dans la plage de la SkipList.
    Node *e = d->sentinel; // Initialise un pointeur d'élément à la sentinelle de la SkipList.
    for (unsigned int j = 0; j <= i; ++j) { // Parcourt chaque élément de la SkipList jusqu'à l'indice spécifié.
        e = e->next[0]; // Déplace le pointeur d'élément au suivant.
    }
    return e->value; // Retourne la valeur de l'élément à l'indice spécifié.
}

// Cette fonction applique une fonction à chaque élément d'une SkipList.
void skiplist_map(SkipList d, ScanOperator f, void *user_data) {
    Node *e = d->sentinel; // Initialise un pointeur d'élément à la sentinelle de la SkipList.
    for (unsigned int i = 0; i < d->size; ++i) { // Parcourt chaque élément de la SkipList.
        e = e->next[0]; // Déplace le pointeur d'élément au suivant.
        f(e->value, user_data); // Applique la fonction de numérisation sur la valeur de l'élément en utilisant l'argument user_data.
    }
}

//****************INSERT*******************************//

// Cette fonction ajoute une valeur à la SkipList.
SkipList skiplist_insert(SkipList d, int value) {
    Node *new = malloc(sizeof(Node)); // Alloue de la mémoire pour un nouveau noeud.
    new->level = rng_get_value(&d->prob, d->max_level - 1); // Obtient le niveau du nouveau noeud à l'aide de la fonction de génération de nombre aléatoire.
    new->level++; // Incrémente le niveau du nouveau noeud.
    unsigned int cmpt = 0; // Initialise un compteur à 0.
    new->value = value; // Initialise la valeur du nouveau noeud avec la valeur donnée.
    new->next = malloc(new->level * sizeof(Node*)); // Alloue de la mémoire pour les pointeurs suivants du nouveau noeud.
    if(skiplist_size(d) == 0){ // Si la SkipList est vide.
        new->prev = d->sentinel; // Initialise le pointeur précédent du nouveau noeud à la sentinelle.
        d->sentinel->prev = new; // Initialise le pointeur précédent de la sentinelle au nouveau noeud.
        for(unsigned int i = 0; i < new->level; ++i){ // Parcourt chaque niveau du nouveau noeud.
            new->next[i] = d->sentinel; // Initialise les pointeurs suivants du nouveau noeud à la sentinelle.
            d->sentinel->next[i] = new; // Initialise les pointeurs suivants de la sentinelle au nouveau noeud.
        }
        d->size++; // Incrémente la taille de la SkipList.
        return d; // Retourne la SkipList.
    }
    Node *e = d->sentinel; // Initialise un pointeur d'élément à la sentinelle de la SkipList.
    while ((e->next[0] != d->sentinel) && (e->next[0]->value <= new->value)) { // Parcourt la SkipList jusqu'à trouver l'endroit où l'élément doit être inséré.
        e = e->next[0]; // Déplace le pointeur d'élément au suivant.
    }
    if ((e != d->sentinel) && (e->value == new->value)) { // Vérifie si un élément avec la même valeur existe déjà.
        free(new->next); // Libère l'espace alloué pour les pointeurs de new.
        free(new); // Libère l'espace alloué pour new.
        return d; // Retourne la SkipList inchangée.
    }
    new->prev = e; // Met à jour les pointeurs de l'élément à insérer.
    e->next[0]->prev = new; // Met à jour les pointeurs de l'élément suivant l'élément à insérer.
    if(e->level>=new->level){ // si le niveau de l'élément courant est supérieur ou égal au niveau du nouvel élément
        for(unsigned int i=0;i<new->level;++i){ // Parcourir les niveaux du nouvel élément
            new->next[i]=e->next[i]; // Le pointeur vers le prochain élément pour le niveau i du nouvel élément pointe vers l'élément suivant de l'élément courant
            e->next[i]=new; // Le pointeur vers le prochain élément pour le niveau i de l'élément courant pointe vers le nouvel élément
        }
    }else{ // Si le niveau de l'élément courant est inférieur au niveau du nouvel élément
        for(unsigned int i=0;i<e->level;++i){ // Parcourir les niveaux de l'élément courant
            new->next[i]=e->next[i]; // Le pointeur vers le prochain élément pour le niveau i du nouvel élément pointe vers l'élément suivant de l'élément courant
            e->next[i]=new; // Le pointeur vers le prochain élément pour le niveau i de l'élément courant pointe vers le nouvel élément
            ++cmpt; // Augmenter le compteur de niveaux parcourus
        }
        while(e->level<new->level){ // Tant que le niveau de l'élément courant est inférieur au niveau du nouvel élément
            e=e->prev; // Avancer à l'élément précédent
        }
        for(unsigned int i=cmpt;i<new->level;++i){ // Parcourir les niveaux restants du nouvel élément
            new->next[i]=e->next[i]; // Le pointeur vers le prochain élément pour le niveau i du nouvel élément pointe vers l'élément suivant de l'élément courant
            e->next[i]=new; // Le pointeur vers le prochain élément pour le niveau i de l'élément courant pointe vers le nouvel élément
        }
    }
    d->size++; // Incremente la taille de la SkipList
    return d; // Retourne la SkipList mise à jour
}

//****************SEARCH*******************************//

// Cette fonction cherche une valeur dans la SkipList. Si cette valeur est dans la SkipList, la fonction renvoie vraie.
bool skiplist_search(SkipList d, int value, unsigned int *nb_operations) {
    Node *current = d->sentinel; // Pointeur vers l'élément sentinelle
    *nb_operations = *nb_operations + 1; // Incrémente le nombre d'opérations effectuées
    // Parcours de la liste depuis le niveau le plus haut vers le plus bas
    for (int i = d->max_level - 1; i >= 0; --i) {
        while ((current->next[i]->value < value) && (current->next[i] != d->sentinel)) {
            current = current->next[i]; // Se déplace vers l'élément suivant dans le niveau courant
            *nb_operations = *nb_operations + 1; // Incrémente le nombre d'opérations effectuées
        }
        if (current->next[i]->value == value) { // Si la valeur de l'élément suivant dans le niveau courant est égale à la valeur cherchée
            return true; // On a trouvé la valeur cherchée donc on retourne true
        }
    }
    // On est arrivé au niveau 0, on vérifie l'élément suivant de l'élément courant dans ce niveau
    if (current->next[0]->value == value) { // Si la valeur de l'élément suivant dans le niveau 0 est égale à la valeur cherchée
        return true; // On a trouvé la valeur cherchée, on retourne true
    }
    return false; // On a parcouru toute la liste, on n'a pas trouvé la valeur cherchée donc on retourne false
}

//****************ITERATEUR*******************************//

// Structure de l'Iterateur
struct s_SkipListIterator{
  Node *current; // pointeur vers le noeud actuel dans la SkipList
  Node *begin; // ointeur vers le noeud de début dans la SkipList
  SkipListIterator(*next)(SkipListIterator); // pointeur vers une fonction qui renvoie le prochain itérateur dans la SkipList
  SkipList  d; // la SkipList associée à cet itérateur
};

// Cette fonction déplace l'itérateur vers le prochain élément dans l'ordre croissant de valeur.
SkipListIterator forward(SkipListIterator it){
    it->current=it->current->next[0]; // Déplace l'itérateur vers le prochain élément.
    return it; // Retourne l'itérateur mis à jour.
}

// Cette fonction déplace l'itérateur vers l'élément précédent dans l'ordre croissant de valeur.
SkipListIterator backward(SkipListIterator it){
    it->current =it->current->prev; // Déplace l'itérateur vers l'élément précédent.
    return it; // Retourne l'itérateur mis à jour.
}

// Cette fonction crée un nouvel itérateur pour la skip-list spécifiée.
SkipListIterator skiplist_iterator_create(SkipList d, unsigned char w){
    SkipListIterator e=malloc(sizeof(struct s_SkipListIterator)); // Alloue de la mémoire pour le nouvel itérateur.
    if(w==FORWARD_ITERATOR){ // Si l'itérateur est dans le sens avant.
        e->current=d->sentinel->next[0]; // Initialise l'itérateur sur le premier élément de la skip-list.
        e->begin=d->sentinel->next[0]; // Stocke le premier élément pour pouvoir réinitialiser l'itérateur plus tard.
        e->next=forward; // Configure la fonction de déplacement pour l'itérateur dans le sens avant.
    }
    if(w==BACKWARD_ITERATOR){ // Si l'itérateur est dans le sens arrière.
        e->current=d->sentinel->prev; // Initialise l'itérateur sur le dernier élément de la skip-list.
        e->begin=d->sentinel->prev; // Stocke le dernier élément pour pouvoir réinitialiser l'itérateur plus tard.
        e->next=backward; // Configure la fonction de déplacement pour l'itérateur dans le sens arrière.
    }
    e->d=d; // Stocke la skip-list à laquelle l'itérateur est associé.
    return e; // Retourne le nouvel itérateur.
}

// Cette fonction libère la mémoire allouée pour un itérateur de SkipList.
void skiplist_iterator_delete(SkipListIterator it){
    if(it) // Si le pointeur d'itérateur est non nul.
        free(it); // Libère la mémoire allouée pour l'itérateur.
}

// Cette fonction réinitialise un itérateur pour qu'il pointe sur l'élément initial de la skip-list associée.
SkipListIterator skiplist_iterator_begin(SkipListIterator it){
    it->current=it->begin; // Initialise le pointeur courant de l'itérateur à l'élément initial.
    return it; // Retourne l'itérateur.
}

// Cette fonction vérifie si l'itérateur est arrivé à la fin de la SkipList en retournant vrai si le pointeur courant de l'itérateur est la sentinelle de la SkipList.
bool skiplist_iterator_end(SkipListIterator it){
    return(it->current==it->d->sentinel); // Retourne vrai si le pointeur courant de l'itérateur est la sentinelle de la SkipList.
}

// Cette fonction renvoie l'itérateur suivant en appelant la fonction "next" de l'itérateur sur l'itérateur courant.
SkipListIterator skiplist_iterator_next(SkipListIterator it){
    return (it->next)(it); // Retourne l'itérateur suivant en appelant la fonction "next" de l'itérateur sur l'itérateur courant.
}

// Cette fonction renvoie la valeur de l'élément courant de l'itérateur s'il n'est pas la sentinelle de la SkipList, sinon elle renvoie 0.
int skiplist_iterator_value(SkipListIterator it){
    if(it->current!=it->d->sentinel) // Si le pointeur courant de l'itérateur n'est pas la sentinelle de la SkipList.
        return it->current->value; // Retourne la valeur de l'élément courant.
    return 0; // Sinon, retourne 0.
}

// Cette fonction supprime un élément de la SkipList.
SkipList skiplist_remove(SkipList d, int value){
    Node* current=d->sentinel->next[0],*prev; // Initialise un pointeur d'élément à l'élément suivant de la sentinelle de la SkipList, ainsi qu'un pointeur d'élément de précedent.
    while(current!=d->sentinel&&current->value!=value){ // Tant que le pointeur courant n'est pas la sentinelle de la SkipList et que la valeur de l'élément courant n'est pas égale à la valeur donnée.
        current=current->next[0]; // Déplace le pointeur courant au suivant.
    }
    if(current!=d->sentinel){ // Si le pointeur courant n'est pas la sentinelle de la SkipList.
        unsigned int compteur=0; // Initialise un compteur et un indice i.
        unsigned int  i=0;
        prev=current; // Définit le pointeur de précedent au pointeur courant.
        while(compteur<current->level){ // Tant que le compteur est inférieur au niveau de l'élément courant.
            prev=prev->prev; // Déplace le pointeur de précedent à l'élément précédent.
            for(i=compteur;i<prev->level&&compteur<current->level;++i){ // Pour chaque indice i supérieur ou égal au compteur et inférieur au niveau de l'élément précédent et courant.
                compteur++; // Incrémente le compteur.
                prev->next[i]=current->next[i]; // Met à jour le pointeur suivant de l'élément précédent à l'indice i avec le pointeur suivant de l'élément courant à l'indice i.
            }
        }
        current->next[0]->prev=current->prev; // Met à jour le pointeur précédent de l'élément suivant de l'élément courant avec le pointeur précédent de l'élément courant.
        d->size--; // Décrémente la taille de la SkipList.
        free(current->next); // Libère la mémoire allouée pour les pointeurs suivants de l'élément courant.
        free(current); // Libère la mémoire allouée pour l'élément courant.
    }
    return d; // Retourne la SkipList modifiée.
}
