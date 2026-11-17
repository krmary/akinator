#include <stdio.h>
#include "assert_my.h"
#include <stdlib.h>
#include <process.h>
#include <stdint.h>
#include <string.h>

typedef char* elem_t;

#define MAX_STR_SIZE 32
#define TXT_FILE_READ "akinator1.txt"
#define PNG_FILE "png_akinator.png"
#define DOT_FILE "png_akinator.dot"

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
void TreeDump   (Node_t* first_node);

Node_t* CreateNode   (elem_t in_data, Node_t* p_parent, side_t side_elem);
void    DestroyNode  (Node_t* node);
void    LinkRight    (Node_t* node, Node_t* p_parent);
void    LinkLeft     (Node_t* node, Node_t* p_parent);
Node_t* ReadTree     (const char* file_name);
Node_t* ReadNode     (FILE* fp, Node_t* p_parent);
side_t  ChooseSide   (Node_t* p_parent);
void    PrintTree    (const char* file_name, Node_t* head_node);
void    PrintNodePre (FILE* fp, Node_t* node);

void GuessPerson (Node_t* head_note);

int main(void) {
    Node_t* node1 = ReadTree(TXT_FILE_READ);
    TreeDump(node1);

    GuessPerson(node1);

    DestroyNode(node1);
    return 0;
}

void DumpNode (FILE* fp, Node_t* node, int number) {
    assert_my(fp, );
    assert_my(node, );

    fprintf(fp, "{rank = %d; \"%p\" [shape=Mrecord, style=\"filled\",fillcolor=\"#%x\", color=\"#0035d6\", label= \"{%s}\"]};\n",\
            number, node, (uintptr_t)node->parent*23456781 + 12345678, node->data);
}

void DumpPngRec (FILE* fp, Node_t* head_node, int i) {
    assert_my(head_node, );
    assert_my(fp, );

    DumpNode(fp, head_node, i);

    if (head_node->left) {
        fprintf(fp, "\"%p\"->\"%p\"[label = \"DA\"];\n", (void*)head_node, (void*)head_node->left);
        DumpPngRec(fp, head_node->left, i + 1);
    }

    if (head_node->right) {
        fprintf(fp, "\"%p\"->\"%p\"[label = \"NET\"];\n", (void*)head_node, (void*)head_node->right);
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

void TreeDump (Node_t* first_node) {
    PngFile(first_node);

    char system_str[50] = {0};
    snprintf(system_str, 50*sizeof(char), "%s%d%s", "dot -Tpng " DOT_FILE " -o ", num_picture, PNG_FILE);
    num_picture += 1;

    int result = system(system_str);
    if (result != 0) printf("Error: Graphviz can't create png file.\n");
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

    if (node->parent) {
        if (node->parent->right == node) {
            node->parent->right = 0;

        } else if (node->parent->left == node){
            node->parent->left = 0;
        }
    }
    if (node->left)  DestroyNode(node->left);
    if (node->right) DestroyNode(node->right);

    free(node->data);
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

    elem_t data = (elem_t)calloc(MAX_STR_SIZE, MAX_STR_SIZE*sizeof(char));
    if (fscanf(fp, "%[^()]", data) == 0) return NULL;

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

    PrintNodePre(fp, head_node);

    fclose(fp);
    return;
}

void PrintNodePre (FILE* fp, Node_t* node) {
    assert_my(fp, );
    assert_my(node, );

    fprintf(fp, "( ");
    fprintf(fp, "%s", node->data);

    if (node->left) PrintNodePre(fp, node->left);
    if (node->right) PrintNodePre(fp, node->right);

    fprintf(fp, ")");
    return;
}

void GuessPerson (Node_t* head_note) {
    assert_my(head_note, );

    char answer[5] = {0};

    while (head_note->right && head_note->left) {
        printf("%s", head_note->data);
        scanf("%s", answer);

        if (!strcmp(answer, "da")) head_note = head_note->left;
        else if (!strcmp(answer, "net")) head_note = head_note->right;
    }

    printf("%s", head_note->data);
    return;
}
