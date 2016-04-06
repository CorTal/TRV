#ifndef TERRAIN_H
#define TERRAIN_H
#include <string>
#include <vector>
#include "str_exception.h"
class Terrain{
private:
  std::string m_type;
  std::vector<std::pair<std::string, float> > m_contraintes_defaut;
  bool m_obstacle;
public:
  Terrain(std::string & _type, std::vector<std::pair<std::string, float> > _contraintes_defaut, bool _obstacle);
  Terrain(std::string & _type, std::vector<std::pair<std::string, float> > _contraintes_defaut);
  ~Terrain();
  
  std::string const & getType();
  float getContrainteDef(std::string const & _nomContrainte);
  bool isObstacle();
};
#endif