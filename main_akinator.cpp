int main(int argc, char** argv) {
    AkinatorGame(argv[1]);
    return 0;
}

void DumpNode (FILE* fp, Node_t* node, int number) {
    assert_my(fp, );
    assert_my(node, );

    fprintf(fp, "{rank = %d; \"%p\" [shape=Mrecord, style=\"filled\",fillcolor=\"#%x\", color=\"#35178e\", label= \"{%s}\"]};\n",\
            number, node, (uintptr_t)node->parent*23456781 + 12345699, node->data);
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
    if (result != 0) printf(RED "Error: Graphviz can't create png file.\n" RESET);
    return;
}

Node_t* CreateNode (elem_t in_data, Node_t* p_parent, side_t side_elem) {
    Node_t* node = (Node_t*)calloc(1, sizeof(Node_t));
    assert_my(node, NULL);

    node->data = (elem_t)calloc(MAX_STR_SIZE, sizeof(char));
    assert_my(node->data, NULL);
    strcpy(node->data, in_data);

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
    else printf(RED "error: no head node in txt file\n" RESET);

    fclose(fp);
    return head_node;
}

Node_t* ReadNode (FILE* fp, Node_t* p_parent) {
    assert_my(fp, NULL);

    elem_t data = (elem_t)calloc(MAX_STR_SIZE, sizeof(char));
    assert_my(data, NULL);

    if (fscanf(fp, " %[^()]", data) == 0) return NULL;

    side_t side = ChooseSide(p_parent);
    Node_t* node = CreateNode(data, p_parent, side);

    int ch = fgetc(fp);
    if (ch == '(') {
        ungetc(ch, fp);
        while ((ch = fgetc(fp)) != ')') {
            if (ch == EOF) return NULL;
            if (!ReadNode(fp, node)) return NULL;
        }
    }
    free(data);

    if (ch == ')') return node;

    printf(RED "error: can't create node\n" RESET);
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

void GuessPerson (Node_t* head_node, mode__t mode) {
    assert_my(head_node, );

    char answer[5] = {0};

    while (head_node->right && head_node->left) {
        printf(YELLOW "%s\n" RESET, head_node->data);
        Scanf_str(4, answer);

        if (!strcmp(answer, "da")) head_node = head_node->left;
        else if (!strcmp(answer, "net")) head_node = head_node->right;
        else printf(RED "error\n" RESET);
    }

    if (mode == CH_AND_ADD) {
        printf(YELLOW "%s?(da/net)\n" RESET, head_node->data);
        scanf("%s", answer);

        if (!strcmp(answer, "da")) return;
        if (!strcmp(answer, "net")) AddPerson(head_node);
        else printf(RED "error\n" RESET);
        return;

    } else printf(YELLOW "%s\n" RESET, head_node->data);
}

void AddPerson (Node_t* node) {
    assert_my(node, );

    printf(YELLOW "A kto eto togda ?\n" RESET);
    char* new_person = (char*)calloc(MAX_STR_SIZE, sizeof(char));
    assert_my(new_person, );

    Scanf_str(63, new_person);
    printf(YELLOW "Chem %s otlichaetsya ot %s?\n" RESET, new_person, node->data);

    elem_t new_characteristic = (elem_t)calloc(MAX_STR_SIZE, sizeof(char));
    assert_my(new_characteristic, );

    Scanf_str(62, new_characteristic);
    CreateNode(new_person, node, LEFT);
    CreateNode(node->data, node, RIGHT);

    strcpy(node->data, strcat(new_characteristic, "?"));

    free(new_characteristic);
    free(new_person);
    return;
}

void ChooseWhatToDo (Node_t* node) {
    printf(CYAN "Cho vi hotite delat?\nugadivat:         \"choose\"\nadd person:       \"add\"\nposmotret derevo: \"dump\"\n" RESET);

    char answer[10] = {0};
    Scanf_str(9, answer);

    while (strcmp(answer, "stop")) {
        if (!strcmp(answer, "stop")) break;

        if (!strcmp(answer, "choose")) GuessPerson(node, CHOOSE);
        else if (!strcmp(answer, "add")) GuessPerson(node, CH_AND_ADD);
        else if (!strcmp(answer, "dump")) TreeDump(node);
        else printf(RED "error, try again\n" RESET);

        printf(CYAN "Chto dalshe?(\"stop\" dlya konca): " RESET);
        Scanf_str(9, answer);
    }
    printf(MAGENTA "Goodbye!\n" RESET);
}

void AkinatorGame (char* file_name) {
    assert_my(file_name, );

    Node_t* node1 = ReadTree(file_name);

    ChooseWhatToDo(node1);

    PrintTree(file_name, node1);
    DestroyNode(node1);
    return;
}
