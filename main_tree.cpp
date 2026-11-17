#include <stdio.h>
#include "assert_my.h"
#include <stdlib.h>
#include <process.h>
#include <stdint.h>

#define PNG_FILE "png_tree.png"
#define DOT_FILE "png_tree.dot"
#define TXT_FILE_READ "tree1.txt"
#define TXT_FILE_WRITE "tree2.txt"

typedef int elem_t;

//глобальная переменная
int num_picture = 0;

struct Node_t
{
    elem_t data;
    Node_t* left;
    Node_t* right;
    Node_t* parent;
};

enum side_t {
    LEFT  = 0,
    RIGHT = 1,
    ERROR = 2
};

void DumpNode   (FILE* fp, Node_t* node, int number);
void DumpPngRec (FILE* fp, Node_t* head_node, int i);
void PngFile    (Node_t* head_node);

Node_t* CreateNode  (elem_t in_data, Node_t* p_parent, side_t side_elem);
void    DestroyNode (Node_t* node);
void    LinkRight   (Node_t* node, Node_t* p_parent);
void    LinkLeft    (Node_t* node, Node_t* p_parent);
Node_t* AddToTree   (elem_t num, Node_t* head_tree);
void    TreeDump    (Node_t* first_node);

Node_t* ReadTree   (const char* file_name);
Node_t* ReadNode   (FILE* fp, Node_t* p_parent);
side_t  ChooseSide (Node_t* p_parent);

void PrintTree     (const char* file_name, Node_t* head_node);
void PrintNodePre  (FILE* fp, Node_t* node);
void PrintNodePost (FILE* fp, Node_t* node);
void PrintNodeIn   (FILE* fp, Node_t* node);

void TreeVerify (Node_t* head_node);

int main(void) {
    Node_t* node1 = AddToTree(50, 0);
    Node_t* node2 = AddToTree(60, node1);
    Node_t* node3 = AddToTree(20, node1);
    Node_t* node4 = AddToTree(30, node1);
    Node_t* node5 = AddToTree(52, node1);
    Node_t* node6 = AddToTree(10, node1);
    Node_t* node7 = AddToTree(67, node1);

    TreeDump(node1);

    Node_t* node8 = AddToTree(90, node1);
    Node_t* node9 = AddToTree(34, node1);
    Node_t* node10 = AddToTree(94, node1);

    TreeVerify(node1);

    TreeDump(node1);

    DestroyNode(node1);

    ReadTree(TXT_FILE_READ);

    TreeDump(node1);

    PrintTree(TXT_FILE_WRITE, node1);

    if (node1) DestroyNode(node1);
    return 0;
}

void DumpNode (FILE* fp, Node_t* node, int number) {
    assert_my(fp, );
    assert_my(node, );

    fprintf(fp, "{rank = %d; \"%p\" [shape=Mrecord, style=\"filled\",fillcolor=\"#%x\", color=\"#0035d6\", label= \"{node %p | parent = %p | DATA = %d| {left %p | right %p}}\"]};\n",\
            number, node, (uintptr_t)node->parent*23456781 + 12345678, (void*)node, (void*)node->parent, node->data, (void*)node->left, (void*)node->right);
}

void DumpPngRec (FILE* fp, Node_t* head_node, int i) {
    assert_my(head_node, );
    assert_my(fp, );

    DumpNode(fp, head_node, i);

    if (head_node->left) {
        fprintf(fp, "\"%p\"->\"%p\"\n", (void*)head_node, (void*)head_node->left);
        DumpPngRec(fp, head_node->left, i + 1);
    }

    if (head_node->right) {
        fprintf(fp, "\"%p\"->\"%p\"\n", (void*)head_node, (void*)head_node->right);
        DumpPngRec(fp, head_node->right, i + 1);
    }
    return;
}

void PngFile (Node_t* head_node) {
    FILE* fp = fopen(DOT_FILE, "w");
    assert_my(fp, );

    fprintf(fp, "digraph G{\n");
    fprintf(fp, "edge[color=\"#8e1732\"]\n");

    if (head_node) DumpPngRec(fp, head_node, 0);
    else fprintf(fp, "error [shape=Mrecord, style=\"filled\",fillcolor=\"#f8c1d0\", color=\"#f8dce1\", label= \"{ERROR}\"];\n");

    fprintf(fp, "}\n");

    fclose(fp);
    return;
}

Node_t* CreateNode (elem_t in_data, Node_t* p_parent, side_t side_elem) {
    Node_t* node = (Node_t*)calloc(1, sizeof(Node_t));
    assert_my(node, NULL);

    node->data = in_data;

    if (p_parent) {
        if (side_elem == RIGHT) LinkRight(node, p_parent);
        else if (side_elem == LEFT) LinkLeft(node, p_parent);
        else  printf("error: too many sides\n");
    }
    return node;
}

void DestroyNode (Node_t* node) {
    assert_my(node, );

    node->data = 0;
    if (node->parent) {
        if (node->parent->right == node) {
            node->parent->right = 0;

        } else if (node->parent->left == node){
            node->parent->left = 0;
        }
    }
    if (node->left)  DestroyNode(node->left);
    if (node->right) DestroyNode(node->right);

    free(node);
    return;
}

void LinkRight (Node_t* node, Node_t* p_parent) {
    assert_my(node, );
    assert_my(p_parent, );

    p_parent->right = node;
    node->parent = p_parent;
    return;
}

void LinkLeft (Node_t* node, Node_t* p_parent) {
    assert_my(node, );
    assert_my(p_parent, );

    p_parent->left = node;
    node->parent = p_parent;
    return;
}

Node_t* AddToTree (elem_t num, Node_t* head_node) {
    if (!head_node) return CreateNode(num, head_node, LEFT);

    if (num >= head_node->data) {
        if (head_node->right) return AddToTree(num, head_node->right);
        return CreateNode(num, head_node, RIGHT);

    } else {
        if (head_node->left) return AddToTree(num, head_node->left);
        return CreateNode(num, head_node, LEFT);
    }
}

void TreeDump (Node_t* first_node) {
    PngFile(first_node);

    char system_str[50] = {0};
    snprintf(system_str, 50*sizeof(char), "%s%d%s", "dot -Tpng " DOT_FILE " -o ", num_picture, PNG_FILE);
    num_picture += 1;

    int result = system(system_str);
    if (result != 0) printf("Error: Graphviz can't create png file.\n");
    return;
}

Node_t* ReadTree (const char* file_name) {
    assert_my(file_name, NULL);

    FILE* fp = fopen(file_name, "r");
    assert_my(fp, NULL);

    Node_t* head_node = 0;
    if (fgetc(fp) == '(') head_node = ReadNode(fp, NULL);
    else printf("error: no head node in txt file\n");

    fclose(fp);
    return head_node;
}

Node_t* ReadNode (FILE* fp, Node_t* p_parent) {
    assert_my(fp, NULL);

    elem_t data = 0;
    if (fscanf(fp, "%d", &data) != 1) return NULL;

    side_t side = ChooseSide(p_parent);
    Node_t* node = CreateNode(data, p_parent, side);

    char ch = fgetc(fp);
    if (ch == '(') {
        while ((ch = fgetc(fp)) != ')') {
            if (ch == EOF) return NULL;
            if (!ReadNode(fp, node)) return NULL;
        }
    }
    if (ch == ')') return node;

    printf("error: can't create node\n");
    return NULL;
}

side_t ChooseSide (Node_t* p_parent) {
    side_t side = LEFT;
    if (p_parent) {
        if (!p_parent->left) side = LEFT;
        else if (!p_parent->right) side = RIGHT;

        else side = ERROR;
    }
    return side;
}

void PrintTree (const char* file_name, Node_t* head_node) {
    assert_my(file_name, );

    FILE* fp = fopen(file_name, "w");
    assert_my(fp, );

    PrintNodeIn(fp, head_node);

    fclose(fp);
    return;
}

void PrintNodePre (FILE* fp, Node_t* node) {
    assert_my(fp, );
    assert_my(node, );

    fprintf(fp, "( ");
    fprintf(fp, "%d", node->data);

    if (node->left) PrintNodePre(fp, node->left);
    if (node->right) PrintNodePre(fp, node->right);

    fprintf(fp, ")");
    return;
}

void PrintNodeIn (FILE* fp, Node_t* node) {
    assert_my(fp, );
    assert_my(node, );

    fprintf(fp, "( ");

    if (node->left) PrintNodeIn(fp, node->left);
    fprintf(fp, "%d", node->data);
    if (node->right) PrintNodeIn(fp, node->right);

    fprintf(fp, ")");
    return;
}

void PrintNodePost (FILE* fp, Node_t* node) {
    assert_my(fp, );
    assert_my(node, );

    fprintf(fp, "( ");

    if (node->left) PrintNodePost(fp, node->left);
    if (node->right) PrintNodePost(fp, node->right);
    fprintf(fp, "%d", node->data);

    fprintf(fp, ")");
    return;
}

void TreeVerify (Node_t* head_node) {
    assert_my(head_node, );

    if (head_node->left && head_node != head_node->left->parent || \
        head_node->right && head_node != head_node->right->parent) {
        printf("Tree is not OK\nhead_node %p\nhead_node->left->parent %p\nhead_node->right->parent %p\n", \
        head_node, head_node->left->parent, head_node->right->parent);
        return;
    }
    printf("head_node %p is OK\n", head_node);

    if (head_node->right) TreeVerify(head_node->right);
    if (head_node->left) TreeVerify(head_node->left);
    return;
}
