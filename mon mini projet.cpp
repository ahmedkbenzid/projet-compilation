#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h> // For getcwd
#ifdef _WIN32
#include <direct.h> // For Windows systems
#define getcwd _getcwd
#endif

typedef struct Unilex {
    int num;
    const char* ul;
    char att[30];
} Unilex;

typedef struct symboles {
    int num;
    char att[30];
    const char* type;
} symboles;

const char* symbole;
FILE *f;
FILE *fichier;

const char* mot_cle[10] = {"program", "var", "begin", "end", "if", "then", "endif", "writeln", "readln", "int"};
symboles tabSem[255];
Unilex tablesSymboles[255];
int indiceALex = 0;
char caractere;
int termine = 0;
int taille = 0;
int limiteAjout = 0;

// Function declarations
void P();
void DCL();
void Liste_id();
void L();
void Liste_inst();
void I();
void C();
void Exp();
void E();
const char* analLex();
void erreur(const char* erreurMsg);
void reculer(int a);
void carsuivant();
void ajouterTableSymboles(const char* ul);
int RangerId(const char* lexeme);
const char* uniLexId(const char* lexeme);
void ajouter_type(const char* typeS);
const char* chercher_type(const char* nomVar);
void emettre(const char* contenu);

char *strconcatChaine(const char *chaine1, const char *chaine2) {
    size_t len1 = strlen(chaine1);
    size_t len2 = strlen(chaine2);
    size_t lenTotal = len1 + len2 + 1;
    char *result = (char *)malloc(lenTotal);
    if (result == NULL) {
        perror("Erreur lors de l'allocation de mémoire");
        exit(EXIT_FAILURE);
    }
    strcpy(result, chaine1);
    strcat(result, chaine2);
    return result;
}

void accepter(const char *T) {
    if (strcmp(symbole, T) == 0) {
        symbole = analLex();
    } else {
        printf("Erreur de syntaxe: \"%s\" attendu, trouve: \"%s\"\n", T, symbole);
        exit(0);
    }
}

void P() {
    symbole = analLex();
    accepter("program");
    accepter("id");
    accepter(";");
    DCL();
    accepter("begin");
    Liste_inst();
    accepter("end");
    accepter(".");
    printf("\nCompilation Correcte!\n");
}

void DCL() {
    if (strcmp(symbole, "var") == 0) {
        accepter("var");
        Liste_id();
        accepter(":");
        accepter("int");
        accepter(";");
    }
}

void Liste_id() {
    if (strcmp(symbole, "id") == 0) {
            accepter("id");
            L();

    }
}
void L(){
    if (strcmp(symbole, ",") == 0) {
        accepter(",");
        Liste_id();
    }
}

void Liste_inst() {
    I();
    if (strcmp(symbole, ";") == 0) {
        accepter(";");
        Liste_inst();
    }
}

void I() {
    if (strcmp(symbole, "id") == 0) {
        accepter("id");
        const char* var1 = tablesSymboles[indiceALex - 2].att;
        if (!isdigit(var1[0])) {
            char* input1 = strconcatChaine("valeurg @", var1);
            emettre(input1);
            free(input1);
        }
        accepter(":=");
        Exp();
        emettre(":=");
    }
    else if (strcmp(symbole, "writeln") == 0) {
        accepter("writeln");
        accepter("(");
        accepter("id");
        accepter(")");
    }
    else if (strcmp(symbole, "readln") == 0) {
        accepter("readln");
        accepter("(");
        accepter("id");
        accepter(")");
    }
    else if (strcmp(symbole, "if") == 0) {
        accepter("if");
        C();
        accepter("then");
        Liste_inst();
        accepter("endif");
    }
}

void C() {
    Exp();
    accepter("oprel");
    Exp();
}

void Exp() {
    if (strcmp(symbole, "id") == 0) {
        accepter("id");
        const char* var1 = tablesSymboles[indiceALex - 2].att;
        if (!isdigit(var1[0])) {
            char* input1 = strconcatChaine("valeurd @", var1);
            emettre(input1);
            free(input1);
        }
        E();
    }
    else if (strcmp(symbole, "nb") == 0) {
        accepter("nb");
        const char* val = tablesSymboles[indiceALex - 2].att;
        char* input = strconcatChaine("empiler ", val);
        E();
        emettre(input);
        free(input);

    }
    else if (strcmp(symbole, "(") == 0) {
        accepter("(");
        Exp();
        accepter(")");
        E();
    }
}
void E(){

    if (strcmp(symbole, "oparith") == 0) {
        accepter("oparith");
        emettre("+");
        Exp();
        E();
    }
}

void carsuivant() {
    caractere = fgetc(f);
    if (caractere != EOF) {
        if (caractere != '\n' && caractere != '\r') {
            int len = strlen(tablesSymboles[indiceALex].att);
            if (len < 29) {
                tablesSymboles[indiceALex].att[len] = caractere;
                tablesSymboles[indiceALex].att[len + 1] = '\0';
            }
        }
    }
}

void erreur(const char* erreurMsg) {
    termine = 1;
    printf("Erreur !! %s\n", erreurMsg);
}

void reculer(int a) {
    fseek(f, -a, SEEK_CUR);
}

void ajouterTableSymboles(const char* ul) {
    tablesSymboles[indiceALex].ul = ul;
    tablesSymboles[indiceALex].num = indiceALex;
    indiceALex++;
}

void supprimerEspaces(char *chaine) {
    int i, j = 0;
    for (i = 0; chaine[i] != '\0'; i++) {
        if (chaine[i] != ' ' && chaine[i] != '\t') {
            chaine[j++] = chaine[i];
        }
    }
    chaine[j] = '\0';
}

int RangerId(const char* lexeme) {
    supprimerEspaces((char*)lexeme);
    for (int i = 0; i < 10; i++) {
        if (strcmp(mot_cle[i], lexeme) == 0) {
            return 0;
        }
    }
    return 1;
}

const char* uniLexId(const char* lexeme) {
    supprimerEspaces((char*)lexeme);
    return (RangerId(lexeme) != 0) ? "id" : lexeme;
}

const char* analLex() {
    int etat = 0;
    int len;

    memset(tablesSymboles[indiceALex].att, 0, 30);

    while (!termine) {

        switch(etat) {
            case 0:
                carsuivant();
                if (caractere == EOF) {
                    termine = 1;
                    return "EOF";
                }

                if(isalpha(caractere)) {
                    etat = 1;
                }
                else if(isdigit(caractere)) etat = 3;
                else if(caractere == ':') etat = 24;
                else if(caractere == ',') etat = 29;
                else if(caractere == '\t' || caractere == ' ' || caractere == '\n' || caractere == '\r') {
                    etat = 0;
                }
                else if(caractere == ';') etat = 25;
                else if(caractere == '.') etat = 26;
                else if(caractere == '+') etat = 5;
                else if(caractere == '=') etat = 8;
                else if(caractere == '<') etat = 9;
                else if(caractere == '>') etat = 13;
                else if(caractere == '*') etat = 16;
                else if(caractere == '(') etat = 30;
                else if(caractere == ')') etat = 31;
                else etat=21;
                break;

            case 1:
                carsuivant();
                if(isalnum(caractere)) {
                    etat = 1;
                } else {
                    etat = 2;
                }
                break;

            case 2: {
                reculer(1);
                len = strlen(tablesSymboles[indiceALex].att);
                if(len > 0 && caractere != '\n') {
                    tablesSymboles[indiceALex].att[len-1] = '\0';
                }
                const char* id = uniLexId(tablesSymboles[indiceALex].att);
                tablesSymboles[indiceALex].ul = id;
                tablesSymboles[indiceALex].num = indiceALex;
                indiceALex++;
                return id;
            }

            case 3:
                carsuivant();
                if(isdigit(caractere)) {
                    etat = 3;
                } else {
                    etat = 4;
                }
                break;

            case 4: {
                reculer(1);
                len = strlen(tablesSymboles[indiceALex].att);
                tablesSymboles[indiceALex].att[len-1] = '\0';
                ajouterTableSymboles("nb");
                return "nb";
            }

            case 5:
                ajouterTableSymboles("oparith");
                return "oparith";

            case 8:
                ajouterTableSymboles("oprel");
                return "oprel";

            case 9 :
                carsuivant();
                if(caractere=='>')
                    etat=10;
                else if(caractere == '=')
                    etat=11;
                else
                    etat=12;
                break;
            case 10:
                ajouterTableSymboles("oprel");
                return "oprel";
            case 11:
                 ajouterTableSymboles("oprel");
                 return "oprel";
            case 12:
                 ajouterTableSymboles("oprel");
                 reculer(1);
                return "oprel";
            case 13 :
                carsuivant();

                if(caractere=='=')
                    etat=15;
                else
                {
                    etat=14;
                }
                break;
            case 14:
                  ajouterTableSymboles("oprel");
                  reculer(1);
                 return "oprel";

            case 16 :
                 ajouterTableSymboles("oparith");

                 return "oparith";

            case 21 :
                termine=1;
                return "EOF";


            case 24 :
                carsuivant();
                if(caractere=='=')
                    etat=27;
                else
                    etat=28;
                break;
            case 25 :
                 ajouterTableSymboles("PVR");
                 return ";";
            case 26 :
                ajouterTableSymboles("pt");
                return ".";
            case 27 :
                ajouterTableSymboles("AFF");
                 return ":=";
            case 28 :
                ajouterTableSymboles("DIP");
                reculer(1);
                return ":";
            case 29 :
                ajouterTableSymboles("VR");
                return ",";
            case 30 :
                ajouterTableSymboles("(");
                return "(";
             case 31 :
                ajouterTableSymboles(")");
                return ")";
            default :
                etat=22;


        }

    }
}

int compatible(char* a, char* b)
{
    if (strcmp(a, b) == 0)
        return 0;
    return 1;
}

void ajouter_type(const char* typeS) {
    int i = indiceALex - 1;
    while (i > limiteAjout) {
        if (tablesSymboles[i].ul != NULL) {
            if (strcmp(tablesSymboles[i].ul, "id") == 0) {
                tabSem[taille].num = taille;
                tabSem[taille].type = typeS;
                strcpy(tabSem[taille].att, tablesSymboles[i].att);
                taille++;
            }
        }
        i--;
    }
    limiteAjout = indiceALex;
}

const char* chercher_type(const char* nomVar) {
    for (int i = 0; i < taille; i++) {
        if (strcmp(tabSem[i].att, nomVar) == 0) {
            return tabSem[i].type;
        }
    }
    return "erreur_de_type";
}

void emettre(const char* contenu) {
    fprintf(fichier, "%s\n", contenu);
}

int main() {
    // Clear everything at start
    memset(tablesSymboles, 0, sizeof(tablesSymboles));
    indiceALex = 0;
    termine = 0;

    printf("Opening files...\n");

    // Open the input file in binary mode to avoid text translation issues
    f = fopen("test.txt", "rb");
    if (f == NULL) {
        printf("Error: Cannot open input file 'test.txt'\n");
        printf("Current working directory: ");
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        }
        return 1;
    }

    // Check if file is empty
    fseek(f, 0, SEEK_END);
    long fileSize = ftell(f);
    if (fileSize == 0) {
        printf("Error: Input file is empty\n");
        fclose(f);
        return 1;
    }

    // Go back to start of file
    rewind(f);

    // Read and print first line for debugging
    char firstLine[256] = {0};
    if (fgets(firstLine, sizeof(firstLine), f) != NULL) {
        printf("First line of file: '%s'\n", firstLine);
    }

    // Go back to start again
    rewind(f);

    fichier = fopen("codepile.txt", "w");
    if (fichier == NULL) {
        printf("Error opening output file\n");
        fclose(f);
        return 1;
    }
    P();

    fclose(fichier);
    fclose(f);
    return 0;
}
