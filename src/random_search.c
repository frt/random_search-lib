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
extern parallel_evolution_t parallel_evolution;

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

int _config_lookup_int(const config_t *config, const char *path, void *value)
{
    return config_lookup_int(config, path, (int *)value);
}

int _config_lookup_int64(const config_t *config, const char *path, void *value)
{
    return config_lookup_int64(config, path, (long long *)value);
}

int _config_lookup_float(const config_t *config, const char *path, void *value)
{
    return config_lookup_float(config, path, (double *)value);
}

int _config_lookup_bool(const config_t *config, const char *path, void *value)
{
    return config_lookup_bool(config, path, (int *)value);
}

int _config_lookup_string(const config_t *config, const char *path, void *value)
{
    return config_lookup_string(config, path, (const char **)value);
}

void log_if_error(int (*config_lookup_fn)(const config_t *, const char *, void *), const config_t *config, const char *path, void *value)
{
    char *logmsg = NULL;
    int n = 0;
    size_t size = 0;

    if (CONFIG_FALSE == config_lookup_fn(config, path, value)) {
        // discover the size of memory needed.
        n = snprintf(logmsg, size, "config error in file '%s' on line %d: %s",
                config_error_file(config),
                config_error_line(config),
                config_error_text(config));
        if (n >= 0) {
            size = (size_t) n + 1;
            logmsg = malloc(size);
            if (logmsg != NULL) {
                snprintf(logmsg, size, "config error in file '%s' on line %d: %s",
                        config_error_file(config),
                        config_error_line(config),
                        config_error_text(config));
                parallel_evolution_log(LOG_PRIORITY_ERR, MODULE_RANDOM_SEARCH, logmsg);
                exit(config_error_type(config));
            }
        }

        parallel_evolution_log(LOG_PRIORITY_ERR, MODULE_RANDOM_SEARCH, "config error");
        exit(config_error_type(config));
    }
}

void random_search_params_init(config_t *config)
{
	int i;

    log_if_error(&_config_lookup_int, config, "random_search.population_size", &random_search.population_size);

	random_search.number_of_dimensions = parallel_evolution.number_of_dimensions;

	random_search.min_x = (double *)random_search_malloc(random_search.number_of_dimensions, sizeof(double), 
			"Não foi possível alocar random_search.min_x.");
	random_search.max_x = (double *)random_search_malloc(random_search.number_of_dimensions, sizeof(double), 
			"Não foi possível alocar random_search.max_x.");
	for (i = 0; i < random_search.number_of_dimensions; ++i) {
		random_search.min_x[i] = parallel_evolution.limits[i].min;
		random_search.max_x[i] = parallel_evolution.limits[i].max;
	}

    log_if_error(&_config_lookup_float, config, "random_search.precision", &random_search.precision);
    log_if_error(&_config_lookup_int, config, "random_search.max_iterations", &random_search.max_iterations);

    random_search.algorithm_stats.iterations = 0;
    random_search.algorithm_stats.fitness_evals = 0;
}

void random_search_population_create()
{
	int i;
	char err_msg[256];

	random_search.individuals = (random_search_individual_t *)random_search_malloc(random_search.population_size, sizeof(random_search_individual_t), 
			"Não foi possível alocar random_search.individuals.");
	
	for (i = 0; i < random_search.population_size; ++i) {
		sprintf(err_msg, "Não foi possível alocar random_search.individuals[%d].x.", i);
		random_search.individuals[i].x = (double *)random_search_malloc(parallel_evolution.number_of_dimensions, sizeof(double), err_msg);
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

	for (i = 0; i < parallel_evolution.number_of_dimensions; ++i)
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
    random_search.algorithm_stats.fitness_evals += random_search.population_size;
}

void random_search_init(config_t *config)
{
	mt_seed();
	random_search_params_init(config);
	random_search_population_create();
	random_search_population_init();
}

void random_search_solution_assign(double *solution1, double *solution2)
{
	int i;

	for (i = 0; i < parallel_evolution.number_of_dimensions; ++i)
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
    ++random_search.algorithm_stats.fitness_evals;
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

/* return the index of the worst solution */
int worst_solution()
{
	int i, worst;

	for (worst = 0, i = 1; i < random_search.population_size; ++i) {
		if (random_search.individuals[i].fitness > random_search.individuals[worst].fitness)
			worst = i;
	}

    return worst;
}

void random_search_run_iterations(int iterations)
{
	int i, k;
	double *temp_x;

	temp_x = (double *)random_search_malloc(parallel_evolution.number_of_dimensions, sizeof(double), "Não foi possível alocar temp_x.");

	for (k = 0; k < iterations; ++k) {
        for (i = 0; i < random_search.population_size; ++i) {
            random_search_random_x(temp_x);
            random_search_individual_assign_if_better(random_search.individuals + i, temp_x);
        }
	}

    random_search.algorithm_stats.iterations += iterations;
}

void random_search_insert_migrant(migrant_t *migrant)
{
	int i;

	if (migrant == NULL) {
		parallel_evolution_log(LOG_PRIORITY_ERR, MODULE_RANDOM_SEARCH, "migrant == NULL.");
	}

	random_search_individual_assign_if_better(&(random_search.individuals[worst_solution()]), migrant->var);
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
	my_migrant->var_size = parallel_evolution.number_of_dimensions; 
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
		if (migrant_create(&new_migrant, parallel_evolution.number_of_dimensions) != SUCCESS)
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
