#include "algogen.h"

Algogen::Algogen(int map_w, int map_h, const std::map< int, Case* >* _sommets, unsigned int _popsize, float _manhattanImportance, float _mutationRatio, float _popToMutate, unsigned int _nbAjouts, float _ratioSupprs, float _ratioModifs, float _ratioElitism, float _cullRatio, unsigned int _nbkids):
m_president(nullptr),m_superman(nullptr),m_ratioElitism(_ratioElitism)
{
  std::srand(std::time(0));
  if(0>_manhattanImportance || 0>_mutationRatio || 0>_popToMutate || 0>_nbAjouts || 0>_ratioSupprs || 0>_ratioModifs || 0>_cullRatio // Checking if arguments are in valid range (todo: factory)
	  || _manhattanImportance>1 || _mutationRatio>1 || _popToMutate>1 || _ratioSupprs>1 || _ratioModifs>1 || _cullRatio>1){
	  std::cerr << "Erreur: les ratios doivent être compris entre 0 et 1 inclus" << std::endl;
	  throw str_exception("Erreur: les ratios doivent être compris entre 0 et 1 inclus");
  }else{
	  m_mapW = map_w;
	  m_mapH = map_h;
	  m_sommets = _sommets;
	  m_popsize=_popsize;
	  m_manhattanImportance=_manhattanImportance;
	  m_mutationRatio=_mutationRatio;
	  m_popToMutate=_popToMutate;
	  m_nbAjouts=_nbAjouts;
	  m_initratioSupprs = _ratioSupprs;
	  m_ratioSupprs=_ratioSupprs;
	  m_initratioModifs = _ratioModifs;
	  m_ratioModifs=_ratioModifs;
	  m_cullRatio=_cullRatio;
	  m_nbkids=_nbkids;
  }
  m_nbkidstotal=0;
  m_nbIterations=0;
}

void Algogen::initPop(int _caseSource, int _caseCible, const Unite* _typeAgent) // Creation of a new sub-population in each individual, at the request of demandeDéplacement()
{
  m_unite.push_back(_typeAgent);	// adding unit type to the relevant container
  m_orig.push_back(m_sommets->at(_caseSource)); // adding new path beginning to the relevant container
  m_cible.push_back(m_sommets->at(_caseCible)); // adding new path ending to the relevant container
  unsigned int originX = m_orig[0]->getX(); // gathering coordinates of those cells
  unsigned int originY = m_orig[0]->getY();
  unsigned int cibleX = m_cible[0]->getX();
  unsigned int cibleY = m_cible[0]->getY();
  int distanceX = originX - cibleX;
  int distanceY = originY - cibleY;
  bool a = cibleX < originX;
  bool b = cibleX == originX;
  bool c = 1-a-b;
  bool d = cibleY < originY;
  bool e = cibleY == originY;
  bool f = 1-d-e;
  bool Astar = ((std::abs(distanceY)>=6) || (std::abs(distanceY)>=3 && std::abs(distanceX)>=3) || (std::abs(distanceX)>=6));
  std::vector<std::pair<bool,bool> *> genome;
  std::vector<Minion*> _minions;
    for(unsigned int i=0;i<8;i++){ // base population consists of 8 total individuals
      if(i<3 && Astar){ // three of them are non-random, and calculated with the A* algorithm at 3 predefined points (one in a straight path, two at 45° angle from the straight path)
	if (!(genome=Map::m_map->A_star_GA(_caseSource, ((originX+3*(b-e+2*(c-f)))*m_mapH+(originY+3*(e-b+2*(f-a)))), nullptr)).empty()) // if the requested A* path is short enough (not calculated if too long)
	{
	  m_pop.push_back(new SurMinion(_minions)); // A new individual is created. This instance regroups all requested paths.
	  m_pop.back()->addMinion(new Minion(genome)); // In this individual, a new path, as defined by A*, is created.
	  i++;
	  genome.clear();
	}
	if (!(genome=Map::m_map->A_star_GA(_caseSource, ((originX+3*((a-c)*(1-e)+2*(c-a)))*m_mapH+(originY+3*((d-f)*(1-b)+2*(e-a-b-c+2*f)))), nullptr)).empty())
	{
	  m_pop.push_back(new SurMinion(_minions)); // the above process is repeated for a different A* path
	  m_pop.back()->addMinion(new Minion(genome));
	  i++;
	  genome.clear();
	}
	if (!(genome=Map::m_map->A_star_GA(_caseSource, ((originX+3*(b-e+2*(f-a)))*m_mapH+(originY+3*(e-b+2*(f-c)))), nullptr)).empty())
	{
	  m_pop.push_back(new SurMinion(_minions)); // and then another
	  m_pop.back()->addMinion(new Minion(genome));
	  i++;
	  genome.clear();
	}
      } // Once all three A* path definitions have been attempted...
      Astar=false;
      if(!Astar){
	for(int j=0;j<6;++j){ // We create the rest of the individuals by assigning them 6 random movements
	  bool bool1 = rand() % 2;
	  bool bool2 = rand() % 2;
	  genome.push_back(new std::pair<bool,bool>(bool1,bool2));
	}
      }
      m_pop.push_back(new SurMinion(_minions));
      m_pop.back()->addMinion(new Minion(genome));
      genome.clear();
    }
  float totalfitness=0.0;
  for (std::vector<SurMinion*>::iterator it = m_pop.begin(); it !=  m_pop.end(); ++it) { // We then run the evaluation function
    evaluate(*it);
    totalfitness+=(*it)->getFitness();
  }
  m_generationTotalFitness.push_back(totalfitness / m_pop.size()); // An average fitness for this generation is then incorporated into the relevant container, for adaptive purposes
}

Algogen::~Algogen() // standard destructor, deleting all objects created in this class ('Minions' are in turn deleted by the Surminion class, which contains the pointers to them)
{
    for (std::vector<SurMinion*>::iterator it = m_pop.begin(); it !=  m_pop.end(); ++it) {
      delete *it;
    }
    for (std::vector<SousMinion*>::iterator it = m_sousMinions.begin(); it!= m_sousMinions.end(); ++it){
      delete *it;
    }
}

void Algogen::crossover(SurMinion* _parent0, SurMinion* _parent1, SurMinion* _parent2) // Creates new individuals based on the genome of three parents
{
for(unsigned int kid=0;kid<m_nbkids;kid++){ // on répète autant qu'on veut créer d'enfants
  std::vector<std::pair<bool,bool>*> kidgenome;
  std::vector<Minion*> minions;
  unsigned int iterations = _parent0->getMinions().size(); // nombre de minions à traiter (identique sur les 3 parents)
  for(unsigned int minion=0;minion<iterations;++minion){ // for sur chaque minion
    Minion* g0 = _parent0->getMinion(minion),*g1 = _parent1->getMinion(minion),*g2=_parent2->getMinion(minion); // recuperation des pointeurs de minion venant des 3 parents
    unsigned int nbchrs=std::max(std::max(g0->getGenomeSize(),g1->getGenomeSize()),g2->getGenomeSize()); // taille genome enfant = max taille genome des 3parents
    unsigned int parent;
    std::vector<Minion*> parents{g0,g1,g2};
    for(unsigned int j=0;j<nbchrs;){
      parent = rand()%(parents.size());
      if(j >= parents.at(parent)->getGenomeSize()){
        parents.erase(parents.begin() + parent);
      }else{
	kidgenome.push_back(new std::pair<bool,bool>(parents.at(parent)->getChromosome(j)->first,parents.at(parent)->getChromosome(j)->second));
	++j;
      } 
    }
    minions.push_back(new Minion(kidgenome));
  }
  m_pop.push_back(new SurMinion(minions));
  m_nbkidstotal++;
  }
} 

Minion* Algogen::crossover(Minion* _parent0, Minion* _parent1, Minion* _parent2)
{
  std::vector<std::pair<bool,bool>*> kidgenome;
  
}



void Algogen::cull()
{
    unsigned int debutRange = (unsigned int)(m_pop.size()/5);
    unsigned int totalSupprs = ceil(m_pop.size() * m_cullRatio);
    for(unsigned int supprs = 0;supprs<totalSupprs;++supprs){
      unsigned int pos = rand() % (m_pop.size() - debutRange) + debutRange;
      m_pop.erase(m_pop.begin()+pos);
    }
}


void Algogen::mutatePop()
{
	int size = (int)(m_pop.size() * m_popToMutate);
	m_lowestElite = (unsigned int)(m_pop.size() * (1 - m_ratioElitism));
	bool elite;
	for (int i=0;i<size;i++) {
	  unsigned int SurMinionToMutate = rand() % m_pop.size();
	  elite = (SurMinionToMutate > m_lowestElite);
	  SurMinion* SM = m_pop.at(SurMinionToMutate);
	  if((m_president!=nullptr && SM->getID()!=m_president->getID()) || m_president==nullptr){
	    unsigned int nbrMinionsToMutate = (unsigned int)(ceil(SM->getNumberMinions() * m_mutationRatio));
	    std::vector<Minion*> minions = SM->getMinions();
	    for(unsigned int i = 0; i < nbrMinionsToMutate; i++){
	      unsigned int minionToMutate = rand()%(minions.size());
	      if(elite){
		std::swap(*(minions.begin()+minionToMutate),minions.back());
		minions.pop_back();
		SM->mutateElite(minionToMutate, m_nbAjouts,m_ratioModifs);
	      }else{
		std::swap(*(minions.begin()+minionToMutate),minions.back());
		minions.pop_back();
		SM->mutate(minionToMutate, m_nbAjouts, m_ratioSupprs,m_ratioModifs);
	      }
	    }
	  }
	}
}



void Algogen::evaluate(SurMinion* _surminion)
{
	float fitnessSM=0.0, fitnessM=0.0,cout;
	bool _vaCheminSM = true;
	unsigned int numAgent=0;
	int newx;
	int newy;
	unsigned int sommet;
	std::vector<int> vec;
	std::vector<float> couts;
	std::vector<std::pair<unsigned int, unsigned int>> vec_conf;
	unsigned int tmps;
	bool _vaChemin = false;
	bool ajout = false;
	for (std::vector<Minion*>::iterator it = _surminion->getMinions().begin(); it < _surminion->getMinions().end(); it++,numAgent++)
	{
	  vec.clear();
	  couts.clear();
	  vec_conf.clear();
	  fitnessM = 0;
	  cout = 0.0;
	  tmps=0;
	  newx = (int)(m_orig.at(numAgent)->getX());
	  newy = (int)(m_orig.at(numAgent)->getY());
	  sommet = (newx*m_mapH) + newy;
	  vec.push_back(sommet);
	  couts.push_back(0.0);
	  _vaChemin=false;
	  vec_conf.push_back(std::pair<unsigned int, unsigned int>(sommet, tmps));
	  std::vector<std::pair<bool,bool>*> genome = _surminion->getMinions().at(numAgent)->getGenome();
	  for(std::vector<std::pair< bool, bool >* >::iterator cit = genome.begin(); cit != genome.end(); ++cit, tmps++){ // parcours du chemin pour détection d'obstacle
	    newx += ((*cit)->second*(1-(2*(*cit)->first)));
	    newy += (((*cit)->second -1) * ((2*(*cit)->first)-1));
	    sommet = (newx*m_mapH) + newy;
	    if (newx < 0 || newx > m_mapW-1 || newy < 0 || newy > m_mapH-1 || m_sommets->at(sommet)->isObstacle()){
	      newx -= ((*cit)->second*(1-(2*(*cit)->first)));
	      newy -= (((*cit)->second -1) * ((2*(*cit)->first)-1));
	      cit=genome.erase(cit);
	      sommet = (newx*m_mapH) + newy;
	      cit--;
	    } else {
	      std::vector<int>::const_iterator debutboucle = std::find(vec.begin(), vec.end(), sommet), debutvec = vec.begin();
	      int pos = std::distance(debutvec,debutboucle);
		if (debutboucle != vec.end()) {
// // 		  std::cout << "Boucle détectée, suppression de " << vec.size() - pos << " chromosomes dans le minion id = " << _surminion->getMinions().at(numAgent)->getID() << std::endl;
		  vec.erase(vec.begin()+pos+1, vec.end());
		  couts.erase(couts.begin()+pos+1,couts.end());
		  cout=*(couts.begin()+pos);
		  cit=genome.erase(genome.begin()+pos, cit+1);
		  cit--;
// 		  while (vec_conf.back().first != sommet)
// 		  {
// 		    vec_conf.pop_back();
// 		    tmps--;
// 		  }
		}else {
		  if(sommet == m_cible.at(numAgent)->get_sommet()){
// 		    std::cout << "Fin de parcours détectée, suppression du reste dans le minion id = " << _surminion->getMinions().at(numAgent)->getID() << std::endl;
		    genome.erase(cit+1,genome.end());
		    vec.erase(vec.begin()+pos, vec.end());
		    couts.erase(couts.begin()+pos,couts.end());
		    cout=couts.back();
		    _vaChemin=true;
		    break;
		  }
		  vec.push_back(sommet);
		  cout+=m_unite.at(numAgent)->getVitesse(m_sommets->at(sommet)->getTerrain());
		  couts.push_back(cout);
		  ajout=false;
		  while(!ajout)
		  {
		    if ((std::find(vec_conf.begin(), vec_conf.end(), std::pair<unsigned int, unsigned int>(sommet, tmps)) == vec_conf.end()))
		    {
		      vec_conf.push_back(std::pair<unsigned int, unsigned int>(sommet,tmps));
		      ajout = true;
		    }
		    else
		    {
		      genome.insert(++cit,nullptr);
		      tmps++;
		    }
		  }
		}
	      }
	    }
	    (*it)->setSommetFinal(vec.back());
	    m_taillemax = std::max(m_taillemax,(unsigned int)genome.size());
	    (*it)->setVaChemin(_vaChemin);
	    int manhattan = 0.0;
	    if(_vaChemin){
	      fitnessM = (float)((float)cout + (float)genome.size());
	    }else{
	      int manhattan = abs(m_cible.at(numAgent)->getX() - m_sommets->at(vec.back())->getX()) + abs(m_cible.at(numAgent)->getY() - m_sommets->at(vec.back())->getY());
	      fitnessM = ((float)manhattan*m_manhattanImportance);
	      _vaCheminSM = false;
	    }
	    (*it)->setFitness(fitnessM);
	    (*it)->setManhattan(manhattan);
	    (*it)->setGenome(genome);
	    fitnessSM+=fitnessM;
	}
	fitnessSM = (float)(fitnessSM / (float) _surminion->getMinions().size());
	_surminion->setFitness(fitnessSM);
	_surminion->setVaChemin(_vaCheminSM);
	if (_vaCheminSM && (m_president==nullptr || !(m_president->getVaChemin()) || m_president->getFitness()>fitnessSM))
	{
	  m_president=_surminion;
	  m_conf_pres = vec_conf;
	}
	else if ( !_vaCheminSM && (m_president==nullptr || (((!m_president->getVaChemin())) && (fitnessSM < m_president->getFitness()))))
	{
	  m_president=_surminion;
	  m_conf_pres = vec_conf;
	}
}

void Algogen::evaluateSSM()
{
  std::vector<std::pair<unsigned int, unsigned int>> vec_conf;
  vec_conf = m_conf_pres;
  int numAgent=0;
  bool ajout;
  int newx;
  int newy;
  unsigned int sommet;
  unsigned int tmps =0;
  for (std::vector<SousMinion*>::iterator it = m_sousMinions.begin(); it < m_sousMinions.end(); it++, numAgent++)
  {
    ajout = false;
    tmps=0;
    newx = (int)(m_orig.at(numAgent)->getX());
    newy = (int)(m_orig.at(numAgent)->getY());
    sommet = (newx*m_mapH) + newy;
    std::vector<std::pair<bool,bool>*> genome = m_sousMinions.at(numAgent)->getGenome();
    for (std::vector<std::pair<bool,bool>*>::iterator cit = genome.begin(); cit < genome.end(); cit++)
    {
      newx += ((*cit)->second*(1-(2*(*cit)->first)));
      newy += (((*cit)->second -1) * ((2*(*cit)->first)-1));
      sommet = (newx*m_mapH) + newy;
      while(!ajout)
	{
	  if ((std::find(vec_conf.begin(), vec_conf.end(), std::pair<unsigned int, unsigned int>(sommet, tmps)) == vec_conf.end()))
	  {
	    vec_conf.push_back(std::pair<unsigned int, unsigned int>(sommet,tmps));
	    ajout = true;
	  }
	  else
	  {
	    genome.insert(++cit,nullptr);
	    tmps++;
	  }
	}
    }
  }
}

void Algogen::iterate()
{
//     std::cout << "AVANT MUTATE !!!!" << std::endl;
//     this->show();
    mutatePop();
//     if(m_generationTotalFitness.size()>2 && m_generationTotalFitness.back() > *(m_generationTotalFitness.end()-1) && m_ratioModifs > (m_initratioModifs/2)){	// Si la fitness générale s'améliore, diminution du taux de mutation
    if(m_generationTotalFitness.back() > m_generationTotalFitness.back() - 1){  
      m_ratioSupprs = m_ratioSupprs * 0.99;
      m_ratioModifs = m_ratioModifs * 0.99;
    }else if(m_ratioModifs < 2*m_initratioModifs){
      m_ratioSupprs = m_ratioSupprs * 1.01;
      m_ratioModifs = m_ratioModifs * 1.01;
    }
//     std::cout << "m_pop avant tri : " << std::endl;
//     for (std::vector<SurMinion*>::iterator i=m_pop.begin(); i!=m_pop.end(); i++)
//     {
//       std::cout << "SurMinion n°" << (*i)->getID() << " | Fitness : " << (*i)->getFitness() << " | varChemin : " << (*i)->getVaChemin() << std::endl;
//     }
//     std::cout << "m_pop après tri : " << std::endl;
//     for (std::vector<SurMinion*>::iterator i=m_pop.begin(); i!=m_pop.end(); i++)
//     {
//       std::cout << "SurMinion n°" << (*i)->getID() << " | Fitness : " << (*i)->getFitness() << " | varChemin : " << (*i)->getVaChemin() << std::endl;
//     }
//     std::cout << "APRES MUTATE, AVANT SUPERMAN" << std::endl;
//     this->show();
    if((m_superman==nullptr || m_pop.front()->getFitness() < m_superman->getFitness()) && m_nbIterations%10==0){
      std::vector<Minion*> superman = m_pop.front()->getMinions();		// création du superman (supersurminion)
      for(unsigned int i = 1;i<m_pop.size();i++){
	for(unsigned int j=0;j<superman.size();j++){
	  if(m_pop.at(i)->getMinion(j)->getFitness() < superman.at(j)->getFitness()){
	    superman.at(j) = new Minion(*m_pop.at(i)->getMinion(j));
	    std::cout << "Nouveau minion créé dans nouveau superman, minion id = " << superman.at(j)->getID() << std::endl;
	  }
	}
      }
      m_pop.push_back(new SurMinion(superman));			// supersurminion ajouté à la pop
      m_superman = m_pop.back();
    }
//     std::cout << "APRES SUPERMAN, AVANT CROSSOVER" << std::endl;
//     this->show();
    SurMinion *sm1=nullptr, *sm2=nullptr, *sm3=nullptr;
    while(m_pop.size()<m_popsize){		// reproduction par rank selection exponentielle tant que la population n'a pas atteint m_popsize
//       if(m_pop.size() < 3){
// 	initPop(m_orig[0]->get_sommet(),m_cible[0]->get_sommet());
//       }

      for (unsigned int i = 1 ;i<m_pop.size();++i){			// probabilité de sélection: 1/2 pour le meilleur individu, 1/4, pour le suivant, 1/8 pour le 3me...
	if((float)(rand()%m_pop.size()) < (float)((1/(pow(2,i))) *  m_pop.size())){
	  if(sm1==nullptr) {sm1=m_pop.at(i-1);
	  }else if(sm2==nullptr && m_pop.at(i-1)!=sm1) {sm2=m_pop.at(i-1);
	  }else if(m_pop.at(i-1)!=sm1 && m_pop.at(i-1)!=sm2){sm3=m_pop.at(i-1); break;
	  }
	}
      }
      if(sm1!=nullptr && sm2!=nullptr && sm3 !=nullptr){
	  crossover(sm1,sm2,sm3);
	  sm1=nullptr;
	  sm2=nullptr;
	  sm3=nullptr;
      }
    }
    float totalfitness=0.0;
//     std::cout << std::endl << "APRES CROSSOVER, AVANT EVALUATE!!!!!!!" << std::endl;
//     this->show();
    for (std::vector<SurMinion*>::iterator it = m_pop.begin(); it !=  m_pop.end(); ++it) {
 	    evaluate(*it);  // evaluation fitness
	    totalfitness+=(*it)->getFitness();
    }
//     std::cout << std::endl << "APRES EVALUATE ET AVANT CULL !!!!!!!" << std::endl;
//     this->show();
    m_generationTotalFitness.push_back(totalfitness);
    std::sort (m_pop.begin(), m_pop.end(), myfonction); // tri
    cull();
//     std::cout << std::endl << "APRES CULL !!!!!!!" << std::endl;
//     this->show();
    m_nbIterations++;
}

// unsigned int Algogen::get_nb_goodResults()
// {
//   unsigned int result = 0;
//   for(unsigned int i=0;i<m_pop.size();i++){
//     if(m_pop.at(i)->getVaChemin()){
//       result++;
//     }
//   }
//   return result;
// }

void Algogen::show() const
{
  std::cout << std::endl << "*******************************************************" << std::endl;
  std::cout << "ITERATION NUMERO " << m_nbIterations << std::endl;
  std::cout << "Stats: " << std::endl;
  std::cout << m_nbkidstotal << " enfants créés." << std::endl;
  std::cout << m_nbIterations << " iterations." << std::endl;

  std::cout << "_______________________" << std::endl << "Fitness des surminions" << std::endl;
  for(unsigned int i=0;i<m_pop.size();++i){
    std::cout << "ID " << m_pop.at(i)->getID() << ", fitness =" << m_pop.at(i)->getFitness() << std::endl;
  }
  
  std::cout << "_______________________" << std::endl << "Toute la population" << std::endl;
  
  for (unsigned int i=0; i<m_pop.size(); ++i)
  {
    for(unsigned int j=0; j<m_pop.at(i)->getMinions().size(); ++j){
      std::vector< std::pair< bool, bool > *> genome = m_pop.at(i)->getMinion(j)->getGenome();
      std::cout << "_****_" << std::endl << "numero : " << j <<  " | id = " << m_pop.at(i)->getMinion(j)->getID() << " | taille genome = " << m_pop.at(i)->getMinion(j)->getGenomeSize() << ", fitness = " << m_pop.at(i)->getMinion(j)->getFitness() << std::endl;
      int newx = (int)(m_orig.at(j)->getX());
      int newy = (int)(m_orig.at(j)->getY());
      std::cout << "Depart en " << newx << ", " << newy << ", case n° " << newx*m_mapH + newy << std::endl;
      for(std::vector< std::pair< bool, bool > *>::iterator cit = genome.begin(); cit != genome.end(); ++cit){
	if(*cit!=nullptr){
	  newx += ((*cit)->second*(1-(2*(*cit)->first)));
	  newy += (((*cit)->second -1) * ((2*(*cit)->first)-1));
	  std::cout << "x = " << newx << ", y = " << newy << ", " << "Case n° " << newx*m_mapH + newy << " | ";
	}else{
	  std::cout << "Attente | ";
	}
      }
      std::cout << std::endl;
    }
  }
  
  std::cout << "_______________________" << std::endl << "Président (id=" << m_president->getID() << ")" << std::endl;
  
  for(unsigned int i=0; i<m_president->getNumberMinions();++i){
//     m_president->getMinion(i);
    std::cout << "Chemin " << i << ": taille genome = " << m_president->getMinion(i)->getGenomeSize() << ", vachemin = " << m_president->getMinion(i)->getVaChemin() << ", genome = " << std::endl;
    int newx = (int)(m_orig.at(i)->getX());
    int newy = (int)(m_orig.at(i)->getY());
    std::vector< std::pair< bool, bool > *> genome = m_president->getMinion(i)->getGenome();
    for(std::vector< std::pair< bool, bool > *>::iterator cit = genome.begin(); cit != genome.end(); ++cit){
      if(*cit!=nullptr){
	newx += ((*cit)->second*(1-(2*(*cit)->first)));
	newy += (((*cit)->second -1) * ((2*(*cit)->first)-1));
	std::cout << "Case n° " << newx*m_mapH + newy << " | de coordonnées : X = " << newx << " Y : " << newy;
      }else{
	std::cout << "Attente | ";
      }
    }
    std::cout << std::endl;
    
  }
}
// 
// unsigned int Algogen::get_pres_nbdeplace()
// {
//   return m_president->getGenomeSize();
// }

void Algogen::addDeplacement(int _idAgent, int _caseSource, int _caseCible, const Unite* _typeAgent)
{
  // vérification des zones etc
  std::vector<unsigned int> m_cases;
  std::vector<std::pair<bool,bool>*> genome;
  bool newleader=true;
  // TO DO 
  
  for(unsigned int i=0;i<m_zones.size();i++){
      m_cases = m_zones.at(i)->get_cases();
      if (m_zones.at(i)->get_fin() == (unsigned int)_caseCible && (std::find(m_cases.begin(), m_cases.end(), _caseCible) != m_cases.end()))
      {
	newleader=false;
	new SousMinion(_idAgent,Map::m_map->A_star_GA(_caseSource, m_zones.at(i)->get_orig(), _typeAgent), genome);
      }
      else
      {
	m_zones.push_back(calcule_Zone(_caseSource, _caseCible));
      }
  }
  if(newleader){
    initPop(_caseSource, _caseCible, _typeAgent);
  }
}

Zone* Algogen::calcule_Zone(int _caseSource, int _caseCible)
{
  std::vector<std::pair<unsigned int, unsigned int>> m_coord;
  std::vector<unsigned int> m_cases;
  std::vector<std::pair<bool,bool>*> genome;
  Minion* _leader = new Minion(genome);
  unsigned int originX = m_sommets->at(_caseSource)->getX();  
  unsigned int originY = m_sommets->at(_caseSource)->getY();
  unsigned int cibleX = m_sommets->at(_caseCible)->getX();
  unsigned int cibleY = m_sommets->at(_caseCible)->getY();
  bool a = cibleX < originX;
  bool b = cibleX == originX;
  bool c = 1-a-b;
  bool d = cibleY < originY;
  bool e = cibleY == originY;
  bool f = 1-d-e;
  unsigned int i = originX-4*c-2*b;
  unsigned int j = originY-4*f-2*e;
  int k = originX+4*a+2*b;
  int l = originY+4*d+2*e;
  if (i<0)
    i=0;
  if (j<0)
    j=0;
  if (k>=m_mapW)
    k=m_mapW-1;
  if (l>=m_mapH)
    l=m_mapH-1;
  for(int x = i; x<=k; ++x)
  {
    for(int y = j; y<=l; ++y)
    {
      m_coord.push_back(std::pair<unsigned int, unsigned int>(x,y));
      m_cases.push_back(x*m_mapH+y);
    }
  }
  for (unsigned int x=0; x<m_cases.size(); ++x)
  {
    if (m_sommets->at(m_cases.at(x))->isObstacle())
    {
      cibleX = m_sommets->at(m_cases.at(x))->getX();
      cibleY = m_sommets->at(m_cases.at(x))->getY();
      a = cibleX < originX;
      b = cibleX == originX;
      c = 1-a-b;
      d = cibleY < originY;
      e = cibleY == originY;
      f = 1-d-e;
      for (std::vector<unsigned int>::iterator g =m_cases.begin(); g<m_cases.end(); ++g)
      {
	unsigned int testX = m_sommets->at(m_cases.at((*g)))->getX();
	unsigned int testY = m_sommets->at(m_cases.at((*g)))->getY();
	if ((a && d) && (testX <= cibleX || testY <= cibleY))
	  m_cases.erase(g);
	else if ((a && e) && (testX <= cibleX && testY == cibleY))
	  m_cases.erase(g);
	else if ((a && f) && (testX <= cibleX || testY >= cibleY))
	  m_cases.erase(g);
	else if ((b && d) && (testX == cibleX && testY <= cibleY))
	  m_cases.erase(g);
	else if ((b && f) && (testX == cibleX && testY >= cibleY))
	  m_cases.erase(g);
	else if ((c && d) && (testX >= cibleX || testY <= cibleY))
	  m_cases.erase(g);
	else if ((c && e) && (testX >= cibleX && testY == cibleY))
	  m_cases.erase(g);
	else if ((c && f) && (testX >= cibleX || testY >= cibleY))
	  m_cases.erase(g);
      }
    }
  }
  return new Zone(_caseSource, _leader , m_cases);
}