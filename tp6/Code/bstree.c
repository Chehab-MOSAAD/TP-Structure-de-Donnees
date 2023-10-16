#include "bstree.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

void bstree_remove_node(ptrBinarySearchTree *t, ptrBinarySearchTree current);

/*------------------------  BSTreeType  -----------------------------*/

typedef enum {red, black} NodeColor;

struct _bstree {
    BinarySearchTree *parent;
    BinarySearchTree *left;
    BinarySearchTree *right;
    int root;
    NodeColor color;
};

struct _bstree* root = NULL;

/*------------------------  BaseBSTree  -----------------------------*/

BinarySearchTree *bstree_create() {
    return NULL;
}

/* This constructor is private so that we can maintain the oredring invariant on
 * nodes. The only way to add nodes to the tree is with the bstree_add function
 * that ensures the invariant.
 */
//ne pourra être utilisé que pour construire les feuilles.
BinarySearchTree *bstree_cons(BinarySearchTree *left, BinarySearchTree *right, int root) {
    BinarySearchTree *tree = malloc(sizeof(struct _bstree));
    tree->parent = NULL;
    tree->left = left;
    tree->right = right;
    if (tree->left != NULL){
        tree->left->parent = tree;
    }
    if (tree->right != NULL){
        tree->right->parent = tree;
    }
    tree->root = root;
    return tree;
}

ptrBinarySearchTree rbtree_cons(BinarySearchTree *left, BinarySearchTree *right, int root, ptrBinarySearchTree parent) {
    ptrBinarySearchTree tree = malloc(sizeof(struct _bstree));
    tree->parent = parent;
    tree->left = left;
    tree->right = right;
    tree->color = red; 
    
    if (tree->left != NULL){
        tree->left->parent = tree;
    }
    if (tree->right != NULL){
        tree->right->parent = tree;
    }
    
    tree->root = root;
    return tree;
}

NodeColor bstree_color(const BinarySearchTree *t) {
    if (t == NULL) {
        return black;  // Assuming NULL nodes are black
    } else {
        return t->color;
    }
}

void bstree_delete(ptrBinarySearchTree *t) {
    while (!bstree_empty(*t)){
        bstree_remove_node(t, *t);
    }
}

bool bstree_empty(const BinarySearchTree *t) {
    return t == NULL;
}

int bstree_root(const BinarySearchTree *t) {
    assert(!bstree_empty(t));
    return t->root;
}

BinarySearchTree *bstree_left(const BinarySearchTree *t) {
    assert(!bstree_empty(t));
    return t->left;
}

BinarySearchTree *bstree_right(const BinarySearchTree *t) {
    assert(!bstree_empty(t));
    return t->right;
}

BinarySearchTree *bstree_parent(const BinarySearchTree *t) {
    assert(!bstree_empty(t));
    return t->parent;
}

/*------------------------  BSTreeDictionary  -----------------------------*/

/* Obligation de passer l'arbre par référence pour pouvoir le modifier */
// void bstree_add(ptrBinarySearchTree *t, int v) {
//     ptrBinarySearchTree *current = t;
//     ptrBinarySearchTree par = NULL;
//     while (*current) {
//         par = *current;
//         if ((*current)->root == v){
//            exit(0);
//         }
//         current = ( ((*current)->root > v) ? &((*current)->left) : &((*current)->right) );
//     }
//     *current = bstree_cons(NULL, NULL, v);
//     (*current)->parent = par;
//     }

bool bstree_search(const BinarySearchTree *t, int v) {
    while (!bstree_empty(t)) {
        if (v < bstree_root(t)){
          t = bstree_left(t);
        }else if (v > bstree_root(t)){
          t = bstree_right(t);
        }else{
          break;
        }
    }
    return !bstree_empty(t);
    
}

BinarySearchTree *bstree_successor(const BinarySearchTree *x) {
    ptrBinarySearchTree s = x->right;
    if (s) {
        while (s->left){
            s = s->left;
        }
    } else {
        s = x->parent;
        while (s && (x == s->right)) {
            x = s;
            s = s->parent;
       }
    }
    return (s);
}
     
BinarySearchTree *bstree_predecessor(const BinarySearchTree *x) {
    ptrBinarySearchTree p = x->left;
    if (p) {
        while (p->right){
          p = p->right;
        }
    } else {
        p = x->parent;
        while (p && (x == p->left)) {
            x = p;
            p = p->parent;
       }
    }
    return (p);
}
     
void bstree_swap_nodes(ptrBinarySearchTree *tree, ptrBinarySearchTree from, ptrBinarySearchTree to) {
    assert(!bstree_empty(*tree) && !bstree_empty(from) && !bstree_empty(to));
    ptrBinarySearchTree *aux=tree;
    (*aux)->root=from->root;
    if( from->parent){
            (*aux)->parent=(from)->parent;
    }else{
            (*aux)->parent=NULL;
            ((*tree))->root=(to)->root;
    }
    if((from)->left){
            (*aux)->left=(from)->left;
    }else{
            (*aux)->left=NULL;
    }
    if((from)->right){
            (*aux)->right=(from)->right;
    }else{
            (*aux)->right=NULL;
        (from)->root=(to)->root;
    }
    if((to)->parent){
            (from)->parent=(to)->parent;
    }else{
            (from)->parent=NULL;
            ((*tree))->root=(*aux)->root;
    }
    if((to)->right){
        (from)->right=(to)->right;
    } else{
            (from)->right=NULL;
    }
    if((to)->left){
        (from)->left=(to)->left;
    }else{
            (from)->left=NULL;
    }
    (to)->parent=(*aux)->parent;
    (to)->left=(*aux)->left;
    (to)->right=(*aux)->right;
    (to)->root=(*aux)->root;
     
}

// t -> the tree to remove from, current -> the node to remove
// void bstree_remove_node(ptrBinarySearchTree *t, ptrBinarySearchTree current) {
//     assert(!bstree_empty(*t) && !bstree_empty(current));
//     BinarySearchTree **temp;
//     BinarySearchTree *leaf;
//         if(!(current)->parent){
//             temp=t;
//         }else if((current)->parent->left==(current)){
//             temp=&(current->parent->left);
//         }else{
//             temp=&((current)->parent->right);
//         }
//         if((current)->left==(current)->right){
//             *temp=bstree_create();
//             free(current);
//         }else if(!(current)->left){
//             *temp=(current)->right;
//             (current)->right->parent=(current)->parent;
//             free(current);
//         }else if (!(current)->right){
//             *temp=(current)->left;
//             (current)->left->parent=(current)->parent;
//             free(current);
//         }else {
//             leaf=bstree_successor(current);
//             if(leaf){
//                 (current)->root=leaf->root;
//                 bstree_remove_node(t, leaf);
//             }
//         }
// }

void bstree_remove(ptrBinarySearchTree *t, int v) {
        BinarySearchTree *current = *t;
        while(!bstree_empty(current)) {
            if(bstree_root(current)==v){
                break;
            }
            current=(bstree_root(current)>v?bstree_left(current):bstree_right(current));
        }
        if(!bstree_empty(current)) {
            bstree_remove_node(t,current);
        }
}
      
/*------------------------  BSTreeVisitors  -----------------------------*/

void bstree_depth_prefix(const BinarySearchTree *t, OperateFunctor f, void *userData) {
    if ( bstree_empty(t) ) return;
    f(t,userData);
    bstree_depth_prefix(t->left,f,userData);
    bstree_depth_prefix(t->right,f,userData);
}

void bstree_depth_infix(const BinarySearchTree *t, OperateFunctor f, void *userData) {
    if ( bstree_empty(t) ) return;
    bstree_depth_infix(t->left,f, userData);
    f(t,userData);
    bstree_depth_infix(t->right,f,userData);
    }
     
void bstree_depth_postfix(const BinarySearchTree *t, OperateFunctor f, void *userData) {
    if ( bstree_empty(t) ) return;
    bstree_depth_postfix(t->left,f, userData);
    bstree_depth_postfix(t->right,f, userData);
    f(t,userData);
}

void bstree_iterative_depth_infix(const BinarySearchTree *t, OperateFunctor f, void *userData) {
    BinarySearchTree *current=(BinarySearchTree *)t;
    while(!bstree_empty(bstree_left(current))){
            current=bstree_left(current);
    }
    while(!bstree_empty(current)) {
            f(current,userData);
            current=bstree_successor(current);
    }
}

void bstree_iterative_breadth_prefix(const BinarySearchTree *t, OperateFunctor f, void *userData) {
    Queue *q = createQueue();
    queuePush(q,t);
    while (!queueEmpty(q)){
            t=queueTop(q);
            q=queuePop(q);
            f(t,userData);
        if (!bstree_empty(bstree_left(t))){
                queuePush(q,bstree_left(t));
        }
        if( !bstree_empty(bstree_right(t))){
                queuePush(q,bstree_right(t));
        }
    }
}

/*------------------------  BSTreeIterator  -----------------------------*/

/**
 typedef enum {
     forward, backward
 } IteratorDirection;
 */

struct _BSTreeIterator {
    /* the collection the iterator is attached to */
    const BinarySearchTree *collection;
    /* the first element according to the iterator direction */
    const BinarySearchTree *(*begin)(const BinarySearchTree *);
    /* the current element pointed by the iterator */
    const BinarySearchTree *current;
    /* function that goes to the next element according to the iterator direction */
    BinarySearchTree *(*next)(const BinarySearchTree *);
};

/* minimum element of the collection */
const BinarySearchTree *goto_min(const BinarySearchTree *e) {
        while(bstree_left(e)){
            e=bstree_left(e);
        }
        return e;
}

/* maximum element of the collection */
const BinarySearchTree *goto_max(const BinarySearchTree *e) {
        while(bstree_right(e)){
            e=bstree_right(e);
        }
        return e;
}

/* constructor */
BSTreeIterator *bstree_iterator_create(const BinarySearchTree *collection, IteratorDirection direction) {
        BSTreeIterator *it = malloc(sizeof(struct _BSTreeIterator));
        if (it == NULL) {
            fprintf(stderr,"Error, Allocation.\n");
            exit(0);
        }
        it->collection = collection;
        if (direction == forward) {
            it->begin = goto_min;
            it->next = bstree_successor;
            it->current = it->begin(collection);
        }
        else {
            it->begin= goto_max;
            it->next = bstree_predecessor;
            it->current = it->begin(collection);
        }
        return it;
    }
 

/* destructor */
void bstree_iterator_delete(ptrBSTreeIterator *i) {
    free(*i);
    *i = NULL;
}

BSTreeIterator *bstree_iterator_begin(BSTreeIterator *i) {
    i->current = i->begin(i->collection);
    return i;
}

bool bstree_iterator_end(const BSTreeIterator *i) {
    return i->current == NULL;
}

BSTreeIterator *bstree_iterator_next(BSTreeIterator *i) {
    i->current = i->next(i->current);
    return i;
}

const BinarySearchTree *bstree_iterator_value(const BSTreeIterator *i) {
    return i->current;
}

/*------------------------  RBTSpecific  -----------------------------*/
void node_to_dot(const BinarySearchTree *t, void *userData) {
    FILE *file = (FILE *)userData;
    NodeColor color = bstree_color(t);  // Get the color of the node

    printf("%d ", bstree_root(t));
    fprintf(file, "\tn%d [label=\"{{<parent>}|%d|{<left>|<right>}}\", style=filled, fillcolor=%s];\n", bstree_root(t), bstree_root(t), (color == red || (bstree_parent(t) == NULL && bstree_left(t) == NULL && bstree_right(t) == NULL)) ? "red" : "gray");

    if (bstree_left(t)) {
        fprintf(file, "\tn%d:left:c -> n%d:parent:c [headclip=false, tailclip=false]\n", bstree_root(t), bstree_root(bstree_left(t)));
    } else {
        fprintf(file, "\tlnil%d [style=filled, fillcolor=gray, label=\"NIL\"];\n", bstree_root(t));
        fprintf(file, "\tn%d:left:c -> lnil%d:n [headclip=false, tailclip=false]\n", bstree_root(t), bstree_root(t));
    }

    if (bstree_right(t)) {
        fprintf(file, "\tn%d:right:c -> n%d:parent:c [headclip=false, tailclip=false]\n", bstree_root(t), bstree_root(bstree_right(t)));
    } else {
        fprintf(file, "\trnil%d [style=filled, fillcolor=gray, label=\"NIL\"];\n", bstree_root(t));
        fprintf(file, "\tn%d:right:c -> rnil%d:n [headclip=false, tailclip=false]\n", bstree_root(t), bstree_root(t));
    }
}

void printNode(const BinarySearchTree *n, void *userData) {
    FILE *file = (FILE *)userData;
    NodeColor color = bstree_color(n);  // Get the color of the node

    fprintf(file, "\tn%d [label=\"{{<parent>}|%d|{<left>|<right>}}\", style=filled, fillcolor=%s];\n", bstree_root(n), bstree_root(n), (color == red || (bstree_parent(n) == NULL && bstree_left(n) == NULL && bstree_right(n) == NULL)) ? "red" : "gray");

    if (bstree_left(n)) {
        fprintf(file, "\tn%d:left:c -> n%d:parent:c [headclip=false, tailclip=false]\n", bstree_root(n), bstree_root(bstree_left(n)));
    } else {
        fprintf(file, "\tlnil%d [style=filled, fillcolor=gray, label=\"NIL\"];\n", bstree_root(n));
        fprintf(file, "\tn%d:left:c -> lnil%d:n [headclip=false, tailclip=false]\n", bstree_root(n), bstree_root(n));
    }

    if (bstree_right(n)) {
        fprintf(file, "\tn%d:right:c -> n%d:parent:c [headclip=false, tailclip=false]\n", bstree_root(n), bstree_root(bstree_right(n)));
    } else {
        fprintf(file, "\trnil%d [style=filled, fillcolor=gray, label=\"NIL\"];\n", bstree_root(n));
        fprintf(file, "\tn%d:right:c -> rnil%d:n [headclip=false, tailclip=false]\n", bstree_root(n), bstree_root(n));
    }
}

void rbtree_export_dot(const BinarySearchTree *t, FILE *file) {
    fprintf(file, "digraph RedBlackTree {\n  graph[ranksep=0.5];\n  node[shape=record];\n\n");
    bstree_iterative_depth_infix(t, printNode, file);
    fprintf(file, "\n}\n");
}

void rightrotate(BinarySearchTree *t) {
    BinarySearchTree* l = t->left;
    t->left = l->right;
    if (t->left)
        t->left->parent = t;
    l->parent = t->parent;
    if (!t->parent)
        root = l;
    else if (t == t->parent->left)
        t->parent->left = l;
    else
        t->parent->right = l;
    l->right = t;
    t->parent = l;
}

void leftrotate(BinarySearchTree *t) {
    BinarySearchTree* r = t->right;
    t->right = r->left;
    if (t->right)
        t->right->parent = t;
    r->parent = t->parent;
    if (!t->parent)
        root = r;
    else if (t == t->parent->left)
        t->parent->left = r;
    else
        t->parent->right = r;
    r->left = t;
    t->parent = r;
}

void testrotateright(BinarySearchTree *t) {
    rightrotate (t);
}

void testrotateleft(BinarySearchTree *t) {
    leftrotate (t);
}

ptrBinarySearchTree grandparent(ptrBinarySearchTree n){
    if ((n != NULL) && (n->parent != NULL))
        return n->parent->parent;
    else
        return NULL;
}

ptrBinarySearchTree uncle(ptrBinarySearchTree n){
    ptrBinarySearchTree g = grandparent(n);
    if (g == NULL)
        return NULL; // No grandparent means no uncle
    if (n->parent == g->left)
        return g->right;
    else
        return g->left;
}

ptrBinarySearchTree fixredblack_insert_case2_left(ptrBinarySearchTree x){
    ptrBinarySearchTree p = x->parent;
    ptrBinarySearchTree pp = grandparent(x);
    if (x == p->left) {
        rightrotate(pp);
        p->color = black;
        pp->color=red;
        return p;
    } else {
        leftrotate(p);
        rightrotate(pp);
        x->color = black;
        pp->color = red;
        return x;
    }
}

ptrBinarySearchTree fixredblack_insert_case2_right(ptrBinarySearchTree x) {
    ptrBinarySearchTree p = x->parent;
    ptrBinarySearchTree pp = grandparent(x);
    if (x == p->right) {
        leftrotate(pp);
        p->color = black;
        pp->color = red;
        return p;
    } else {
        rightrotate(p);
        leftrotate(pp);
        x->color = black;
        pp->color = red;
        return x;
    }
}

ptrBinarySearchTree fixredblack_insert_case2(ptrBinarySearchTree x){
    ptrBinarySearchTree p = x->parent;
    ptrBinarySearchTree pp = grandparent(x);
    if (pp->left == p)
        x = fixredblack_insert_case2_left(x);
    else
        x = fixredblack_insert_case2_right(x);
    return x;
}

ptrBinarySearchTree fixredblack_insert(ptrBinarySearchTree x);

ptrBinarySearchTree fixredblack_insert_case1(ptrBinarySearchTree x){
    ptrBinarySearchTree f = uncle(x);
    if ((f != NULL) && (f->color == red)) {
        ptrBinarySearchTree pp = grandparent(x);
        x->parent->color = black;
        f->color = black;
        pp->color = red;
        return fixredblack_insert(pp);
    } else {
        return fixredblack_insert_case2(x);
    }
}

ptrBinarySearchTree fixredblack_insert_case0(ptrBinarySearchTree x){
    ptrBinarySearchTree p = x->parent;
    if ( p->parent == NULL)
        p->color = black;
    else
        x = fixredblack_insert_case1(x);
    return x;
}

ptrBinarySearchTree fixredblack_insert(ptrBinarySearchTree x){
    if (x->parent == NULL)
        x->color=black;
    else if (x->parent->color == red)
        x = fixredblack_insert_case0(x);
    return x;
}

void bstree_add (ptrBinarySearchTree *t, int v) {
    ptrBinarySearchTree *current = t;
    ptrBinarySearchTree parent = NULL;
    while (*current != NULL) {
        if ((*current)->root == v)
            return;
        parent = *current;
        current = ( ((*current)->root > v) ? &((*current)->left) : &((*current)->right) );
    }

    *current = rbtree_cons(NULL, NULL, v, parent);
    /* fix redblack properties on current */
    BinarySearchTree * stop = fixredblack_insert(*current);
    /* set the new tree root if needed */
    if (stop -> parent == NULL )
        *t = stop ;
}

ptrBinarySearchTree fixredblack_remove(ptrBinarySearchTree p, ptrBinarySearchTree x);

ptrBinarySearchTree fixredblack_remove_case1_left(ptrBinarySearchTree p, ptrBinarySearchTree x);

ptrBinarySearchTree fixredblack_remove_case1_right(ptrBinarySearchTree p, ptrBinarySearchTree x);

ptrBinarySearchTree fixredblack_remove_case1(ptrBinarySearchTree p, ptrBinarySearchTree x);

ptrBinarySearchTree fixredblack_remove_case2(ptrBinarySearchTree p, ptrBinarySearchTree x);


void bstree_remove_node (ptrBinarySearchTree *t, ptrBinarySearchTree current){
    ptrBinarySearchTree substitute;
    /* Get the real node to remove and its substitute */
    if (current->left == current->right) {
        substitute = NULL;
    } else if (! current->left) {
        /* current has only its right child */
        substitute = current->right;
    } else if (! current->right) {
        /* current has only its left child */
        substitute = current->left;
    } else {
        /* current has both childs */
        ptrBinarySearchTree leaf;
        leaf = bstree_successor(current);
        /* swap current and leaf */
        current->root = leaf->root;
        current = leaf;
        substitute = current->right;
    }
    /* update the tree */
    if (substitute != NULL)
        substitute->parent = current->parent;
    if ( ! current->parent )
        *t = substitute;
    else if (current->parent->left == current)
        current->parent->left = substitute;
    else
        current->parent->right = substitute;
    /* fix the colors if needed */
    if (current->color == black) {
        /* fix redblack properties */
        if ( (substitute == NULL) || (substitute->color == black) ) {
        /* substitute is double black : must fix */
        ptrBinarySearchTree subtreeroot = fixredblack_remove(current->parent, substitute);
            if (subtreeroot->parent == NULL)
                *t = subtreeroot;
        } else {
            /* substitute become black */
            substitute->color = black;
        }
    }
    /* delete the removed node */
    free(current);
}

ptrBinarySearchTree fixredblack_remove(ptrBinarySearchTree p, ptrBinarySearchTree x){
    /* node x is the double black node with parent p */
    if (p == NULL) {
        /* the root x is black, double black evaporates */
        return x;
    } else {
        /* take a decision according to the brother’s color */
        ptrBinarySearchTree b = (p->left == x) ? (p->right) : (p->left);

    if ( b->color==black )
        return fixredblack_remove_case1(p, x);
    else
        return fixredblack_remove_case2(p, x);
    }
}

ptrBinarySearchTree fixredblack_remove_case1_left(ptrBinarySearchTree p, ptrBinarySearchTree x){
    ptrBinarySearchTree b = p->right; /* b is black */
    if (((b->left == NULL) || (b->left->color == black)) && ((b->right == NULL) || (b->right->color == black))) {
        /* case 1.a : both sons are black or null leaf */
        b->color = red;
        if ( p->color == red) {
            p->color = black;
            return p;
        } else {
            return fixredblack_remove(p->parent, p);
        }
    } else if ((b->right != NULL) && (b->right->color == red)) {
        /* case 1.b : right son is red, left is red or black */
        leftrotate(p);
        b->color = p->color;
        p->color = black;
        b->right->color = black;
        return b;
    } else {
        /* case 1.c : right son is black and left son is red */
        b->left->color = black;
        b->color = red;
        rightrotate(b);
        leftrotate(p);
        b->color = p->color;
        x = x;
        return b->parent;
    }
}

ptrBinarySearchTree fixredblack_remove_case1_right(ptrBinarySearchTree p, ptrBinarySearchTree x) {
    ptrBinarySearchTree b = p->left; /* b is black */
    if (((b->right == NULL) || (b->right->color == black)) && ((b->left == NULL) || (b->left->color == black))) {
        /* case 1.a : both sons are black or null leaf */
        b->color = red;
        if (p->color == red) {
            p->color = black;
            return p;
        } else {
            return fixredblack_remove(p->parent, p);
        }
    } else if ((b->left != NULL) && (b->left->color == red)) {
        /* case 1.b : left son is red, right is red or black */
        rightrotate(p);
        b->color = p->color;
        p->color = black;
        b->left->color = black;
        return b;
    } else {
        /* case 1.c : left son is black and right son is red */
        b->right->color = black;
        b->color = red;
        leftrotate(b);
        rightrotate(p);
        b->color = p->color;
        x = x;
        return b->parent;
    }
}

ptrBinarySearchTree fixredblack_remove_case1(ptrBinarySearchTree p, ptrBinarySearchTree x){
    /* x is not the root of the tree */
    if (x == p->left)
        return fixredblack_remove_case1_left(p, x);
    else
        return fixredblack_remove_case1_right(p, x);
}

ptrBinarySearchTree fixredblack_remove_case2_left(ptrBinarySearchTree p, ptrBinarySearchTree x){
    ptrBinarySearchTree f = p->right; /* f is red */
    leftrotate(p);
    p->color = red;
    f->color = black;
    fixredblack_remove_case1_left(p, x);
    return f;
}

ptrBinarySearchTree fixredblack_remove_case2_right(ptrBinarySearchTree p, ptrBinarySearchTree x) {
    ptrBinarySearchTree f = p->left; /* f is red */
    rightrotate(p);
    p->color = red;
    f->color = black;
    fixredblack_remove_case1_right(p, x);
    return f;
}

ptrBinarySearchTree fixredblack_remove_case2(ptrBinarySearchTree p, ptrBinarySearchTree x){
    /* x is not the root of the tree */
    if (x == p->left)
        return fixredblack_remove_case2_left(p, x);
    else
        return fixredblack_remove_case2_right(p, x);
}
