#include "libs.h"
#include "UI/TUI_func.h"
#include "data_operations/route_struct.h"

// Struktura reprezentująca węzeł w drzewie Little'a
typedef struct {
    uint64_t lower_bound;
    uint32_t* matrix;
    
    // Tablice śledzące, które wiersze i kolumny są jeszcze nie wykreślone
    uint8_t* active_rows;
    uint8_t* active_cols;
    
    // Zapisane krawędzie, które weszły do trasy
    int* path_from;
    int* path_to;
    int edges_count;

} Little_Node;

// Min-Heap
typedef struct {
    Little_Node** heap;
    size_t capacity;
    size_t size;
} Min_Heap;

static Min_Heap* init_min_heap(size_t initial_capacity){

    Min_Heap* pq = malloc(sizeof(Min_Heap));
    if(!pq){
        print_error("Min_Heap alloc for best-first-search failed.\n");
        return NULL;
    }
    
    pq->capacity = initial_capacity;
    pq->size = 0;
    pq->heap = malloc(initial_capacity * sizeof(Little_Node*));

    if(!pq->heap){
        print_error("pq->heap alloc for best-first-search failed.\n");
        free(pq);
        return NULL;
    }

    return pq;

}

static void free_min_heap(Min_Heap* pq){

    for(size_t i = 0; i < pq->size; i++){
        free(pq->heap[i]->matrix);
        free(pq->heap[i]->active_rows);
        free(pq->heap[i]->active_cols);
        free(pq->heap[i]->path_from);
        free(pq->heap[i]->path_to);
        free(pq->heap[i]);
    }

    free(pq->heap);
    free(pq);

}

// Dodawanie do kopca
static void push_min_heap(Min_Heap* pq, Little_Node* node){

    if(pq->size >= pq->capacity){

        pq->capacity *= 2;
        Little_Node** temp = realloc(pq->heap, pq->capacity * sizeof(Little_Node*));
        if(!temp){
            print_error("Heap realloc failed in B&B BestDF.\n");
            free(node->matrix);
            free(node->active_rows);
            free(node->active_cols);
            free(node->path_from);
            free(node->path_to);
            free(node);
            free_min_heap(pq);
            return;
        }

        pq->heap = temp;

    }

    // Heapify
    size_t current = pq->size;
    pq->heap[current] = node;
    pq->size++;

    while(current > 0){
        size_t parent = (current - 1) / 2;

        if(pq->heap[current]->lower_bound < pq->heap[parent]->lower_bound){

            Little_Node* temp_node = pq->heap[current];
            pq->heap[current] = pq->heap[parent];
            pq->heap[parent] = temp_node;
            current = parent;

        }else {
            break;
        }

    }

}

// Pobieranie elementu z najmniejszym Lower Bound (Korzeń)
static Little_Node* pop_min_heap(Min_Heap* pq){
    if(pq->size == 0){
        print_error("Called pop_min_heap on empty heap.\n");
        return NULL;
    }

    Little_Node* min_node = pq->heap[0];
    pq->size--;

    if(pq->size > 0){
        
        pq->heap[0] = pq->heap[pq->size];
        size_t current = 0;

        while(1){

            size_t left = 2 * current + 1;
            size_t right = 2 * current + 2;
            size_t smallest = current;

            if(left < pq->size && pq->heap[left]->lower_bound < pq->heap[smallest]->lower_bound)
                smallest = left;

            if(right < pq->size && pq->heap[right]->lower_bound < pq->heap[smallest]->lower_bound)
                smallest = right;

            if(smallest != current){

                Little_Node* temp_node = pq->heap[current];
                pq->heap[current] = pq->heap[smallest];
                pq->heap[smallest] = temp_node;
                current = smallest;

            }else {
                break;
            }

        }

    }

    return min_node;

}

// Funkcja redukuje macierz w węźle i zwraca koszt tej redukcji
static uint64_t reduce_matrix(Little_Node* node, size_t num_points){

    uint64_t reduction_cost = 0;

    // Redukcja wierszy
    for(size_t i = 0; i < num_points; i++){

        if(!node->active_rows[i])
            continue;

        uint32_t min_val = UINT32_MAX;
        for(size_t j = 0; j < num_points; j++)
            if(node->active_cols[j] && node->matrix[i * num_points + j] < min_val)
                min_val = node->matrix[i * num_points + j];
        
        if(min_val > 0 && min_val != UINT32_MAX){

            reduction_cost += min_val;
            for(size_t j = 0; j < num_points; j++)
                if(node->active_cols[j] && node->matrix[i * num_points + j] != UINT32_MAX)
                    node->matrix[i * num_points + j] -= min_val;
                
        }

    }

    // Redukcja kolumn
    for(size_t j = 0; j < num_points; j++){
        if(!node->active_cols[j])
            continue;

        uint32_t min_val = UINT32_MAX;
        for(size_t i = 0; i < num_points; i++)
            if (node->active_rows[i] && node->matrix[i * num_points + j] < min_val)
                min_val = node->matrix[i * num_points + j];

        if(min_val > 0 && min_val != UINT32_MAX){

            reduction_cost += min_val;
            for (size_t i = 0; i < num_points; i++)
                if (node->active_rows[i] && node->matrix[i * num_points + j] != UINT32_MAX)
                    node->matrix[i * num_points + j] -= min_val;

        }

    }

    return reduction_cost;

}

// Znajdowanie zera o najwyższej "karze"
static void find_best_zero(Little_Node* node, size_t num_points, int* best_row, int* best_col){

    uint32_t max_penalty = 0;
    *best_row = -1;
    *best_col = -1;

    for(size_t i = 0; i < num_points; i++){

        if(!node->active_rows[i])
            continue;
        
        for(size_t j = 0; j < num_points; j++){

            if(!node->active_cols[j])
                continue;

            if(node->matrix[i * num_points + j] == 0){
                
                uint32_t min_row = UINT32_MAX;
                for(size_t k = 0; k < num_points; k++)
                    if(k != j && node->active_cols[k] && node->matrix[i * num_points + k] < min_row)
                        min_row = node->matrix[i * num_points + k];

                uint32_t min_col = UINT32_MAX;
                for(size_t k = 0; k < num_points; k++)
                    if(k != i && node->active_rows[k] && node->matrix[k * num_points + j] < min_col)
                        min_col = node->matrix[k * num_points + j];

                uint32_t current_penalty = (min_row == UINT32_MAX ? 0 : min_row) + (min_col == UINT32_MAX ? 0 : min_col);

                if(current_penalty >= max_penalty || *best_row == -1){
                    max_penalty = current_penalty;
                    *best_row = (int)i;
                    *best_col = (int)j;
                }

            }

        }

    }

}

static void prevent_subtour(Little_Node* node, int u, int v, size_t num_points){
    int start = u;
    int end = v;

    int changed = 1;
    while(changed){

        changed = 0;

        for(int i = 0; i < node->edges_count; i++){

            if(node->path_to[i] == start){
                start = node->path_from[i];
                changed = 1;
            }

            if(node->path_from[i] == end){
                end = node->path_to[i];
                changed = 1;
            }

        }

    }
    
    node->matrix[end * num_points + start] = UINT32_MAX;
    
}

// Tworzenie dzieci
static Little_Node* create_child_node(Little_Node* parent, size_t num_points){

    Little_Node* child = malloc(sizeof(Little_Node));
    if (!child) return NULL;

    child->lower_bound = parent->lower_bound;
    child->edges_count = parent->edges_count;

    child->matrix = malloc(num_points * num_points * sizeof(uint32_t));
    child->active_rows = malloc(num_points * sizeof(uint8_t));
    child->active_cols = malloc(num_points * sizeof(uint8_t));
    child->path_from = malloc(num_points * sizeof(int));
    child->path_to = malloc(num_points * sizeof(int));

    memcpy(child->matrix, parent->matrix, num_points * num_points * sizeof(uint32_t));
    memcpy(child->active_rows, parent->active_rows, num_points * sizeof(uint8_t));
    memcpy(child->active_cols, parent->active_cols, num_points * sizeof(uint8_t));
    memcpy(child->path_from, parent->path_from, num_points * sizeof(int));
    memcpy(child->path_to, parent->path_to, num_points * sizeof(int));

    return child;
    
}

void bb_best(uint32_t* original_distances, size_t num_points, [[maybe_unused]] double timeout_seconds, Route* best_route){

    // Inicjalizacja kopca
    Min_Heap* pq = init_min_heap(num_points * 10);
    if(!pq)
        return;

    // Przygotowanie węzła początkowego
    Little_Node* root = malloc(sizeof(Little_Node));
    root->matrix = malloc(num_points * num_points * sizeof(uint32_t));
    root->active_rows = malloc(num_points * sizeof(uint8_t));
    root->active_cols = malloc(num_points * sizeof(uint8_t));
    root->path_from = malloc(num_points * sizeof(int));
    root->path_to = malloc(num_points * sizeof(int));
    root->edges_count = 0;
    root->lower_bound = 0;

    memcpy(root->matrix, original_distances, num_points * num_points * sizeof(uint32_t));
    for(size_t i = 0; i < num_points; i++){

        root->active_rows[i] = 1;
        root->active_cols[i] = 1;
        
        root->matrix[i * num_points + i] = UINT32_MAX;

    }

    root->lower_bound += reduce_matrix(root, num_points);
    push_min_heap(pq, root);

    // Główna pętla przeszukiwania
    while (pq->size > 0) {

        #ifdef NDEBUG
        if(omp_get_wtime() - best_route->time > timeout_seconds){
            print_error("B&B Best-FS exceeded timeout. Ending execution early.\n");
            break;
        }
        #endif

        // Wyciągamy węzeł z najmniejszym dolnym ograniczeniem
        Little_Node* current = pop_min_heap(pq);

        if(current->lower_bound >= best_route->distance_u){
            free(current->matrix);
            free(current->active_rows);
            free(current->active_cols);
            free(current->path_from);
            free(current->path_to);
            free(current);
            break;
        }

        // Sprawdzenie, czy doszliśmy do liścia
        if(current->edges_count == (int)num_points - 1){
            
            int last_u = -1, last_v = -1;
            for(size_t i = 0; i < num_points; i++){

                if(current->active_rows[i])
                    last_u = (int)i;

                if(current->active_cols[i])
                    last_v = (int)i;

            }
            
            if(last_u != -1 && last_v != -1){

                current->path_from[current->edges_count] = last_u;
                current->path_to[current->edges_count] = last_v;
                current->edges_count++;

            }

            uint8_t* temp_order = calloc(num_points, sizeof(uint8_t));
            int start_node = 0;
            temp_order[0] = 0;
            
            for(size_t step = 1; step < num_points; step++){
                for(int e = 0; e < current->edges_count; e++){
                    if(current->path_from[e] == start_node){

                        start_node = current->path_to[e];
                        temp_order[step] = (uint8_t)start_node;
                        break;

                    }

                }

            }

            best_route->distance_u = current->lower_bound;
            memcpy(best_route->city_order, temp_order, num_points * sizeof(uint8_t));

            free(temp_order);
            free(current->matrix);
            free(current->active_rows);
            free(current->active_cols);
            free(current->path_from);
            free(current->path_to);
            free(current);
            continue;

        }

        int u = -1, v = -1;
        find_best_zero(current, num_points, &u, &v);

        if(u != -1 && v != -1){
            
            // Dziecko "INCLUDE"
            Little_Node* child_include = create_child_node(current, num_points);
            if(child_include){
                child_include->path_from[child_include->edges_count] = u;
                child_include->path_to[child_include->edges_count] = v;
                child_include->edges_count++;

                child_include->active_rows[u] = 0;
                child_include->active_cols[v] = 0;

                prevent_subtour(child_include, u, v, num_points);

                child_include->lower_bound += reduce_matrix(child_include, num_points);

                if(child_include->lower_bound < best_route->distance_u){
                    push_min_heap(pq, child_include);
                }else {

                    free(child_include->matrix); 
                    free(child_include->active_rows);
                    free(child_include->active_cols);
                    free(child_include->path_from);
                    free(child_include->path_to);
                    free(child_include);

                }

            }

            // Dziecko "EXCLUDE"
            Little_Node* child_exclude = create_child_node(current, num_points);
            if(child_exclude){

                child_exclude->matrix[u * num_points + v] = UINT32_MAX;

                child_exclude->lower_bound += reduce_matrix(child_exclude, num_points);

                if(child_exclude->lower_bound < best_route->distance_u){
                    push_min_heap(pq, child_exclude);
                }else {

                    free(child_exclude->matrix);
                    free(child_exclude->active_rows);
                    free(child_exclude->active_cols);
                    free(child_exclude->path_from);
                    free(child_exclude->path_to);
                    free(child_exclude);

                }

            }

        }

        free(current->matrix);
        free(current->active_rows);
        free(current->active_cols);
        free(current->path_from);
        free(current->path_to);
        free(current);

    }

    free_min_heap(pq);

}
