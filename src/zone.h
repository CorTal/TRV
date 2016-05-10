#ifndef ZONE_H
#define ZONE_H

#include "minion.h"
#include "surminion.h"

class Zone {
private:
  static unsigned int m_incrID;
  unsigned int m_numero;
  unsigned int m_caseOrig;
  unsigned int m_caseFin;
  Minion* m_leader;
  std::vector<unsigned int> m_cases;
  
public:
  Zone(unsigned int _caseOrig, Minion* _leader, const std::vector<unsigned int> _cases);
  unsigned int get_orig() const;
  unsigned int get_fin() const;
  Minion* get_leader() const;
  const std::vector<unsigned int> &  get_cases() const;
  ~Zone();
};

#endif