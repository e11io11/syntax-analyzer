#include "../inc/builtin_func.h"


builtinFunctionSymbolTables blt_functions;


/**
 * @brief List of all blt functions 
 * 
 */
blt_f_decl __blt_fct[] = {
    /**
     * @brief Get an int from stdin and returns it
     * 
     */
    {.name = "getint",
    .parameters = "void",
    .parametersAmount = 0,
    .return_value = "int",
    .asm_src_code = "\0"}, 
    /**
     * @brief Get a char from stdin and returns it
     * 
     */
    {.name = "getchar",
    .parameters = "void",
    .parametersAmount = 0,
    .return_value = "char",
    .asm_src_code = "\0"},
    /**
     * @brief Put an int into stdout
     * 
     */
    {.name = "putint",
    .parameters = "int",
    .parametersAmount = 1,
    .return_value = "void",
    .asm_src_code = "\0"},
    /**
     * @brief Put a char into stdout
     * 
     */
    {.name = "putchar",
    .parameters = "char",
    .parametersAmount = 1,
    .return_value = "void",
    .asm_src_code = "putchar: ;rdi char\n\
    push rbp\n\
    mov rbp, rsp\n\
    push rdi\n\
    mov rax, 1\n\
    mov rdi, 1\n\
    mov rsi, rsp\n\
    mov rdx, 1\n\
    syscall\n\
    mov rsp, rbp\n\
    pop rbp\n\
    ret\n"},


    /* You can add builtin here :
    format : 
    {.name = "" [max 256],
    .parameters = "type1,..." [max 1024],
    .parametersAmount = 0,
    .return_value = "type" [max 5],
    .asm_src_code = "code" [max 8096]},
    */


    /* END TOKEN DO NOT MODIFY */
    {.name = "",
    .parameters = "",
    .parametersAmount = 0,
    .return_value = "",
    .asm_src_code = "\0"}
};

value init_func_symboltab(blt_f_decl bf, int index) {
    value result;
    char * p;
    result.type = _type_function;
    result.pileOffset = index;
    result.val.func.ret = charToType(bf.return_value);
    p = strtok(bf.parameters, ",");
    if (p)
        result.val.func.param = newFcParam(charToType(p));
    for (int i = 0; i < bf.parametersAmount - 1; i++) {
        _type_fc_param * temp = result.val.func.param;
        p = strtok(NULL, ",");
        if (p == NULL) break;
        result.val.func.param = newFcParam(charToType(p));
        result.val.func.param->next = temp;
    }
    result.val.func.param_ct = bf.parametersAmount;
    return result;
}

int count_blt_func() {
    int i = 0;
    while (__blt_fct[i].name[0]) i++;
    return i;
}

void blt_func_init() {
    int i;
    blt_functions.amount = count_blt_func();
    blt_functions.fcts = newSymbolTab();
    for (i = 0; i < blt_functions.amount; i++) {
        putHashVal_checked(&(blt_functions.fcts),
            newHashElem(__blt_fct[i].name, 
                        init_func_symboltab(__blt_fct[i], i), 0));
    }

}

void displayBuiltin() {
    printf("Loaded Builtin functions:\n");
    displayHashTable(blt_functions.fcts);
}

void displayBuiltinDecl() {
    int c = count_blt_func();
    printf(
        COLOR_CYAN STYLE_BOLD "List of available builtin : [%d]\n" COLOR_RESET \
        COLOR_YELLOW "[HINT] They can be defined at ./src/builtin_func.c\n\n" COLOR_RESET,
        c
    );
    for (int i = 0; i < c; i++) {
        printf(" - %s : %s -> %s\n", __blt_fct[i].name, __blt_fct[i].parameters, __blt_fct[i].return_value);
    }
    printf("\n\n");
}

char * getBuiltinSrcCode(HashElem * elem) {
    if (AMOUNT_BLT_FUNCTIONS > elem->h_val.pileOffset) {
        return __blt_fct[elem->h_val.pileOffset].asm_src_code;
    }
    raiseError("Error while loading asm src code in builtin");
    return "";
}