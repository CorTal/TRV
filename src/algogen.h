#ifndef ALGOGEN_H
#define ALGOGEN_H

#include <vector>
#include <algorithm>
#include <iostream>
#include <set>
#include <ctime>
#include <cstdlib>
#include <math.h>

#include "sousminion.h"
#include "surminion.h"
#include "str_exception.h"
#include "map.h"

class Algogen{

private:
	int m_mapW, m_mapH;  // Map width, map height
	std::map<int, Case*> const * m_sommets; // Pointer to the map's tiles container
	std::vector<Case *>m_orig,m_cible; // Pointers to the requested beginning/ending tiles
	std::vector<SurMinion*> m_pop; // Population of individuals, each of them representing every requested paths (see class SurMinion)
	std::vector<SousMinion*> m_sousMinions; // "sub"-individuals, representing the path taken by units to fall in line with the leader of a group
	std::vector<const Unite*> m_unite; // Storage of unit types, used to get information about the speed and collision parameters of a type of unit
	std::vector<std::pair<std::vector<Case*>,Case*> >m_zones; // Contains the areas subject to falling in line behind a leader rather than finding a new path
	SurMinion* m_president, *m_superman; // Special individuals, respectively the best individual yet (the one whose path is returned, not affected by mutations/deletions/etc) and one that is created by selecting the best paths of all individuals
	unsigned int m_popsize, m_taillemax; // Population size and maximum genome length (!)
	unsigned int m_nbkids; // Number of children generated by every triplet of parents
	unsigned int m_lowestElite; // Not a parameter, depends on ratioElitism and m_popsize
	unsigned int m_nbkidstotal, m_nbIterations; // Total number of children created and iterations done, for reporting purposes
	float m_manhattanImportance; // Relative importance of manhattan distance in fitness formula
	float m_mutationRatio; // Mutation ratio per individual and per generation
	float m_popToMutate; // Population ratio subject to mutations
	unsigned int m_nbAjouts; // Numbre of added chromosomes per generation (for relevant individuals)
	float m_ratioSupprs, m_initratioSupprs, m_ratioModifs, m_initratioModifs; // Initial ratios of chromosomes deletion and modification (subject to change over time, cf adaptive genetic algorithm)
	float m_ratioElitism; // Ratio of individuals considered "elite" (selection by fitness) 
	float m_cullRatio; // Ratio of individuals deleted at each iteration
	std::vector<float> m_generationTotalFitness; // Contains each iteration's global fitness, used to adapt m_ratioSupprs and m_ratioModifs
	std::vector<std::pair<unsigned int,unsigned int>> m_conf_pres; // Container used for collision detection and subsequent genome modifications
	
	void initPop(int _caseSource, int _caseCible,const Unite * _typeAgent); // Creation of a new sub-population in each individual, at the request of demandeDéplacement()
	
	public:
	Algogen(int map_w, int map_h,std::map<int, Case*> const * _sommets, unsigned int _popsize, float _manhattanImportance, float _mutationRatio, 
		  float _popToMutate, unsigned int _nbAjouts, float _ratioSupprs,float _ratioModifs, float _ratioElitism, float _cullRatio, unsigned int _nbkids);
	~Algogen();

	void addDeplacement(int _idAgent, int _caseSource, int _caseCible, const Unite * _typeAgent); // Called by the map: processes a new path request.
	void iterate(); // Main method, calling all genetic algorithm functions. This method should be called a number of times dependant on the map size.

	void crossover(SurMinion* _parent0, SurMinion* _parent1, SurMinion* _parent2); // Creates new individuals based on the genome of three parents
	void mutatePop(); // Mutates the population (see .cpp for details)
	void cull(); // Destroys a number of individuals (see .cpp for details)
	void evaluate(SurMinion* _minion); // Assigns a fitness to an individual as a whole, and to each of its paths. Corrects loops, out of bounds movements, overshootings, and also manages collisions.
	void evaluateSSM(); // Manages collisions for sub-individuals.
	void show() const; // prints statistics and details about the results
// 	unsigned int get_nb_goodResults();
// 	unsigned int get_pres_nbdeplace();
};

#endif
