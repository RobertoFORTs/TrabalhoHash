#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define SEED    0x12345678

typedef struct Kd_municipios{
  char cod_ibge[7];
  char nome[50];
  float latitude;
  float longitude;
  int capital;
  int cod_uf;
  int cod_siafi;
  int ddd;
  char fuso_hor[50];
} cidade;

char * get_key(void * reg){
    return (*((cidade *)reg)).cod_ibge;
}

void * aloca_cidade(char * cod_ibge, char * nome, float latitude, float longitude, int capital, int cod_uf, int cod_siafi, int ddd, char *fuso_hor){
    cidade * cid = malloc(sizeof(cidade));
    strcpy(cid->cod_ibge, cod_ibge);
    strcpy(cid->nome, nome);
    cid->latitude = latitude;
    cid->longitude = longitude;
    cid->capital = capital;
    cid->cod_uf= cod_uf;
    cid->cod_siafi= cod_siafi;
    cid->ddd = ddd;
    strcpy(cid->fuso_hor, fuso_hor);
    return cid;
}

typedef struct {
     uintptr_t * table;
     int size;
     int max;
     uintptr_t deleted;
     char * (*get_key)(void *);
}thash;


uint32_t hashf(const char* str, uint32_t h){
    for (; *str; ++str) {
        h ^= *str;
        h *= 0x5bd1e995;
        h ^= h >> 15;
    }
    return h;
}


void hash_insere(thash * h, void * bucket){
    uint32_t hash = hashf(h->get_key(bucket),SEED);
    int pos = hash % (h->max);
    if (h->max == (h->size+1)){
        free(bucket);
        
    }else{
        while(h->table[pos] != 0){
            if (h->table[pos] == h->deleted)
                break;
            pos = (pos +1) % h->max;
        }
        h->table[pos] = (uintptr_t) bucket;
        h->size +=1;
    }
}



int hash_constroi(thash * h,int nbuckets, char * (*get_key)(void *) ){
    h->table = calloc(sizeof(uintptr_t),nbuckets +1);
    
    if (h->table == NULL){
        return EXIT_FAILURE;
    }
    h->max = nbuckets +1;
    h->size = 0;
    h->deleted = (uintptr_t) & (h->size);
    h->get_key = get_key;
    return EXIT_SUCCESS;
}


void * hash_busca(thash h, const char * key){
    int pos = hashf(key,SEED) %(h.max);
    while(h.table[pos] != 0){
        if (strcmp (h.get_key((void*)h.table[pos]),key) ==0)
            return (void *)h.table[pos];
        else
            pos = (pos+1)%(h.max);
    }
    return NULL;
}

void hash_remove(thash * h, const char * key){
    int pos = hashf(key,SEED) % (h->max);
    while(h->table[pos]!=0){
        if (strcmp (h->get_key((void*)h->table[pos]),key) ==0){
            free((void *) h->table[pos]);
            h->table[pos] = h->deleted;
            h->size -=1;
            return;
        }else{
            pos = (pos+1)%h->max;
        }

    }
    return;
}

void hash_apaga(thash *h){
    int pos;
    for(pos =0;pos< h->max;pos++){
        if (h->table[pos] != 0){
            if (h->table[pos]!=h->deleted){
                free((void *)h->table[pos]);
            }
        }
    }
    free(h->table);
}

void menu(){
    thash *newHash;
    int op = -1;
    FILE *arq;
    arq = fopen("municipios.txt", "r");
    
    if (arq == NULL){
        printf("Não foi possível abrir o arquivo!\n");
    }
    
    while (op != 5){

        printf("------------- HASH PROGRAM -------------\n");
        printf("1 - build Hash(s)\n");
        printf("2 - Search in Hash(s)\n");
        printf("3 - Delete hash\n");
        printf("4 - Delete in hash\n");
        printf("5 - Stop program\n");

        scanf(" %d", &op);

        if (op == 1){

            hash_constroi(newHash, 7000, get_key);
            
            while (!feof(arq)) {

                cidade *aux = malloc(sizeof(cidade));
                fscanf(arq, "%[^,], %[^,], %f, %f, %d, %d, %d, %d, %[^,\n]", &aux->cod_ibge, aux->nome, &aux->latitude, &aux->longitude, &aux->capital, &aux->cod_uf, &aux->cod_siafi, &aux->ddd, aux->fuso_hor);
                hash_insere(newHash, aux);
                printf("to aqui\n");
            }
            fclose(arq);
        }
        
        if (op == 2){
            char codigo[8];
            printf("\nDigite o código IBGE: ");
            scanf(" %s", codigo);
            cidade *resultado = (cidade*)hash_busca(*newHash, codigo);
            if (resultado == NULL){
                printf("\n Município não encontrado!!\n");
            }
            else{
                printf("Município: %s \n", resultado->nome);
            }
             
        }
        
        if (op == 3){
            hash_apaga(newHash);
        }
        
        if (op == 4){
            
            char codigo[50];
            printf("\nDigite o código IBGE: ");
            scanf(" %s", codigo);
            hash_remove(newHash, codigo);
        }
    }   
}

int main(){
    
    thash *newHash;
    int op = -1;
    FILE *arq;
    arq = fopen("municipios.txt", "r");
    
    if (arq == NULL){
        printf("Não foi possível abrir o arquivo!\n");
    }
    
    while (op != 5){

        printf("------------- HASH PROGRAM -------------\n");
        printf("1 - build Hash(s)\n");
        printf("2 - Search in Hash(s)\n");
        printf("3 - Delete hash\n");
        printf("4 - Delete in hash\n");
        printf("5 - Stop program\n");

        scanf(" %d", &op);

        if (op == 1){

            hash_constroi(newHash, 10, get_key);
            
            while (!feof(arq)) {

                cidade *aux = malloc(sizeof(cidade));
                fscanf(arq, "%d, %[^,], %f, %f, %d, %d, %d, %d, %[^,\n]", &aux->cod_ibge, aux->nome, &aux->latitude, &aux->longitude, &aux->capital, &aux->cod_uf, &aux->cod_siafi, &aux->ddd, aux->fuso_hor);
                hash_insere(newHash, aloca_cidade(aux->cod_ibge, aux->nome, aux->latitude,aux->longitude, aux->capital, aux->cod_uf, aux->cod_siafi, aux->ddd, aux->fuso_hor));
            }
            
        }
        
        if (op == 2){
            char codigo[8];
            printf("\nDigite o código IBGE: ");
            scanf(" %s", codigo);
            cidade *resultado = hash_busca(*newHash, codigo);
            
            printf("Município: %s\n", resultado->nome);
            
             
        }
        
        if (op == 3){
            hash_apaga(newHash);
        }
        
        if (op == 4){
            
            char codigo[50];
            printf("\nDigite o código IBGE: ");
            scanf(" %s", codigo);
            hash_remove(newHash, codigo);
        }
    }

    return 0;
}