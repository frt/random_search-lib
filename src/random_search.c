#include "random_search.h"
#include <stdlib.h>
#include <mtwist.h>

#define RANDOM_SEARCH_ERR_MALLOC 1

typedef struct random_search_individual {
	double *x;
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

void random_search_population_init()
{
	int i, j;
	double range;

	for (i = 0; i < random_search.population_size; ++i) {
		for (j = 0; j < random_search.number_of_dimensions; ++j) {
			range = random_search.max_x[j] - random_search.min_x[j];
			random_search.individuals[i].x[j] = random_search.min_x[j] + range * mt_ldrand();
		}
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
}

void random_search_insert_migrant(migrant_t *migrant)
{
}

void random_search_pick_migrant(migrant_t *my_migrant)
{
	int i;

	for (i = 0; i < my_migrant->var_size; ++i)
		my_migrant->var[i] = 42;
}

int random_search_ended()
{
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
