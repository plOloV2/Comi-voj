# Asymmetric Traveling Salesperson Problem

Projekt realizowany w ramach przedmiotu "Projektowanie Efektywnych Algorytmów" (PEA). Repozytorium zawiera implementację oraz analizę wydajności różnych algorytmów rozwiązujących Asymetryczny Problem Komiwojażera (ATSP).

## Opis projektu

Głównym celem projektu jest praktyczne zbadanie i porównanie złożoności obliczeniowej oraz efektywności wybranych algorytmów. Program posiada interfejs konsolowy umożliwiający wczytywanie danych z pliku, generowanie losowych instancji problemu oraz dokładny pomiar czasu wykonania poszczególnych metod. 

### Zaimplementowane algorytmy

**Etap 1: Algorytmy heurystyczne i przegląd zupełny**
*   **Brute-Force (Przegląd zupełny)** - algorytm dokładny wyznaczający optymalną ścieżkę, służący jako punkt odniesienia do wyliczania błędu względnego dla pozostałych metod.
*   **Nearest Neighbour (NN)** - algorytm zachłanny, szukający najbliższego nieodwiedzonego sąsiada.
*   **Repetitive Nearest Neighbour (RNN)** - wariant algorytmu NN iterowany z uwzględnieniem każdego wierzchołka jako startowego.
*   **Algorytm losowy** - metoda generująca zadaną liczbę ($10 \times N$) unikalnych, losowych permutacji trasy.

**Etap 2: Podział i ograniczenia (Branch and Bound)**
*   **Algorytm Branch and Bound (B&B)** dla problemu ATSP.
*   Eksperymentalne porównanie metod przeszukiwania przestrzeni stanów:
    *   Depth-First Search (DFS)
    *   Breadth-First Search (BFS)
    *   Best-First Search (Lowest-cost)

## Technologie
*   **Język:** C
*   **System budowania:** CMake (wersja minimum 4.1) oraz generator Ninja.
*   **Wielowątkowość:** OpenMP.
*   **Zależności zewnętrzne:** Moja implementacja algorytmu `Xoshiro256**`.

## Jak zbudować i uruchomić

Projekt wykorzystuje system `CMake` do automatyzacji procesu budowania. Projekt zawiera kilka gotowych presetów kompilacji z wykorzystaniem kompilatorów **Clang**, **GCC** oraz **ICX**.
* release_gcc
* release_clang
* release_icx
* debug_clang
* cross_windows_gcc_static

```bash
# Klonowanie repozytorium
git clone https://github.com/plOloV2/Comi-voj
cd Comi-voj

# Kompilacja projektu (mozna zamienic release_gcc na dowolny inny preset)
cmake --preset release_gcc
cmake --build --preset release_gcc
