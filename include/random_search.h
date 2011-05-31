#pragma once

#include <parallel_evolution.h>

/**
 * Creates the initial population and other initialization things.
 */
void random_search_init();

/**
 * Runs a number of iterations.
 *
 * @param iterations The number of iterations to run.
 */
void random_search_run_iterations(int iterations);

/** 
 * Inserts a new individual into the population.
 *
 * If the new individual is better than the worst individual in the population,
 * the new individual replaces the worst individual.
 *
 * @param migrant The new individual to be inserted.
 */
void random_search_insert_migrant(migrant_t *migrant);

/**
 * Picks an individual to send to other population.
 *
 * It picks a random individual from the population.
 *
 * @param my_migrant The pointer that will receive the individual.
 */
void random_search_pick_migrant(migrant_t *my_migrant);

/**
 * Termination condition.
 *
 * @return 1 if converged, 0 if not.
 */
int random_search_ended();

/**
 * Makes a copy of the population.
 *
 * Makes a copy of the population suitable to use with parallel_evolution-lib.
 *
 * @param population The pointer that will receive the copy of the population.
 * @return The status, indicating if the copy was successfull or not.
 */
status_t random_search_get_population(population_t **population);
