// Need this to use the getline C function on Linux. Works without this on MacOs. Not tested on Windows.
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "token.h"
#include "queue.h"
#include "stack.h"

bool isSymbol(char c){ /*renvoie un booléen indiquant si le caractère est l'un des symboles d'opérateur ou de parenthèse (vrai si c'est le cas et faux sinon).*/
    return c == '+' || c =='-' || c == '*' || c == '/' || c == '^' || c == '(' || c == ')';
}

Queue *stringToTokenQueue(const char *expression){
    Queue * token_queue = createQueue(); /* Crée une nouvelle file vide pour stocker les tokens. */
    const char *curpos = expression; /* Initialise le pointeur de caractère courant pour pointer vers le début de la chaîne. */
    int lg = 1; /* Initialise la longueur du token courant à 1. */

    while( *(curpos) != '\0' ){ /* Tant que nous ne sommes pas à la fin de la chaîne. */
        while( ( *(curpos) == '\n' || *(curpos) == ' '  ) ){ /* Ignore les espaces et les retours à la ligne. */
            ++curpos;
        }

        if ( *(curpos) != '\0' ){ /* Si nous n'avons pas atteint la fin de la chaîne, c'est que nous avons trouvé un token. */
            const char * char_num_courant = curpos + 1; /* Initialise un pointeur de caractère pour pointer vers le caractère suivant le caractère courant. */

            /* Si le caractère courant n'est pas un symbole et que le caractère suivant n'est pas un symbole, cela signifie que nous avons trouvé un nombre. */
            if(! isSymbol( *(curpos) ) && ! isSymbol( *(char_num_courant) )){
                while(! isSymbol( *(char_num_courant)) && *(char_num_courant) != '\0' ){ /* Tant que nous ne trouvons pas un symbole ou la fin de la chaîne, augmenter la longueur du token. */
                    ++lg;
                    ++char_num_courant;
                }
            }

            Token *token_cur = createTokenFromString(curpos,lg); /* Crée un nouveau token à partir de la sous-chaîne actuelle et ajoute-le à la file de tokens. */
            queuePush(token_queue, token_cur); /* Ajoute le token à la file de tokens. */
            curpos += lg; /* Passe à la prochaine sous-chaîne. */
            lg = 1; /* Réinitialise la longueur du token courant à 1. */
        }
    }
    return token_queue; /* Retourne la file de tokens. */
} 

void printToken(FILE  *f, void*e){
	Token* t  = (Token*)e; /*La fonction convertit d'abord le pointeur void en un pointeur Token et le stocke dans une variable locale t. */
	if(tokenIsNumber(t)){ /*Vérifie si la variable locale t est un nombre: */ 
		fprintf(f," %f ", tokenGetValue(t)); /*La fonction affiche le nombre. */
	}else if(tokenIsOperator(t)){ /*ou si t est un opérateur: */
		fprintf(f," %c ", tokenGetOperatorSymbol(t)); /*La fonction affiche l'opérateur. */
	}else if(tokenIsParenthesis(t)){ /*ou si t est une parenthèse: */
		fprintf(f," %c ", tokenGetParenthesisSymbol(t)); /*La fonction affiche la parenthèse. */
	}
}

Queue *shuntingYard(Queue* infix){
	Queue *outputQueue  = createQueue();/*file de sortie contenant les Tokens de l'expression postfixée, à libérer*/
	Stack *operatorStack  = createStack(30);/*pile contenant les opérateurs et les parenthèses, STACK_SIZE = 30*/

	while(! queueEmpty(infix)){/* tant que la file d'entrée contient encore des Tokens */
		Token * t = (Token *)queueTop(infix);
		queuePop(infix);
		if(tokenIsNumber(t)){/* si le Token est un nombre, on le met directement dans la file de sortie */
			queuePush(outputQueue, t);
		}else if(stackEmpty(operatorStack)){/* si la pile d'opérateurs est vide, on empile le Token */
			stackPush(operatorStack, t);
		}else{/* sinon, on traite les Tokens en fonction de leur type */
			if(tokenIsOperator(t)){/* si le Token est un opérateur */
				if (!stackEmpty(operatorStack)) {/* et si la pile d'opérateurs n'est pas vide */
					/* on compare la priorité de l'opérateur courant avec celle de l'opérateur en haut de pile, en fonction de leur associativité */
					bool comparePriority_on_operator = (!stackEmpty(operatorStack)) && ( !tokenIsParenthesis(stackTop(operatorStack)) && ( tokenGetOperatorPriority(stackTop(operatorStack)) >= tokenGetOperatorPriority(t) ) && tokenOperatorIsLeftAssociative(t) );
				while ( comparePriority_on_operator ){/* tant que l'opérateur en haut de pile a une priorité supérieure ou égale */
					queuePush(outputQueue, stackTop(operatorStack));/* on dépile l'opérateur et on le met dans la file de sortie */
					stackPop(operatorStack);
					/* on compare la priorité de l'opérateur courant avec celle de l'opérateur en haut de pile, en fonction de leur associativité */
					comparePriority_on_operator = (!stackEmpty(operatorStack)) && ( !tokenIsParenthesis(stackTop(operatorStack)) && ( tokenGetOperatorPriority(stackTop(operatorStack)) >= tokenGetOperatorPriority(t) ) && tokenOperatorIsLeftAssociative(t) );
				}
				stackPush(operatorStack, t);/* on empile l'opérateur courant */
				}
			}
			if( tokenIsParenthesis(t) && tokenGetParenthesisSymbol(t) == '('){/* si le Token est une parenthèse ouvrante */
				stackPush(operatorStack, t);/* on empile la parenthèse ouvrante */
			}
			if ( tokenIsParenthesis(t) && tokenGetParenthesisSymbol(t) == ')'){/* si le Token est une parenthèse fermante */
				char valueToken;
				if(tokenIsParenthesis(stackTop (operatorStack))){/* on regarde le Token en haut de pile */
					valueToken = tokenGetParenthesisSymbol (stackTop(operatorStack) ); /* si c'est une parenthèse, on enregistre son symbole */
				}else{
					 valueToken = tokenGetOperatorSymbol(stackTop(operatorStack)); /* si c'est un opérateur, on enregistre son symbole */
				}
				while( valueToken != '('){ /*si le Token est une une parenthèse ouvrante, la fonction entre dans cette boucle qui continue tant que le premier élément de operatorStack n'est pas une parenthèse fermante. */
					queuePush(outputQueue, stackTop(operatorStack)); /*À chaque itération de la boucle, la fonction extrait le premier élément de operatorStack et l'ajoute sur outputQueue. */
					stackPop(operatorStack);
					if( stackEmpty(operatorStack)){
						fprintf(stderr, "Parenthèse incompatible\n"); /*Si operatorStack devient vide avant qu'une parenthèse fermante ne soit trouvée, la fonction imprime un message d'erreur*/
						exit(1);
					}
					if( tokenIsParenthesis(stackTop(operatorStack))){  /* recherche une parenthèse fermante correspondante */
						valueToken = tokenGetParenthesisSymbol (stackTop(operatorStack));
					}else if(tokenIsOperator(stackTop(operatorStack))){ /*teste si le premier élément de operatorStack est un opérateur */
						valueToken = tokenGetOperatorSymbol(stackTop(operatorStack));
					}
				}
				Token * parenthesis_difficult_to_find_for_delete = stackTop(operatorStack);
				deleteToken(&parenthesis_difficult_to_find_for_delete); /*om supprime la parenthèse ouvrante*/
				deleteToken(&t); /*on supprime la parenthèse fermante */
				stackPop(operatorStack); /*on extrait le Token de la parenthèse fermante de operatorStack.*/
			}
		}
	}
	while(! stackEmpty(operatorStack)){ /*Après avoir traité tous les Token de la file infix, la fonction entre dans une autre boucle qui continue tant que operatorStack n'est pas vide.*/
		/*vérifier que l'opérateur en haut n'est pas une parenthèse. */
		if( tokenIsParenthesis( stackTop(operatorStack) ) ){
			fprintf(stderr, "Parenthèse incompatible\n"); /*Si le premier élément de operatorStack est une parenthèse, la fonction affiche un message d'erreur et quitte le programme.*/
			exit(2);
		}
		queuePush(outputQueue, stackTop(operatorStack)); /*on extrait le premier élément de operatorStack et l'ajoute sur outputQueue.*/
		stackPop(operatorStack);
	}
	deleteStack(&operatorStack); /*on supprime le operatorStack*/
	return outputQueue;/*et on renvoie le outputQueue, qui doit contenir l'expression postfixée équivalente à l'expression infixe d'origine.*/
}

Token *evaluateOperator(Token *arg1,Token *op ,Token *arg2){
	char char_op  = tokenGetOperatorSymbol(op); /*La fonction récupère l'opérateur à appliquer sur les deux opérandes. */
	float op1 = tokenGetValue(arg1); /*La fonction récupère la valeur numérique de chaque opérande à l'aide de la fonction tokenGetValue. */
	float op2 = tokenGetValue(arg2); 
	float float_res = 0; /*Variable qui stocke le résultat */
	switch(char_op){
		case '+': /*Si l'opérateur est une addition (+), la fonction ajoute les deux opérandes. */
			float_res = op1+ op2;
			break;
			case '-': /*Si l'opérateur est une soustraction (-), la fonction soustrait le deuxième opérande du premier.*/
				float_res = op1- op2;
				break;
			case '*': /*Si l'opérateur est une multiplication (*), la fonction multiplie les deux opérandes. */
				float_res = op1* op2;
				break;
			case '/': /*Si l'opérateur est une division (/), la fonction divise le premier opérande par le deuxième. */
				float_res = op1 / op2;
				break;
			case '^': /*Si l'opérateur est une puissance (^), la fonction calcule la première opérande à la puissance de la deuxième opérande. */
				float_res = pow(op1,op2);
				break;
	}
	return createTokenFromValue(float_res); /*La fonction crée un nouveau Token en utilisant la fonction createTokenFromValue() et y stocke le résultat puis elle le retourne. */
}

float evaluateExpression(Queue* postfix){
	Stack* stackEvaluation  = createStack(30); /*Creation d'une pile vide de taille 30 qui sera utilisée pour stocker les opérandes et les résultats intermédiaires de l'évaluation.*/
	while (! queueEmpty(postfix)) { /*Continue si la file postfix n'est pas vide. Et a chaque itération de la boucle, la fonction retire le premier élément de la file et le supprime.*/
		Token * t = queueTop(postfix);
		queuePop(postfix);
		Token * arg1 = NULL;
		Token *arg2 = NULL;
		Token * res = NULL;
		if(tokenIsOperator(t)){ /*Si l'élément retiré est un opérateur, la fonction calcule le résultat de l'opération en utilisant les deux opérandes précédemment stockés dans la pile stackEvaluation. */
			arg2 = stackTop(stackEvaluation); /*Récupère les deux opérandes qui ont été précédemment ajoutés sur la pile stackEvaluation. */
			stackPop(stackEvaluation); /*Puis les supprime de la pile. */
			arg1 = stackTop(stackEvaluation);
			stackPop(stackEvaluation);
			res = evaluateOperator(arg1, t, arg2); /*Le résultat de l'opération est stocké dans un nouveau Token appelé res.*/
			deleteToken(&arg1); /*Les opérandes et l'opérateur sont supprimés.*/
			deleteToken(&arg2);
			deleteToken(&t);
			stackPush(stackEvaluation, res); /*Ensuite, le nouveau Token res est stocké dans la pile stackEvaluation. */
		}else if(tokenIsNumber(t)){ /*Si l'élément retiré est un nombre, la fonction stocke simplement le Token contenant le nombre dans la pile stackEvaluation. */
				stackPush(stackEvaluation,t);
		}
	} 
	float res_float = tokenGetValue(stackTop(stackEvaluation)); /*Après avoir parcouru tous les éléments de la file postfix, la fonction récupère le résultat final. */
	Token * last_token_to_delete = stackTop(stackEvaluation);
	deleteToken(&last_token_to_delete ); /*Le Token en haut de la pile stackEvaluation est ensuite supprimé. */
	stackPop(stackEvaluation); 
	deleteStack(&stackEvaluation); /*et la pile stackEvaluation est supprimée aussi.*/
	return res_float; /*La fonction retourne le résultat final res_float.*/
}

void computeExpressions(FILE *stream){
	char *buffer = NULL;
	size_t size = 0;
	while(getline(&buffer, &size, stream) != -1){ /*On parcourt le fichier ligne par ligne jusqu'à ce qu'il n'y ait plus de lignes à lire et on lit une ligne du fichier et stocke le résultat dans buffer. La fonction renvoie -1 si la fin du fichier est atteinte.*/
		printf("\nInput    : %s",buffer);
		printf("Infix    : ");
		Queue *infix_queue = stringToTokenQueue(buffer); /*On convertit la chaîne de caractères en une queue d'objets Token.*/
		queueDump(stdout,infix_queue,printToken); /* Puis on affiche le contenu de la queue. */
		printf("\nPostfix  : ");
		Queue *postfix_queue = shuntingYard(infix_queue); /*On prend la queue en notation infixée et la convertit en notation postfixée en utilisant la fonction Shunting Yard.*/
		queueDump(stdout,postfix_queue,printToken); /* Puis on affiche le contenu de la queue postfixée.*/
		printf("\nEvaluate : ");
		float evaluate = evaluateExpression(postfix_queue); /*On prend la queue postfixée et évalue l'expression en utilisant une pile pour stocker les opérandes et les opérateurs.*/
		printf("%f\n",evaluate); /* On affiche le résultat de l'évaluation.*/
		deleteQueue(&postfix_queue); /*Les deux queues d'objets Token sont supprimées à la fin de chaque itération de la boucle pour éviter des fuites de mémoire.*/
		deleteQueue(&infix_queue);
	}
	free(buffer);
}

/** Main function for testing.
 * The main function expects one parameter that is the file where expressions to translate are
 * to be read.
 *
 * This file must contain a valid expression on each line
 *
 */

int main(int argc, char **argv){
	FILE *input;

	if (argc<2) {
		fprintf(stderr,"usage : %s filename\n", argv[0]);
		return 1;
	}

	input = fopen(argv[1], "r");

	if ( !input ) {
		perror(argv[1]);
		return 1;
	}

	computeExpressions(input);

	fclose(input);
	return 0;
}
