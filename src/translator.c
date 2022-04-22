/**
 * @author Antonin JEAN
 * @email ofghanirre@gmail.com
 * @create date 2022-04-05 16:48:52
 * @modify date 2022-04-05 16:48:52
 * @desc Src file related to asm translation
 */

#include "../inc/translator.h"

#define SIZE_INT "dd"
#define SIZE_CHAR "db"

#define __ASM_PATTERN_HEADER \
    "; FILE AUTO GENERATED BY COMPILER\n"                   \
    "; --------------------\n"                              \
    "; Translator tpc -> asm\n"                             \
    ";\n"                                                   \
    "; authors : Elliott FALGUEROLLE & Antonin JEAN\n\n"

#define __ASM_PATTERN_MAIN_FOOTER\
    "; End of Program: Print -1 as Fine-Output\n"           \
    "mov rdi, -1\n"                                       \
    "call printInt\n"                                     \
    "mov rax, 60\n"                                         \
    "mov rdi, 0\n"                                          \
    "syscall"

#define __ASM_PATTERN_FNC_HEADER \
    "push rbp" \
    "mov rbp, rsp\n"

#define __ASM_PATTERN_FNC_FOOTER \
    "mov rsp, rbp\n" \
    "pop rbp\n" \
    "ret\n\n"


FILE * asm_file = NULL;

FILE * getFile() { return asm_file; }

void __initAsmFile(const char * name, programSymbolTables symbolTabs, Node* tree) {
    char filename[256] = "\0";
    sprintf(filename, "%s.asm", name);
    asm_file = fopen(filename, "w");
    if (!asm_file) raiseError("__initAsmFile : asm_file failed at creation");
    fprintf(asm_file, __ASM_PATTERN_HEADER);
    initGlobalVariables(symbolTabs);
    initTextSection(symbolTabs);  
    initFunctions(symbolTabs);
    fclose(asm_file);
}

void initTextSection(programSymbolTables symbolTabs) {
    fprintf(asm_file, "section .text\n");
    if (findHashElem(symbolTabs.globals, "main") != NULL)
        fprintf(asm_file, "global: main\n");
    fprintf(asm_file, "\n");

}

void initGlobalVariables(programSymbolTables symbolTabs) {
    int amount = symbolTabs.globals.elemAmount;
    HashElem ** elements = HashTableValues(&(symbolTabs.globals));
    fprintf(asm_file, "section .data\n");
    for (int i = 0; i < amount; i++) {
        HashElem* el = elements[i];
        if (el->h_val.type != _type_function) {
            fprintf(asm_file, "%s: %s 0\n", el->h_key, 
                el->h_val.type == _type_char ? SIZE_CHAR : SIZE_INT);
        }
    }
    free(elements);
}


void initMain(programSymbolTables symbolTabs) {
    fprintf(asm_file, "main:\n");
    fprintf(asm_file, "%s", __ASM_PATTERN_MAIN_FOOTER);
}


void initFunctions(programSymbolTables symbolTabs) {
    int amount = symbolTabs.globals.elemAmount;
    HashElem ** elements = HashTableValues(&(symbolTabs.globals));
    for (int i = 0; i < amount; i++) {
        HashElem* el = elements[i];
        if (el->h_val.type == _type_function) {
            if (strcmp(el->h_key, "main") == 0)
                initMain(symbolTabs);
            else {
                fprintf(asm_file, "%s:\n", el->h_key);
                fprintf(asm_file, "%s", __ASM_PATTERN_FNC_HEADER); 
                //initFunctionBody()
                fprintf(asm_file, "%s", __ASM_PATTERN_FNC_FOOTER);    
            }
        }
    }
    free(elements);
}
