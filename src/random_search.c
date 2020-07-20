#include "random_search.h"
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#define MT_NO_INLINE
#include <mtwist.h>

#define RANDOM_SEARCH_ERR_MALLOC 1
#define MODULE_RANDOM_SEARCH "random_search"

/* fitness function declaration */
double (*random_search_fitness_func)(double*);

typedef struct random_search_individual {
	double *x;
	double fitness;
} random_search_individual_t;

typedef struct random_search {
	int population_size;
	int number_of_dimensions;
	double *min_x;
	double *max_x;
	random_search_individual_t *individuals;
	double precision;
    int max_iterations;
    algorithm_stats_t algorithm_stats;
} random_search_t;

/* this struct is the "global namespace" of random_search algorithm */
random_search_t random_search;
extern char log_msg[256];

/**
 * Random search allocation and error handling.
 *
 * \param array Array that will be allocated.
 * \param size Size of the array.
 * \param type_size Size of the type of the elements of the array.
 * \param err_msg Error message.
 */
void *random_search_malloc(int size, size_t type_size, char *err_msg)
{
	void *array;
	array = malloc(size * type_size);
	if (array == NULL) {
		printf("[random_search:malloc_error]: %s\n", err_msg);
		exit(RANDOM_SEARCH_ERR_MALLOC);
	}
	return array;
}

void random_search_params_init()
{
	int i;

	random_search.population_size = 50;	/* TODO turn into a configurable variable */
	random_search.number_of_dimensions = 50;	/* TODO turn into a configurable variable */
	random_search.min_x = (double *)random_search_malloc(random_search.number_of_dimensions, sizeof(double), 
			"Não foi possível alocar random_search.min_x.");
	random_search.max_x = (double *)random_search_malloc(random_search.number_of_dimensions, sizeof(double), 
			"Não foi possível alocar random_search.max_x.");
	for (i = 0; i < random_search.number_of_dimensions; ++i) {
		random_search.min_x[i] = -12;	/* TODO turn into a configurable variable */
		random_search.max_x[i] = 12;	/* TODO turn into a configurable variable */
	}
	random_search.precision = 1e-5;	/* TODO turn into a configurable variable */
	random_search.max_iterations = 50000;	/* TODO turn into a configurable variable */
    random_search.algorithm_stats.iterations = 0;
}

void random_search_population_create()
{
	int i;
	char err_msg[256];

	random_search.individuals = (random_search_individual_t *)random_search_malloc(random_search.population_size, sizeof(random_search_individual_t), 
			"Não foi possível alocar random_search.individuals.");
	
	for (i = 0; i < random_search.population_size; ++i) {
		sprintf(err_msg, "Não foi possível alocar random_search.individuals[%d].x.", i);
		random_search.individuals[i].x = (double *)random_search_malloc(random_search.number_of_dimensions, sizeof(double), err_msg);
	}
}

double random_search_random_value_for_dimension(int dimension)
{
	double range;

	range = random_search.max_x[dimension] - random_search.min_x[dimension];
	return random_search.min_x[dimension] + range * mt_ldrand();
}

/**
 * Assign random values to x.
 */
void random_search_random_x(double *x)
{
	int i;

	for (i = 0; i < random_search.number_of_dimensions; ++i)
		x[i] = random_search_random_value_for_dimension(i);
}

void random_search_population_init()
{
	int i;

    assert(random_search.population_size > 0);

    // first iterarion optimized
    random_search_random_x(random_search.individuals[0].x);
    random_search.individuals[0].fitness = random_search_fitness_func(random_search.individuals[0].x);
    random_search.algorithm_stats.avg_fitness = random_search.individuals[0].fitness;
    random_search.algorithm_stats.best_fitness = random_search.individuals[0].fitness;

	for (i = 1; i < random_search.population_size; ++i) {
		random_search_random_x(random_search.individuals[i].x);
		random_search.individuals[i].fitness = random_search_fitness_func(random_search.individuals[i].x);
        random_search.algorithm_stats.avg_fitness = random_search.algorithm_stats.avg_fitness * i / (i + 1) + random_search.individuals[i].fitness / (i + 1);
        if (random_search.individuals[i].fitness < random_search.algorithm_stats.best_fitness)
            random_search.algorithm_stats.best_fitness = random_search.individuals[i].fitness;
	}
}

void random_search_init()
{
	mt_seed();
	random_search_params_init();
	random_search_population_create();
	random_search_population_init();
}

void random_search_solution_assign(double *solution1, double *solution2)
{
	int i;

	for (i = 0; i < random_search.number_of_dimensions; ++i)
		solution1[i] = solution2[i];
}

/**
 * Assign values and fitness of x to individual if it has a better fitness (minimization).
 */
bool random_search_individual_assign_if_better(random_search_individual_t *individual, double *x)
{
	double temp_fitness;
	int i;

	temp_fitness = random_search_fitness_func(x);
	if (temp_fitness < individual->fitness) {
        // update stats                
        random_search.algorithm_stats.avg_fitness -= individual->fitness / random_search.population_size;
        random_search.algorithm_stats.avg_fitness += temp_fitness / random_search.population_size;
        if (temp_fitness < random_search.algorithm_stats.best_fitness)
            random_search.algorithm_stats.best_fitness = temp_fitness;

		random_search_solution_assign(individual->x, x);
		individual->fitness = temp_fitness;
        return true;
	} else
        return false;
}

void random_search_run_iterations(int iterations)
{
	int i, k;
	double *temp_x;

	temp_x = (double *)random_search_malloc(random_search.number_of_dimensions, sizeof(double), "Não foi possível alocar temp_x.");

	for (k = 0; k < iterations; ++k) {
        random_search_random_x(temp_x);
		for (i = 0; i < random_search.population_size; ++i) {
			if (random_search_individual_assign_if_better(&(random_search.individuals[i]), temp_x))
                break;
		}
	}

    random_search.algorithm_stats.iterations += iterations;
}

void random_search_insert_migrant(migrant_t *migrant)
{
	int i;
	int worst = 0;

	if (migrant == NULL) {
		parallel_evolution_log(LOG_PRIORITY_ERR, MODULE_RANDOM_SEARCH, "migrant == NULL.");
	}

	/* find the worst solution */
	for (i = 1; i < random_search.population_size; ++i) {
		if (random_search.individuals[i].fitness > random_search.individuals[worst].fitness)
			worst = i;
	}

	random_search_individual_assign_if_better(&(random_search.individuals[worst]), migrant->var);
}

void random_search_pick_migrant(migrant_t *my_migrant)
{
	int i;
	int best = 0;

	/* find the best solution */
	for (i = 1; i < random_search.population_size; ++i) {
		if (random_search.individuals[i].fitness < random_search.individuals[best].fitness)
			best = i;
	}

	random_search_solution_assign(my_migrant->var, random_search.individuals[best].x);
	my_migrant->var_size = random_search.number_of_dimensions; 
}

int random_search_ended()
{
    sprintf(log_msg, "iterations: %d; avg_fit: %g; best_fit: %g;",
            random_search.algorithm_stats.iterations,
            random_search.algorithm_stats.avg_fitness,
            random_search.algorithm_stats.best_fitness);
    parallel_evolution_log(LOG_PRIORITY_INFO, MODULE_RANDOM_SEARCH, log_msg);

	return (random_search.algorithm_stats.avg_fitness - random_search.algorithm_stats.best_fitness < random_search.precision
            || random_search.algorithm_stats.iterations >= random_search.max_iterations);
}

status_t random_search_get_population(population_t **population)
{
	int i, j;
	migrant_t *new_migrant;

	if (population_create(population, random_search.population_size) != SUCCESS)
		return FAIL;

	for (i = 0; i < random_search.population_size; ++i) {
		if (migrant_create(&new_migrant, random_search.number_of_dimensions) != SUCCESS)
			return FAIL;
		random_search_solution_assign(new_migrant->var, random_search.individuals[i].x);
		population_set_individual(*population, new_migrant, i);
	}

	return SUCCESS;
}

algorithm_stats_t *random_search_get_stats()
{
    return &(random_search.algorithm_stats);
}
