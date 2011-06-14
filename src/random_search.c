#include "random_search.h"
#include <stdlib.h>
#include <mtwist.h>

#define RANDOM_SEARCH_ERR_MALLOC 1

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
} random_search_t;

/* this struct is the "global namespace" of random_search algorithm */
random_search_t random_search;

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

	random_search.population_size = 50;
	random_search.number_of_dimensions = 50;
	random_search.min_x = (double *)random_search_malloc(random_search.number_of_dimensions, sizeof(double), 
			"Não foi possível alocar random_search.min_x.");
	random_search.max_x = (double *)random_search_malloc(random_search.number_of_dimensions, sizeof(double), 
			"Não foi possível alocar random_search.max_x.");
	for (i = 0; i < random_search.number_of_dimensions; ++i) {
		random_search.min_x[i] = -12;
		random_search.max_x[i] = 12;
	}
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

	for (i = 0; i < random_search.population_size; ++i) {
		random_search_random_x(random_search.individuals[i].x);
		random_search.individuals[i].fitness = random_search_fitness_func(random_search.individuals[i].x);
	}
}

void random_search_init()
{
	mt_seed();
	random_search_params_init();
	random_search_population_create();
	random_search_population_init();
}

void random_search_run_iterations(int iterations)
{
	int i, j, k;
	double *temp_x;
	double temp_fitness;

	temp_x = (double *)random_search_malloc(random_search.number_of_dimensions, sizeof(double), "Não foi possível alocar temp_x.");

	for (k = 0; k < iterations; ++k) {
		for (i = 0; i < random_search.population_size; ++i) {
			random_search_random_x(temp_x);
			temp_fitness = random_search_fitness_func(temp_x);
			if (temp_fitness < random_search.individuals[i].fitness) {
				for (j = 0; j < random_search.number_of_dimensions; ++j)
					random_search.individuals[i].x[j] = temp_x[j];
				random_search.individuals[i].fitness = temp_fitness;
			}
		}
	}
}

void random_search_insert_migrant(migrant_t *migrant)
{
	int i;
	int worst = 0;

	for (i = 1; i < random_search.population_size; ++i) {
		if (random_search.individuals[i].fitness > random_search.individuals[worst])
			worst = i;
	}

	if (random_search_fitness_func(migrant) < random_search.individuals[worst])
random_search.individuals[worst] = 
}

void random_search_pick_migrant(migrant_t *my_migrant)
{
	/* TODO */
	int i;

	for (i = 0; i < my_migrant->var_size; ++i)
		my_migrant->var[i] = 42;
}

int random_search_ended()
{
	/* TODO */
	static int it = 0;
	int ret;

	if (it < 10)
		ret = 0;
	else
		ret = 1;

	++it;
	return ret;
}

status_t random_search_get_population(population_t **population)
{
	/* TODO */
	int i, j;
	migrant_t *new_migrant;

	if (population_create(population, 5) != SUCCESS)
		return FAIL;

	for (i = 0; i < 5; ++i) {
		if (migrant_create(&new_migrant, 50) != SUCCESS)
			return FAIL;
		for (j = 0; j < new_migrant->var_size; ++j)
			new_migrant->var[j] = 42;
		population_set_individual(*population, new_migrant, i);
	}

	return SUCCESS;
}
