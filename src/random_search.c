#include "random_search.h"
#include <stdlib.h>

#define RANDOM_SEARCH_ERR_MALLOC 1

typedef struct random_search {
	int population_size;
	int number_of_dimensions;
	double *min_x;
	double *max_x;
} random_search_t;

/* this struct is the "global namespace" of random_search algorithm */
random_search_t random_search;

void random_search_malloc_double_array(double **double_array, int size, char *err_msg)
{
	*double_array = (double*)malloc(size * sizeof(double));
	if (*double_array == NULL) {
		printf("[random_search:malloc_error]: %s\n", err_msg);
		exit(RANDOM_SEARCH_ERR_MALLOC);
	}
}

void random_search_init()
{
	int pop_size = 50;
	int dims = 50

	random_search.population_size = pop_size;
	random_search.number_of_dimensions = dims;
	random_search_malloc_double_array(&(random_search.min_x), dims, "Não foi possível alocar random_search.min_x.");
	random_search_malloc_double_array(&(random_search.max_x), dims, "Não foi possível alocar random_search.max_x.");
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
