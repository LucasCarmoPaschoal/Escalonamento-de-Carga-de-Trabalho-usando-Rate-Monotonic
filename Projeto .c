#include <stdio.h>
#include <math.h>
#include <json-c/json.h>
#include <stdlib.h>

int compare_crescente(const void *a, const void *b) {
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
    
    char buffer[1024];
    fread(buffer, sizeof(buffer), 1, fp);
    fclose(fp);

    parsed_json = json_tokener_parse(buffer);

    json_object_object_get_ex(parsed_json, "tarefas", &tarefas);
    n_tarefas = json_object_array_length(tarefas);

    // Declaração de uma variavel para contador de um for
    int p;
    // Declaração de um vetor onde vai armazenar os valores dos periodos
    int periodos[n_tarefas]; 
    // Declaração de uma variavel para medição da utilização e limite que é viavel
    float utilizacao=0,limite_viabilidade;
    //
    for (i = 0; i < n_tarefas; i++) {
        tarefa = json_object_array_get_idx(tarefas, i);

        json_object_object_get_ex(tarefa, "id", &id);
        json_object_object_get_ex(tarefa, "periodo", &periodo);
        json_object_object_get_ex(tarefa, "tempo_execucao", &tempo_execucao);
        
        //Vetor contendo Periodo das tarefas
        periodos[i] = json_object_get_int(periodo);
        //Calculo da utilização
        utilizacao = utilizacao + (float)json_object_get_int(tempo_execucao)/  json_object_get_int(periodo);
        //
        printf("Tarefa %lu:\n", i+1);
        printf("  ID: %s\n", json_object_get_string(id));
        printf("  Período: %d\n", json_object_get_int(periodo));
        printf("  Tempo de Execução: %d\n", json_object_get_int(tempo_execucao));
       
    }
    //Calculo do limite da viabilidade
    float expoent=1.0/n_tarefas;
    limite_viabilidade=n_tarefas*(pow(2.0,expoent)-1.0);
    //  Função que ordena o vetor "periodos" na ordem crescente
    qsort(periodos, n_tarefas, sizeof(int), compare_crescente);
    //Criação do arquivo "saida.json"
       FILE *file = fopen("saida.json", "w");
     // Verificar se o arquivo foi aberto corretamente 
     if (file == NULL) { printf("Erro ao criar o arquivo.\n");
      return 1;
       } 
       // Escrever o conteúdo JSON no arquivo
       //Se for viavel ele dita que é viavel e descreve as tarefas em ordem de prioridade
       // buscando do menor periodo ate o maior periodo
     if(utilizacao<=limite_viabilidade) { 
         
         fprintf(file, "{\n");
    
         fprintf(file, " \"schedulability\": \"viable\",\n"); 
         fprintf(file, " \"suggested_schedule\": [\n"); 
         

    /*emos um for dentro de outro for onde o mais externo é utiliazdo para varer o 
    vetor peridos e o mais interno  vai varer toda a entrada para quando encontrarmos que id 
    coresponde a ele */
    
    for (p = 0; p < n_tarefas; p++) {
        for (i = 0; i < n_tarefas; i++) {
        tarefa = json_object_array_get_idx(tarefas, i);

        json_object_object_get_ex(tarefa, "id", &id);
        json_object_object_get_ex(tarefa, "periodo", &periodo);
        json_object_object_get_ex(tarefa, "tempo_execucao", &tempo_execucao);
        json_object_object_get_ex(tarefa, "prioridade", &prioridade);
        //Compara o perido que buscamos com o periodo atual da for que olha a entrada
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
        // Caso que não é viavel
        fprintf(file, "{\n");
         fprintf(file, " \"schedulability\": \"not viable\"\n"); 
         fprintf(file, "}\n");
     }


    json_object_put(parsed_json); // Libera a memória alocada pelo parsed_json
    return 0;
}
