#include "libs.h"
#include "UI/TUI_func.h"
#include "data_operations/route_struct.h"

// Struktura reprezentująca węzeł w drzewie Little'a
typedef struct {
    uint64_t lower_bound;
    uint32_t* matrix;
    
    // Tablice śledzące, które wiersze i kolumny są jeszcze aktywne (nie wykreślone)
    uint8_t* active_rows;
    uint8_t* active_cols;
    
    // Zapisane krawędzie (skąd -> dokąd), które na pewno weszły do trasy
    int* path_from;
    int* path_to;
    int edges_count;         // Ile krawędzi już wybraliśmy

} Little_Node;

// Struktura Kolejki Priorytetowej (Min-Heap)
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

// Deklaracje funkcji pomocniczych do operacji na macierzy
// (redukcja macierzy, obliczanie kar za zera, wykluczanie cykli)
// ...

// --- GŁÓWNA FUNKCJA LITTLE'A ---
void bb_best(uint32_t* distances, size_t num_points, [[maybe_unused]] double timeout_seconds, Route* best_route, uint32_t* min_out){
    // 1. Inicjalizacja kopca
    // 2. Skopiowanie oryginalnej macierzy do węzła początkowego (Root)
    // 3. Wstępna redukcja wierszy i kolumn
    // 4. Główna pętla wyciągająca najmniejszy Lower Bound z kopca
    
    return;
}
