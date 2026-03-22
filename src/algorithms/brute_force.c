#include "libs.h"
#include "UI/TUI_func.h"
#include "data_operations/route_struct.h"

Route* brute_force(uint32_t* distances, size_t num_points){

}

/*
function next_permutation(array):
    n = length of array
    i = n - 2
    
    // Step 1: Find the largest index i such that array[i] < array[i + 1]
    while i >= 0 and array[i] >= array[i + 1]:
        i = i - 1
        
    // If no such index exists, we've checked all permutations
    if i < 0:
        return false 
        
    // Step 2: Find the largest index j greater than i such that array[i] < array[j]
    j = n - 1
    while array[j] <= array[i]:
        j = j - 1
        
    // Step 3: Swap the values of array[i] and array[j]
    swap(array[i], array[j])
    
    // Step 4: Reverse the sequence from array[i + 1] up to the end
    left = i + 1
    right = n - 1
    while left < right:
        swap(array[left], array[right])
        left = left + 1
        right = right - 1
        
    return true
*/

/*
function TSP_Brute_Force(distance_matrix, num_cities):
    // Fix city 0 as the start and end point.
    // We only permute the intermediate cities: 1 through (num_cities - 1)
    cities_to_visit = [1, 2, ..., num_cities - 1]
    
    min_path_cost = INFINITY
    best_path = []
    
    has_more_permutations = true
    
    while has_more_permutations == true:
        current_cost = 0
        
        // 1. Add the distance from the Start City (0) to the first city in the array
        current_cost = current_cost + distance_matrix[0][cities_to_visit[0]]
        
        // 2. Add the distances between consecutive cities in the array
        for k from 0 to length(cities_to_visit) - 2:
            from_city = cities_to_visit[k]
            to_city = cities_to_visit[k + 1]
            current_cost = current_cost + distance_matrix[from_city][to_city]
            
        // 3. Add the distance from the last city in the array back to Start City (0)
        last_city = cities_to_visit[length(cities_to_visit) - 1]
        current_cost = current_cost + distance_matrix[last_city][0]
        
        // 4. Compare current_cost with our lowest found cost so far
        if current_cost < min_path_cost:
            min_path_cost = current_cost
            // Create a full path array for the result
            best_path = [0] + copy_of(cities_to_visit) + [0] 
            
        // 5. Generate the next order of cities. 
        // If there are none left, this returns false and the loop ends.
        has_more_permutations = next_permutation(cities_to_visit)
        
    return min_path_cost, best_path
*/
