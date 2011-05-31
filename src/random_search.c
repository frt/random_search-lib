#include "random_search.h"

/*
#include <stdio.h>
#include <stdlib.h>

#include "../include/bfo.h"
#include "../include/report.h"
#include "../include/user_defined.h"

const char* params_file_name = "bfo.in";
*/

void random_search_init()
{
	/*
	initialize_user_defined();

	report_initial();
	report_before_initialize_params();
	bfo_initialize_params();
	report_after_initialize_params();
	report_before_initialize_population();
	ret = bfo_initialize_population();
	if (ret != SUCCESS) {
		printf("ERRO: Não foi possível inicializar a população.\n");
		return ret;
	}
	report_after_initialize_population();
	report_before_initialize_runtime();
	bfo_initialize_runtime();
	report_after_initialize_runtime();
	bfo_dispersal_counter_zero();
	*/
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
