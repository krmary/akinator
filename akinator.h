#include <stdio.h>
#include "../assert_my.h"
#include <stdlib.h>
#include <process.h>
#include <stdint.h>
#include <string.h>

typedef char* elem_t;

#define MAX_STR_SIZE 64
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

enum mode__t {
    ERROR_MODE = 0,
    CHOOSE     = 1,
    CH_AND_ADD = 2,
    DUMP       = 3
};

#define Scanf_str(dl, str_p) \
{\
    scanf(" %"#dl"[^\n]", str_p);\
    while (getchar() != '\n') ;\
}

#define RESET       "\033[0m"
#define RED         "\033[1;31m"
#define YELLOW      "\033[1;33m"
#define MAGENTA     "\033[1;35m"
#define CYAN        "\033[1;36m"

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

void GuessPerson    (Node_t* head_node, mode__t mode);
void AddPerson      (Node_t* node);
void ChooseWhatToDo (Node_t* node);

void AkinatorGame (char* file_name);
