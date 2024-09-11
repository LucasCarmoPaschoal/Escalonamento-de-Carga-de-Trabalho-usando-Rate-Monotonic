#include <stdio.h>
#include <math.h>
#include <json-c/json.h>
#include <stdlib.h>

int compare_descending(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}



int main() {
    FILE *fp;
    struct json_object *parsed_json;
    struct json_object *tarefas;
    struct json_object *tarefa;
    struct json_object *id;
    struct json_object *periodo;
    struct json_object *tempo_execucao;
    struct json_object *prioridade;

    size_t n_tarefas;
    size_t i;

    fp = fopen("tarefas.json", "r");
    if (fp == NULL) {
        fprintf(stderr, "Não foi possível abrir o arquivo.\n");
        return 1;
    }
    int p;
    char buffer[1024];
    fread(buffer, sizeof(buffer), 1, fp);
    fclose(fp);

    parsed_json = json_tokener_parse(buffer);

    json_object_object_get_ex(parsed_json, "tarefas", &tarefas);
    n_tarefas = json_object_array_length(tarefas);

    // 
    int periodos[n_tarefas]; 

    float utilizacao=0,limite_viabilidade;
    //
    for (i = 0; i < n_tarefas; i++) {
        tarefa = json_object_array_get_idx(tarefas, i);

        json_object_object_get_ex(tarefa, "id", &id);
        json_object_object_get_ex(tarefa, "periodo", &periodo);
        json_object_object_get_ex(tarefa, "tempo_execucao", &tempo_execucao);
        
        //
        periodos[i] = json_object_get_int(periodo);
        //
        utilizacao = utilizacao + (float)json_object_get_int(tempo_execucao)/  json_object_get_int(periodo);

        printf("Tarefa %lu:\n", i+1);
        printf("  ID: %s\n", json_object_get_string(id));
        printf("  Período: %d\n", json_object_get_int(periodo));
        printf("  Tempo de Execução: %d\n", json_object_get_int(tempo_execucao));
       
    }
    float expoent=1.0/n_tarefas;
    limite_viabilidade=n_tarefas*(pow(2.0,expoent)-1.0);
    //  
    qsort(periodos, n_tarefas, sizeof(int), compare_descending);
    //
       FILE *file = fopen("saida.json", "w");
     // Verificar se o arquivo foi aberto corretamente 
     if (file == NULL) { printf("Erro ao criar o arquivo.\n");
      return 1;
       } 
       // Escrever o conteúdo JSON no arquivo
       
     if(utilizacao<=limite_viabilidade) { 
         fprintf(file, "{\n");
     
         fprintf(file, " \"schedulability\": \"viable\",\n"); 
         fprintf(file, " \"suggested_schedule\": [\n"); 
         


    for (p = 0; p < n_tarefas; p++) {
        for (i = 0; i < n_tarefas; i++) {
        tarefa = json_object_array_get_idx(tarefas, i);

        json_object_object_get_ex(tarefa, "id", &id);
        json_object_object_get_ex(tarefa, "periodo", &periodo);
        json_object_object_get_ex(tarefa, "tempo_execucao", &tempo_execucao);
        json_object_object_get_ex(tarefa, "prioridade", &prioridade);
        //
        if(periodos[p]==json_object_get_int(periodo)){
            
            fprintf(file, "  {\n"); 
            fprintf(file, "     \"id\": \"%s\",\n",json_object_get_string(id)); 
            fprintf(file, "     \"priority\": %d\n",p+1); 
            if(p+1<n_tarefas){
            fprintf(file, "  },\n"); }else{
                fprintf(file, "  }\n"); 
            }
                  
            }
        //
        }
    }
         fprintf(file, " ]\n"); 
         fprintf(file, "}\n"); 
         // Fechar o arquivo 
         fclose(file);
          
     }else{
        
        fprintf(file, "{\n");
         fprintf(file, " \"schedulability\": \"not viable\"\n"); 
         fprintf(file, "}\n");
     }




    ///////////
    json_object_put(parsed_json); // Libera a memória alocada pelo parsed_json
    return 0;
}