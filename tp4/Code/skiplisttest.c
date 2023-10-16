#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "skiplist.h"

/**
 *	@defgroup SkipListTest Test program for SkipList Implantation
 *  @brief Definition of the SkipList type and operators
 *  @{
 */

/** Print a usage message for this program.
 
 @code{.unparsed}
 $skiplisttest -id num
 where id is :
 c : construct and print the skiplist with data read from file ../Test/test_files/construct_num.txt
 s : construct the skiplist with data read from file ../Test/test_files/construct_num.txt and search elements from file test_files/search_num..txt
 Print statistics about the searches.
 i : construct the skiplist with data read from file ../Test/test_files/construct_num.txt and search, using an iterator, elements read from file test_files/search_num.txt
 Print statistics about the searches.
 r : construct the skiplist with data read from file test_files/construct_num.txt, remove values read from file test_files/remove_num.txt and print the list in reverse order
 
 and num is the file number for input.
 @endcode
 */
void usage(const char *command) {
	printf("usage : %s -id num\n", command);
	printf("where id is :\n");
	printf("\tc : construct and print the skiplist with data read from file test_files/construct_num.txt\n");
	printf("\ts : construct the skiplist with data read from file test_files/construct_num.txt and search elements from file test_files/search_num..txt\n\t\tPrint statistics about the searches.\n");
	printf("\ti : construct the skiplist with data read from file test_files/construct_num.txt and search, using an iterator, elements read from file test_files/search_num.txt\n\t\tPrint statistics about the searches.\n");
	printf("\tr : construct the skiplist with data read from file test_files/construct_num.txt, remove values read from file test_files/remove_num.txt and print the list in reverse order\n");
	printf("and num is the file number for input\n");
}

/** Return the filename associated with the action to perform and the number of a test.
 
 @param action Describe the action associated with the file to load. Must be one of "construct" "search" or "remove"
 @param filenumber Number of the file to load. Corresponds to the number of the test to perform.
 @return fileame  in a relative form
 
 @code{.c}
 char *filename = gettestfilename("construct", 2);
 @endcode
 will set filename to @code{.c}"../Test/test_files/construct_2.txt"@endcode
 
 */
char *gettestfilename(const char *action, int numtest) {
	const char *testdir ="../Test/test_files/";
	int l = strlen(testdir) + strlen(action) + strlen("_xxx.txt");
	char *name = malloc(l*sizeof(char));
	sprintf(name, "%s%s_%d.txt", testdir, action, numtest);
	return name;
}

/** Build a list corresponding to the fiven file number.
 */
SkipList buildlist(int num) {
	SkipList d;
	FILE *input;
	unsigned int level;
	unsigned int nb_values;
	int value;
	
	char *constructfromfile = gettestfilename("construct", num);
	input = fopen(constructfromfile, "r");
	if (input!=NULL) {
		
		fscanf(input, "%u", &level);
		d = skiplist_create(level);
		
		fscanf(input, "%u", &nb_values);
		for (unsigned int i=0;i< nb_values; ++i) {
			fscanf(input, "%d", &value);
			d = skiplist_insert(d, value);
		}
	} else {
		printf("Unable to open file %s\n", constructfromfile);
		free(constructfromfile);
		exit (1);
	}
	free(constructfromfile);
	fclose(input);
	return d;
}

/*----------------------------------------------------------------------------------------------*/

/** Exercice 1.
 	Programming and test of skiplist construction.
 */
void test_construction(int num){
    SkipList d=buildlist(num); // création d'une SkipList d'après l'argument num
    unsigned int size=skiplist_size(d); // récupération de la taille de la SkipList d
    printf("Skiplist (%u)\n",size); // affichage du "Skiplist (taille)" où taille est remplacé par la variable taille
    for(unsigned int i=0;i<size;++i){ // boucle sur tous les éléments de la SkipList d
        printf("%d ",skiplist_ith(d,i)); 
    }
    skiplist_delete(d); // supprimer la SkipList d
    printf("\n");
}

/** Exercice 2.
 Programming and test of skiplist search operator.
 */
void test_search(int num){
    SkipList d=buildlist(num); // création d'une SkipList d'après l'argument num
    FILE *input; 
    unsigned int nb_values; 
    int value;
    char *constructfromfile = gettestfilename("search", num); // récupération le nom du fichier de test
    input = fopen(constructfromfile, "r"); // ouvrir constructfromfile en lecture
    if (input!=NULL) { // si input n'est pas nul alors :
        fscanf(input, "%u", &nb_values); // lire le nombre de valeurs à chercher dans le fichier
        unsigned int *tabres=malloc(nb_values*sizeof(unsigned int)); // allouer un tableau de nb_values entiers non signés
        bool res;
        int compteur=0;
        for (unsigned int i=0; i < nb_values; ++i) {
            fscanf(input, "%d", &value); // lire la prochaine valeur à chercher dans le fichier
            tabres[i]=0; // initialiser le nombre d'opérations à 0 pour cette valeur
            res = skiplist_search(d, value, &tabres[i]); // chercher la valeur dans la SkipList
            if(res)
                ++compteur; // incrémenter le compteur si la valeur est trouvée
            printf("%d -> ",value ); // afficher le résultat de la recherche
            printf("%s", res     ? "true\n" : "false\n"); 
        }
        unsigned int max=tabres[0];
        unsigned int min=tabres[0];
        unsigned int moy=0;
        for(unsigned int i=0;i<nb_values;++i){ // calculer les statistiques sur le nombre d'opérations nécessaires
            if(tabres[i]>max)
                max=tabres[i];
            if(tabres[i]<min)
                min=tabres[i];
            moy+=tabres[i];
        }
        moy=moy/nb_values;
        printf("Statistics :\n\tSize of the list : %u\nSearch %u values :\n\tFound %u\n\tNot found %u\n\tMin number of operations : %u\n\tMax number of operations : %u\n\tMean number of operations : %u\n", skiplist_size(d),nb_values,compteur,nb_values-compteur,min,max,moy);
    } else {
        printf("Unable to open file %s\n", constructfromfile);
        free(constructfromfile); // libérer la mémoire allouée pour constructfromfile
        exit (1); // quitter le programme avec un code d'erreur
    }
    free(constructfromfile); // libérer la mémoire allouée pour constructfromfile
    fclose(input); // fermer le fichier
}


/** Exercice 3.
 Programming and test of naïve search operator using iterators.
 */
void test_search_iterator(int num){
    SkipList d=buildlist(num); // Création d'une SkipList de taille num
    FILE *input; 
    unsigned int nb_values;
    int value;
    char *constructfromfile = gettestfilename("search", num);
    input = fopen(constructfromfile, "r"); // Ouverture d'un fichier contenant les valeurs à chercher

    if (input!=NULL) { // Si le fichier est ouvert avec succès
        fscanf(input, "%u", &nb_values); // Lecture du nombre de valeurs à chercher    
        unsigned int *tabres=malloc(nb_values*sizeof(unsigned int)); // Allocation dynamique d'un tableau de résultats
        bool res; // Variable booléenne pour stocker le résultat de la recherche
        int compteur=0; // Compteur de recherche
        SkipListIterator e; // Création d'un itérateur pour parcourir la SkipList
        e = skiplist_iterator_create (d ,FORWARD_ITERATOR ) ;
        
        for (unsigned int i=0;i< nb_values; ++i) { // Pour chaque valeur à chercher    
            fscanf(input, "%d", &value); // Lecture de la valeur à chercher
            tabres[i]=0; // Initialisation du nombre de pas effectués pour la recherche de cette valeur
            e = skiplist_iterator_begin (e); // Positionnement de l'itérateur au début de la SkipList
            tabres[i]++; // Incrémentation du nombre de pas effectués
    
            while(!skiplist_iterator_end(e) && value!=skiplist_iterator_value(e) ){ // Tant que la fin de la SkipList n'est pas atteinte et que la valeur cherchée n'est pas trouvée
                e=skiplist_iterator_next (e); // Passage à l'élément suivant de la SkipList
                tabres[i]++; // Incrémentation du nombre de pas effectués
            }
            
            if(value==skiplist_iterator_value(e)){ // Si la valeur cherchée est trouvée
                res=true; // Stockage du résultat de la recherche
                ++compteur; // Incrémentation du compteur de recherche
            }else{ // Si la valeur cherchée n'est pas trouvée
                tabres[i]--; // Décrémentation du nombre de pas effectués pour cette recherche
                res=false; // Stockage du résultat de la recherche
            }
            
            printf("%d -> ", value); // Affichage de la valeur cherchée et du résultat de la recherche
            printf("%s", res ? "true\n" : "false\n");
        }

        // Calcul des statistiques sur les résultats des recherches
        unsigned int max=tabres[0];
        unsigned int min=tabres[0];
        unsigned int moy=0;
        for(unsigned int i=0; i<nb_values; ++i){
            if(tabres[i]>max)
                max=tabres[i];
            if(tabres[i]<min)
                min=tabres[i];
            moy+=tabres[i];
        }
        moy=moy/nb_values;
        // Affichage des statistiques
        printf("Statistics :\n\tSize of the list : %u\nSearch %u values :\n\tFound %u\n\tNot found %u\n\tMin number of operations : %u\n\tMax number of operations : %u\n\tMean number of operations : %u\n", skiplist_size(d), nb_values, compteur, nb_values-compteur, min, max, moy);
    }else{// Si le fichier n'a pas pu être ouvert
        printf("Unable to open file %s\n", constructfromfile);
        free(constructfromfile);
        exit (1);
    }
    free(constructfromfile); // Libération de la mémoire allouée
    fclose(input);
}

/** Exercice 4.
 Programming and test of skiplist remove operator.
 */
void test_remove(int num){
    SkipList d = buildlist(num); // Construire une skip list de taille 'num'
    FILE* remove;
    char* file_name = gettestfilename("remove", num);
    if(!(remove = fopen(file_name, "rt"))){ // Ouvrir le fichier de test pour les suppressions
        perror(file_name); // Afficher un message d'erreur si le fichier ne peut pas être ouvert
        exit(1);
    }
    unsigned int size; // Lire la première ligne du fichier, qui contient le nombre de valeurs à supprimer
    int value;
    fscanf(remove, "%u", &size);
    while(size){ // Supprimer chaque valeur lue dans le fichier de test
        fscanf(remove, "%d", &value);
        d = skiplist_remove(d, value);
        size--;
    }
    fclose(remove); // Fermer le fichier de test
    size = skiplist_size(d); // Obtenir la taille de la skip list après les suppressions
    printf("Skiplist (%u)\n", size); // Afficher les éléments de la skip list, en partant du dernier
    for(unsigned int i = 0; i < size; ++i){
        printf("%d ", skiplist_ith(d, size - i - 1));
    }
    skiplist_delete(d); // Supprimer la skip list
    printf("\n");
}


/** Function you can use to generate dataset for testing.
 */
void generate(int nbvalues);


int main(int argc, const char *argv[]){
	if (argc < 3) {
		usage(argv[0]);
		return 1;
	}
	switch (argv[1][1]) {
		case 'c' :
			test_construction(atoi(argv[2]));
			break;
		case 's' :
			test_search(atoi(argv[2]));
			break;
		case 'i' :
			test_search_iterator(atoi(argv[2]));
			break;
		case 'r' :
			test_remove(atoi(argv[2]));
			break;
		case 'g' :
			generate(atoi(argv[2]));
			break;
		default :
			usage(argv[0]);
			return 1;
	}
	return 0;
}


/* Generates a set of test files for a given number of value. */

void generate(int nbvalues){
	FILE *output;
	int depth;
	int maxvalue;
	output = fopen("construct.txt", "w");
	srand(nbvalues);
	depth = rand()%16;
	maxvalue = rand()%10 * nbvalues;
	fprintf(output, "%d\n%d\n", depth, nbvalues);
	for (int i=0; i< nbvalues; ++i) {
		fprintf(output, "%d\n", rand()%maxvalue);
	}
	fclose(output);
	output = fopen("search.txt", "w");
	srand(rand());
	nbvalues *= depth/4;
	fprintf(output, "%d\n", nbvalues);
	for (int i=0; i< nbvalues; ++i) {
		fprintf(output, "%d\n", rand()%maxvalue);
	}
	fclose(output);
}


/** @} */

/**
 @mainpage
 @section TP45 Implantation du TAD Listes à raccourci.
 @subsection SkipLists SkipLists
 Les listes à raccourcis, nommées SkipLists, sont une alternative aux arbres de
 recherche équilibrés que nous verrons dans la suite du cours.
 
 Inventées par William Pugh en 1902, elles reposent sur une structure de données
 linéaire construite de manière probabiliste.
 Les opérations de dictionnaire définies sur les listes à raccourcis sont simples à
 programmer, élégantes, et l’aspect probabiliste permet de démontrer une complexité
 moyenne en O(log(N)) au lieu des O(N) inhérents aux structures linéaires.
 
 Toutefois, l’aspect probabiliste de la structure de données ne permet pas d’assurer
 la complexité en pire cas, comme on pourra le faire sur les arbres équilibrés, mais
 donne, en pratique, une structure extrêmement efficace (la probabilité de construire
 une structure non efficace étant très faible). Nous ne ferons pas de démonstration
 ici, et il ne vous est pas demandé d’en effectuer une, les étudiants curieux
 pourront se rapporter à leur cours de complexité et à l’analyse fournie dans
 l’article de William Pugh, accessible sur Moodle et sur la <a href="https://dl.acm.org/citation.cfm?id=78977"> bibliothèque ACM</a>.
 
 @note \anchor WP-skiplist-1990 William Pugh, <a href="https://dl.acm.org/citation.cfm?id=78977">Skip lists : a probabilistic alternative to balanced trees</a> in Communications of the ACM, June 1990, 33(6) 668-676
 
 @subsection Objectifs Objectifs du TP
 - Implantation du TAD SkipList
 - Implantation d'un itérateur sur une SkipList
 - Comparaison empirique des opérations de dictionnaires sur les SkipList et en utilisant un itérateur.
 
 */
