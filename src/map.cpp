#include "map.h"
Map * Map::m_map;
// *************
// Constructeurs
// *************

Map::Map() : m_w(),m_h(),m_sommets(), m_terrains(), m_agents(), m_unites(), m_contraintes()
{

}

Map::Map(const Map& _map)
{

}


// ************
// Destructeurs
// ************

Map::~Map()
{

}

// On ne veut pas donner accès au constructeur de Controller car on ne souhaite qu'une instance de Controller à la fois
Map* Map::create()
{
  if(Map::m_map==nullptr){ // On regarde si une map existe déjà
    Map::m_map = new Map(); // Si non, on en créé une
    return Map::m_map;
  }else{
    throw new str_exception("Une map existe déjà."); //Si oui, on lève une exception
  }
}

// Fonction d'ajout de Terrain (sans précision d'obstacle) à la map

void Map::addTerrain(const std::string& _type, std::vector< std::pair<std::string const &, float > > & _contraintes_defaut)
{
  // On regarde si le terrain existe déjà au vector m_terrains de la map
  std::vector< std::pair<Contrainte*, float> > vec_cont;
  for(unsigned int i=0;i<m_terrains.size();++i){
    if(m_terrains.at(i)->getType()==_type){
      // Si oui, on lève une exception
      throw new str_exception("Le terrain '" + _type + "' existe déjà");
    }
  }
  // Si non, on créé une nouvelle pair à ajouter au vector m_terrains
  for(unsigned int i=0;i<_contraintes_defaut.size();++i){			// creation du vecteur de contraintes
    Contrainte * t = this->get_Contrainte(_contraintes_defaut.at(i).first);
    vec_cont.push_back(std::make_pair(t,_contraintes_defaut.at(i).second));
  }
  m_terrains.push_back(new Terrain(_type,vec_cont));
}

// Fonction d'ajout de Terrain (avec précision d'obstacle) à la map

void Map::addTerrain(const std::string& _type, std::vector< std::pair<std::string const &, float > > & _contraintes_defaut, bool _obstacle)
{
  // On regarde si le terrain existe déjà au vector m_terrains de la map
  std::vector< std::pair<Contrainte*, float> > vec_cont;
  for(unsigned int i=0;i<m_terrains.size();++i){
    if(m_terrains.at(i)->getType()==_type){
      // Si oui, on lève une exception
      throw new str_exception("Le terrain '" + _type + "' existe déjà");
    }
  }
  // Si non, on créé une nouvelle pair à ajouter au vector m_terrains
  for(unsigned int i=0;i<_contraintes_defaut.size();++i){			// creation du vecteur de contraintes
    Contrainte * t = this->get_Contrainte(_contraintes_defaut.at(i).first);
    vec_cont.push_back(std::make_pair(t,_contraintes_defaut.at(i).second));
  }
  m_terrains.push_back(new Terrain(_type,vec_cont,_obstacle));
}

// Fonction d'ajout d'Unité à la map

void Map::addUnite(const std::string& _type, std::vector< std::pair< std::string const &, float > >& _vitesse_d, 
		   std::vector< std::pair< std::string const &, float > >& _consoContraintes)
{
  // On regarde si l'unité existe déjà au vector m_terrains de la map
  for(unsigned int i=0;i<m_unites.size();++i){
    if(m_unites.at(i)->getType()==_type){
      // Si oui, on lève une exception
      throw new str_exception("Le type d'unité '" + _type + "' existe déjà");
    }
  }
  // Si non, on construit la nouvelle unité puis on l'ajoute au vector m_unites
  std::vector< std::pair<Terrain*, float> > vec_vit;
  std::vector< std::pair<Contrainte*, float> > vec_cont;
  for(unsigned int i=0;i<_vitesse_d.size();++i){			// creation du vecteur de vitesses
    Terrain * t = this->get_Terrain(_vitesse_d.at(i).first);
    vec_vit.push_back(std::pair<Terrain*, float>(t,_vitesse_d.at(i).second));
  }
  for(unsigned int i=0;i<_consoContraintes.size();++i){			// creation du vecteur de contraintes
    Contrainte * t = this->get_Contrainte(_consoContraintes.at(i).first);
    vec_cont.push_back(std::pair<Contrainte*, float>(t,_consoContraintes.at(i).second));
  }
  m_unites.push_back(new Unite(_type,vec_vit,vec_cont));
}

// Fonction d'ajout d'un agent à la map
void Map::addAgent(int _iden, int _x, int _y, std::string const & _unite)
{
  // On aggrandit le vector m_agents s'il n'y a pas assez de place
  unsigned int id = (unsigned int) _iden;
  if(m_agents.size()<=id){
    m_agents.resize(id+10);
  }
  // On récupère le pointeur vers la case de coordonnée x,y du vector m_sommets de la map
  Case * caseptr = m_sommets.at((_x*m_h)+_y);
  Unite * uniteptr = nullptr;
  // On récupère l'unité de nom _unite avec le vector m_unites de map
  for(unsigned int i=0;i<m_unites.size();++i){
    if(m_unites.at(i)->getType()==_unite){
      uniteptr=m_unites.at(i);
      break;
    }
  }
  // On regarde si la case et l'unité existe dans les différents vector de la map
  if(caseptr==nullptr||uniteptr==nullptr){
    // Si non, on lève une exception
    throw new str_exception("Impossible de créer l'agent '" + std::to_string(id) +"': coordonnées ou type d'unité non valides");
  }else{
    // Si oui, on créer et rajoute le nouvelle agent
    m_agents.at(id)=new Agent(id,caseptr,uniteptr);
  }
}

// Fonction d'ajout d'une Contrainte à la map
void Map::addContrainte(std::string const & _contrainte)
{
  // On regarde si la contrainte existe déjà au vector m_terrains de la map
  for(unsigned int i=0;i<m_contraintes.size();++i){
    if(m_contraintes.at(i)->getNom()==_contrainte){
      // Si oui, on lève une exception
      throw new str_exception("La contrainte '" + _contrainte + "' existe déjà");
    }
  }
  // Si non, on créer et rajoute une nouvelle contrainte
  m_contraintes.push_back(new Contrainte(_contrainte));
}

// Fonction pour la déplacement d'un agent d'identifiant id et à la "position" id du vector m_agents vers la case de coordonnées x,y
void Map::move_agent(int id, int x, int y)
{
  // On recherche si l'agent existe dans le vector m_agnets de la map
  unsigned int id2 = (unsigned int) id;
	if (id2 > m_agents.size())
		// Si non, on lève une excpetion
		throw new str_exception("Cette unité n'existe pas");
	// Si oui
	else
	{
		// On regarde si l'unité existe encore
		if (m_agents[id2]==NULL)
			// Si non, on lève une exception
			throw new str_exception("Cette unité n'existe plus");
		else
			// Si oui, on change la case de l'unité
			m_agents[id2]->setCase(Map::m_map->get_Case(x,y));
	}
}

// Fonction pour la suppresion d'un agent d'identifiant id et à la "position" id du vector m_agents
void Map::suppr_agent(int id)
{
  unsigned int id2 = (unsigned int) id;
        // On regarde si l'unité existe encore 
	if (m_agents[id2]==NULL)
		// Si non, on lève une exception
		throw new str_exception("Cette unité n'existe plus");
	else
		// Si oui, on mets le pointeur à NULL
		m_agents[id2]=NULL;
}

// ********
// Getteurs
// ********

int Map::get_m_h() const
{
  return m_h;
}

int Map::get_m_w() const
{
  return m_w;
}

Case* Map::get_Case(int _x, int _y) const
{
  return m_sommets.at((_x*m_h)+_y);
}

Agent* Map::get_Agent(int id) const
{
  unsigned int id2 = (unsigned int) id;
	if (id2 > m_agents.size())
		throw new str_exception("Cette unité n'existe pas");
	else
	{
		if (m_agents[id2]==NULL)
			throw new str_exception("Cette unité n'existe plus");
		else
			return m_agents[id2];
	}
}

Terrain* Map::get_Terrain(const std::string& _terrName) const
{
  for(unsigned int j=0;j<m_terrains.size();++j){
    if(m_terrains.at(j)->getType()==_terrName){
      return m_terrains.at(j);
    }
  }
  throw new str_exception("Erreur: le terrain '" + _terrName + "' n'existe pas.");
}

Contrainte* Map::get_Contrainte(const std::string& _contrName) const
{
  for(unsigned int j=0;j<m_contraintes.size();++j){
    if(m_contraintes.at(j)->getNom()==_contrName){
      return m_contraintes.at(j);
    }
  }
  throw new str_exception("Erreur: la contrainte '" + _contrName + "' n'existe pas.");
}

// ********
// Setteurs
// ********

// Fonction pour changer le type de la case de coordonnée x,y en terrain de type _terrName
void Map::set_Terrain(int _x, int _y, const std::string& _terrName)
{
  Case * c = this->get_Case(_x,_y);
  Terrain * t = this->get_Terrain(_terrName);
  c->setTerrain(t);
}

// Fonction pour changer le type de la case de coordonnée x,y en contrainte de type _contrName
void Map::set_Contrainte(int _x, int _y, const std::__cxx11::string& _contrName)
{
  Case * c = this->get_Case(_x,_y);
  Terrain * terr = this->get_Terrain(_contrName);
  c->setContrainte(terr->getContraintes());
}

// Fonction pour changer le type de la case de coordonnée x,y en contrainte de type _contrName et en obstacle
void Map::set_Contrainte(int _x, int _y, const std::string& _contrName, float _qte)
{
 Case * c = this->get_Case(_x,_y);
 Contrainte * con = this->get_Contrainte(_contrName);
 c->setContrainte(con,_qte);
 
}

// Fonction changeant le booléen obst de la case à la coordonnée x,y
void Map::set_Obstacle(int _x, int _y, int obst)
{
  Case * c = this->get_Case(_x,_y);
  if (obst == 0)
    c->setObstacle(false);
  else
    c->setObstacle(true);
}

// Fonction pour modifier la taille de la map
void Map::set_Taille(int _w, int _h)
{
  m_w = _w;
  m_h = _h;
  int casenum=0;
  for(int i=0;i<_w;++i){
    for(int j=0;j<_h;++j){
      m_sommets.insert( std::pair<int, Case*>(casenum++,new Case(i,j,(i*_h)+j,m_terrains.at(0))) );
    }
  }
}

// Fonction pour tester l'état de la map
void Map::test()
{
  try{
//   for (unsigned int i = 0; i<m_sommets.size(); ++i){
//     std::cout << m_sommets[i]->getX() << std::endl;
//     std::cout << m_sommets[i]->getY() << std::endl;
//     std::cout << m_sommets[i]->get_sommet() << std::endl;
//     std::cout << m_sommets[i]->getTerrain().getType() << std::endl;
//     if (m_sommets[i]->isObstacle())
//       std::cout << "obstacle" << std::endl;
//     for(std::vector<Contrainte*>::iterator i2 = m_contraintes.begin(); i2 != m_contraintes.end(); ++i2){
//       std::cout << (*i2)->getNom() << std::endl; 
//       std::cout << m_sommets[i]->getContrainte(*i2) << std::endl;
//     }
//     std::cout << "//////////////////////////" << std::endl;
//   }
//   std::cout << std::endl;
  std::cout << "agents:" << std::endl;
  std::cout << m_agents.size() << std::endl;
  for(unsigned int i=0;i<m_agents.size();++i){
    if(m_agents[i]!=nullptr){
      std::cout << m_agents[i]->getUnite()->getType() << ", id=" << m_agents[i]->getId() << ", case n°: " << std::to_string(m_agents[i]->getCase()->get_sommet()) << std::endl;
    }
  }
  } catch(str_exception* e)
  {
    std::cout << e->what() << std::endl;
  }
//   std::vector<Terrain*>::iterator i;
//   for(std::vector<Terrain*>::iterator i = m_terrains.begin(); i != m_terrains.end(); ++i){
//     std::cout << (*i)->getType() << std::endl; 
//     std::cout << (*i)->isObstacle() << std::endl;
//     for(std::vector<Unite*>::iterator i1 = m_unites.begin(); i1 != m_unites.end(); ++i1){
//       std::cout << (*i1)->getVitesse(*i) << std::endl;
//     }
//   }
//   for(std::vector<Unite*>::iterator i2 = m_unites.begin(); i2 != m_unites.end(); ++i2){
//     std::cout << (*i2)->getType() << std::endl; 
//   }
//   
//   for(std::vector<Contrainte*>::iterator i = m_contraintes.begin(); i != m_contraintes.end(); ++i){
//     std::cout << (*i)->getNom() << std::endl; 
//     for(std::vector<Unite*>::iterator i1 = m_unites.begin(); i1 != m_unites.end(); ++i1){
//       std::cout << (*i1)->getConso(*i) << std::endl;
//     }
//   }
  
}


