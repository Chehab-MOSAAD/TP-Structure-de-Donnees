/*-----------------------------------------------------------------*/
/*
 Licence Informatique - Structures de données
 Mathias Paulin (Mathias.Paulin@irit.fr)
 
 Implantation du TAD List vu en cours.
 */
/*-----------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "list.h"

typedef struct s_LinkedElement {
	int value;
	struct s_LinkedElement *previous;
	struct s_LinkedElement *next;
} LinkedElement;

/* Use of a sentinel for implementing the list :
 The sentinel is a LinkedElement * whose next pointer refer always to the head of the list and previous pointer to the tail of the list
 */
struct s_List {
	LinkedElement *sentinel;
	int size;
};

/*****************************************************************************/
List *list_create() { /*Cette fonction crée une nouvelle liste vide avec un élément sentinelle initialisé pour marquer le début et la fin de la liste.*/
	List *l = (List *) malloc(sizeof(List)); /*on alloue de la mémoire pour la nouvelle liste.*/
	l->sentinel = (LinkedElement *) malloc(sizeof(LinkedElement)); /*on crée la sentinelle.*/
	l->sentinel->next = l->sentinel->previous = l->sentinel; /*les pointeurs next et previous pointent vers la sentinelle car la liste est vide lors de sa création.*/
	l->size = 0; /*on initialise size à 0 car la liste est vide.*/
	return l; /*on retourne un pointeur vers la nouvelle liste.*/
}

List *list_push_back(List *l, int v) { /*Cette fonction ajoute un nouvel élément contenant la valeur v à la fin de la liste l.*/
	LinkedElement *new_element = malloc(sizeof(LinkedElement)); /*on alloue de la mémoire pour le nouvel élément.*/
	new_element->value = v; /*on assigne la variable v à l'attribut value du nouvel élément.*/
	new_element->next = l->sentinel; /*mise à jour des pointeurs next et previous de la sentinelle.*/
	new_element->previous = l->sentinel->previous;
	l->sentinel->previous->next = new_element;
	l->sentinel->previous = new_element;
	++l->size; /*mise à jour du variable size (longueur de la liste).*/
	return l; /*on retourne un pointeur vers la liste modifiée.*/
}

List * list_map(List *l, SimpleFunctor f) { /*Cette fonction applique la fonction f à chaque élément de la liste l.*/
	for(LinkedElement *e = l->sentinel->next ; e != l->sentinel; e = e->next){ /*on parcours la liste. */
		e->value = f(e->value); /*on remplace la valeur d'origine de l'élément par la valeur retournée par f.*/
	}
	return l; /*on retourne un pointeur vers la liste modifiée.*/
}

void list_delete(ptrList *l) { /*Cette fonction supprime une liste et libère la mémoire allouée.*/
	free(*l);
	*l=NULL;
}

bool list_is_empty(List *l) { /*Cette fonction retoune vraie si une liste est vide*/
	return l->size == 0;
}

int list_size(List *l) { /*Cette fonction retoune la taille de la liste*/
	return l->size;
}

/*****************************************************************************/

List *list_push_front(List *l, int v) { /*Cette fonction ajoute un nouvel élément au début d'une liste */
	LinkedElement *new_element = malloc(sizeof(LinkedElement)); /*on alloue de la mémoire pour le nouvel élément.*/
	new_element->value = v; /*on assigne la variable v à l'attribut value du nouvel élément.*/
	new_element->next = l->sentinel->next; /*mise à jour des pointeurs next et previous de la sentinelle.*/
	new_element->previous = l->sentinel;
	l->sentinel->next->previous = new_element;
	l->sentinel->next = new_element;
	++l->size; /*on incrémente la taille de la liste*/
	return l; /*on retourne un pointeur vers la liste modifiée.*/
}

List *list_reduce(List *l, ReduceFunctor f, void *userData) { /*Cette fonction applique une fonction de réduction à tous les éléments d'une liste */
	for(LinkedElement *e = l->sentinel->next ; e != l->sentinel; e = e->next){ /*on parcours la liste. */
		e->value =f(e->value, userData); /*on remplace la valeur d'origine de l'élément par la valeur retournée par f.*/
	}
	return l; /*on retourne un pointeur vers la liste modifiée.*/
}

/*****************************************************************************/

int list_front(List *l) { /*Cette fonction retourne la valeur du premier élément d'une liste*/
	return l->sentinel->next->value;
}

int list_back(List *l) { /*Cette fonction retourne la valeur du dernier élément d'une liste*/
	return l->sentinel->previous->value;
}

List *list_pop_front(List *l) { /*Cette fonction supprime le premier élément d'une liste*/
	l->sentinel->next = l->sentinel->next->next; /*on met à jour le pointeur next de la sentinelle pour qu'il pointe vers le deuxième élément. */
	l->sentinel->next->previous = l->sentinel; /*on met à jour le pointeur previous du nouveau premier élément pour qu'il pointe vers la sentinelle. */
	--l->size; /*on décrémente la taille de la liste */
	return l; /*on retourne un pointeur vers la liste mise à jour.*/
}

List *list_pop_back(List *l){ /*Cette fonction supprime le dernier élément d'une liste*/
	l->sentinel->previous = l->sentinel->previous->previous; /*on met à jour le pointeur previous de la sentinelle pour qu'il pointe vers le deuxième dernier élément. */
	l->sentinel->previous->next = l->sentinel; /*on met à jour le pointeur next du nouveau dernier élément pour qu'il pointe vers la sentinelle. */
	--l->size; /*on décrémente la taille de la liste */
	return l; /*on retourne un pointeur vers la liste mise à jour.*/
}

/*****************************************************************************/

List *list_insert_at(List *l, int p, int v) { /*Cette fonction insére une nouvelle valeur à une position spécifique dans une liste*/
	LinkedElement *insert_at = l->sentinel;
	for(int i = 0 ; i  < p ; ++i ){ /*on commence à la sentinelle et on avance dans la liste jusqu'à atteindre l'endroit où l'on souhaite insérer la nouvelle valeur. */
		insert_at = insert_at->next;
	}
	LinkedElement *new_element = malloc(sizeof(LinkedElement)); /*on alloue de la mémoire pour le nouvel élément à insérer. */
	new_element->value = v; /*on lui donne la valeur v. */
	new_element->next = insert_at->next; /*on met à jour les pointeurs next et previous de l'élément à insérer. */
	new_element->previous = insert_at;
	insert_at->next->previous = new_element; /*on met à jour les pointeurs next et previous des éléments suivant et précédent le nouvel élément. */
	insert_at->next = new_element;
	++l->size; /*on incrémente la taille de la liste. */
	return l; /*on retourne un pointeur vers la liste modifiée. */
}

List *list_remove_at(List *l, int p) { /*Cette fonction supprime l'élément situé à la position p dans la liste l.*/
	LinkedElement *remove_at = l->sentinel->next;
	for(int i = 0 ; i  < p ; ++i ){ /*on commence à la sentinelle et on avance dans la liste jusqu'à atteindre l'élément à supprimer.*/
		remove_at = remove_at->next;
	}
	remove_at->next->previous = remove_at->previous; /*on met à jour les pointeurs next et previous des éléments suivant et précédent l'élément à supprimer.*/
	remove_at->previous->next = remove_at->next;
	--l->size;  /*on décrémente la taille de la liste. */
	return l; /*on retourne un pointeur vers la liste modifiée. */
}

int list_at(List *l, int p) { /*Cette fonction renvoie la valeur de l'élément à l'index p de la liste. */
	LinkedElement *value_at; /*on crée un pointeur vers un élément. */
	value_at = l->sentinel->next; /*on initialise ce pointeur au premier élément de la liste (après la sentinelle). */
	for(int i = 0 ; i  < p ; ++i ){ /*on boucle p fois pour se déplacer dans la liste jusqu'à l'élément à l'index p. */
		value_at = value_at->next;
	}
	return value_at->value; /*on renvoie la valeur de l'élément à l'index p. */
}

/*****************************************************************************/

typedef struct s_SubList{
	LinkedElement *head;
	LinkedElement *tail;
	int size;
} SubList;

SubList *SubList_create(){ /*Cette fonction crée une nouvelle sous-liste vide. */
	SubList * l = malloc(sizeof(SubList)); /*on alloue de la mémoire pour la sous-liste. */
	l->head = l->tail = NULL;  /*on initialise les pointeurs de tête et de queue à NULL. */
	l->size = 0; /*on initialise la taille de la sous-liste à 0. */
	return l; /*on retourne un pointeur vers la sous-liste créée. */
}

SubList *SubList_push_back(SubList *l , int v){ /* Cette fonction insère un nouvel élément à la fin d'une sous-liste. */
	LinkedElement **e = (l->size ? &(l->tail->next) : &(l->head)); /*on crée un double pointeur "e" qui pointe sur l'adresse de la prochaine case disponible dans la sous-liste pour stocker un nouvel élément. */
	*e = malloc (sizeof(LinkedElement)); /*on alloue de la mémoire pour un nouvel élément et on stocke son adresse dans "e". */
	(*e)->value = v ; /*on initialise la valeur de l'élément à la valeur donnée en entrée. */
	(*e)->next = NULL; /*on initialise le pointeur "next" de l'élément à NULL, car c'est le dernier élément de la sous-liste. */
	(*e)->previous = l->tail; /*on relie l'élément précédent à l'élément actuel en initialisant le pointeur "previous" de l'élément actuel avec la queue de la sous-liste. */
	l->tail = *e; /*on met à jour la queue de la sous-liste en initialisant le pointeur "tail" avec l'élément actuel. */
	++(l->size); /*on incrémente la taille de la sous-liste. */
	return l; /*on retourne un pointeur vers la sous-liste mise à jour. */
}

int SubList_size(SubList *l) { /*Cette fonction retoune la taille de la sous-liste*/
	return l->size;
}

void Sublist_delete(SubList ** l){ /*Cette fonction supprime la sous-liste et libère la mémoire allouée.*/
	free(*l);
	*l = NULL;
}

void Sublist_print(SubList *l){ /*Cette fonction affiche les valeurs des éléments de la sous-liste.*/
	int size = l->size; /*on récupère la taille de la sous-liste. */
	LinkedElement  *SubList_at = l->head; /*on récupère l'élément de tête de la sous-liste. */
	while(size){ /*Tant qu'il y a des éléments dans la sous-liste. */
		printf("%d ",SubList_at->value); /*on affiche la valeur de l'élément courant. */
		SubList_at = SubList_at->next; /*on passe à l'élément suivant. */
		--size; /*on décrémente la taille de la sous-liste restante. */
	}
	printf("\n"); /*on affiche une nouvelle ligne pour finir l'affichage. */
}

SubList *list_split(SubList * l){ /*Cette fonction divise la liste l en deux sous-listes.*/
	SubList * split_Sublist = SubList_create(); /*on crée une nouvelle sous-liste pour stocker la seconde moitié de la liste originale. */
	int half = l->size / 2 - 1; /*on trouve l'indice de l'élément au milieu de la liste originale (arrondi vers le bas). */
	split_Sublist->size = half + 1; /*on met à jour la taille de la nouvelle sous-liste. */
	LinkedElement  *SubList_at = l->head; /*on initialise un pointeur pour parcourir la liste originale à partir de la tête. */
	while(half){ /*on parcourt la liste jusqu'à l'élément du milieu. */
		SubList_at = SubList_at->next;
		--half;
	}
	split_Sublist->head = SubList_at; /*on met à jour la tête de la nouvelle sous-liste pour qu'elle pointe vers l'élément du milieu. */
	split_Sublist->tail = SubList_at->next; /*on met à jour la queue de la nouvelle sous-liste pour qu'elle pointe vers l'élément suivant l'élément du milieu. */

	return split_Sublist; /*on retourne un pointeur vers la nouvelle sous-liste*/
}

SubList *list_merge(SubList *leftlist, SubList *rightlist, OrderFunctor f){ /*Cette fonction renvoie une nouvelle sous-liste triée qui contient tous les éléments des deux sous-listes d'entrée triées selon l'ordre spécifié par la fonction d'ordre. */
	SubList * mergedList = SubList_create(); /*on crée une nouvelle sous-liste pour stocker les éléments fusionnés. */
	LinkedElement * right_current  =  rightlist->head ; /*on initialise le pointeur right_current à la tête de la sous-liste droite.*/
	LinkedElement * left_current  = leftlist->head; /*on initialise le pointeur left_current à la tête de la sous-liste gauche. */

	while(right_current != NULL && left_current != NULL){ /*Tant que les deux sous-listes ne sont pas vide. */
		if( f(left_current->value, right_current->value) ) { /*Si la valeur de l'élément actuel de la sous-liste gauche est inférieure à celle de la sous-liste droite selon l'ordre spécifié par la fonction d'ordre. */
			SubList_push_back(mergedList , left_current->value); /*on ajoute l'élément de la sous-liste gauche à la sous-liste fusionnée. */
			left_current = left_current->next; /*on avance l'indice de la sous-liste gauche vers l'élément suivant. */
		}else{
			SubList_push_back(mergedList , right_current->value); /*Sinon, on ajoute l'élément de la sous-liste droite à la sous-liste fusionnée. */
			right_current = right_current->next; /*on avance l'indice de la sous-liste droite vers l'élément suivant. */
		}
	}
	if(right_current && ! left_current){ /*Si la sous-liste droite n'est pas vide et la sous-liste gauche est vide. */
		while (right_current) { /*on ajoute tous les éléments restants de la sous-liste droite à la sous-liste fusionnée. */
			SubList_push_back(mergedList , right_current->value);
			right_current = right_current->next;
		}
	}else if(left_current && ! right_current){ /*Sinon, si la sous-liste gauche n'est pas vide et la sous-liste droite est vide. */
		while (left_current) { /*on ajoute tous les éléments restants de la sous-liste gauche à la sous-liste fusionnée. */
			SubList_push_back(mergedList , left_current->value);
			left_current = left_current->next;
		}
	}
	return mergedList; /*on retourne la sous-liste fusionnée triée. */
}

SubList *list_mergesort(SubList *l, OrderFunctor f) { /*C'est une fonction de tri fusion pour une sous-liste. */
	if (l->size > 1) { /*Si la taille de la sous-liste est supérieure à 1. */
		SubList *separator = list_split(l); /*on sépare de la sous-liste en deux. */
		SubList *leftlist = SubList_create(); /*on crée de la sous-liste gauche à partir de la tête de la liste originale jusqu'à la fin de la première partie. */
		leftlist->head = l->head;
		leftlist->tail = separator->head;
		leftlist->tail->next = NULL;
		leftlist->size = separator->size;
		
		SubList *rightlist = SubList_create(); /*on crée la sous-liste droite à partir de la fin de la première partie jusqu'à la fin de la liste originale. */
		rightlist->head = separator->tail;
		rightlist->tail = l->tail;
		rightlist->tail->next = NULL;
		rightlist->size = separator->size + (l->size % 2);
		Sublist_delete(&separator); /*on supprime de la sous-liste séparatrice, qui n'est plus utile. */
		SubList *merged_list = list_merge(list_mergesort(leftlist, f), list_mergesort(rightlist, f), f); /*on appelle récursivement la fonction list_mergesort sur les deux sous-listes et on fusionne les résultats. */
		Sublist_delete(&leftlist); /*on supprime de la sous-liste gauche et droite, qui ne sont plus utiles. */
		Sublist_delete(&rightlist);
		return merged_list; /*on retourne la sous-liste triée résultante. */
	} else { /*Si la sous-liste a une taille de 1 ou moins donc elle est déjà triée. */
		return l; /*on retourne le pointeur vers la sous-liste. */
	}
}

List *list_sort(List *l, OrderFunctor f) { /*Cette fonction trie la liste l en utilisant l'algorithme de tri fusion (mergesort).*/
	SubList * sl = SubList_create(); /*on crée une sous-liste à partir de la liste d'entrée.*/
	LinkedElement * add = l->sentinel->next; /*on initialise un pointeur vers le premier élément de la liste d'entrée.*/
	while(add != l->sentinel){ /*on parcourt la liste d'entrée jusqu'à la sentinelle.*/
		sl = SubList_push_back(sl, add->value); /*on ajoute chaque élément de la liste d'entrée dans la sous-liste.*/
		add = add->next; /*on passe à l'élément suivant de la liste d'entrée.*/
	}
	sl = list_mergesort(sl,f); /*on applique l'algorithme de tri fusion sur la sous-liste.*/
	LinkedElement * modify = l->sentinel->next; /*on initialise un pointeur vers le premier élément de la liste d'entrée.*/
	LinkedElement * newValue = sl->head; /*on initialise un pointeur vers le premier élément de la sous-liste triée.*/
	while(modify != l->sentinel){ /*on parcourt la liste d'entrée jusqu'à la sentinelle.*/
		modify->value = newValue->value; /*on remplace la valeur de chaque élément de la liste d'entrée par la valeur correspondante de la sous-liste triée.*/
		modify = modify->next; /*on passe à l'élément suivant de la liste d'entrée.*/
		newValue = newValue->next; /*on passe à l'élément suivant de la sous-liste triée.*/
	}
	return l; /*on retourne la liste d'entrée triée.*/
}

/*
bool lt(int i, int j) {
	return i<j;
}

int main(){
	SubList * l = SubList_create();
	SubList_push_back(l,5);
	SubList_push_back(l,1);
	SubList_push_back(l,4);
	SubList_push_back(l,32);
	SubList_push_back(l,12);
	SubList_push_back(l,20);
	SubList_push_back(l,0);
	SubList_push_back(l,16);
	SubList_push_back(l,55);
	SubList_push_back(l,9);
	SubList_push_back(l,70);

	List * mergedList = list_sort(l,lt);

	Sublist_print(mergedList);
	Sublist_delete(&mergedList);
	Sublist_delete(&l);
	return 0;
}*/