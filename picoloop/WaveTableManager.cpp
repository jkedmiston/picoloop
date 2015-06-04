#include "WaveTableManager.h"

//WaveTableManager::WaveTableManager() : wtvector(0,WaveTable())
WaveTableManager::WaveTableManager() : wtvector()
{
  size=0;
}


WaveTableManager::~WaveTableManager()
{

}


WaveTableManager& WaveTableManager::getInstance()
{
  static WaveTableManager instance;
  return instance;
}


int WaveTableManager::getSize()
{
  return wtvector.size();
}
/*
void WaveTableManager::addWaveTable(WaveTable * WT)
{
  //wtvector.insert(WT);
  wtvector.push_back(WT);
  //wtvector[0]=WT;
}
*/

void WaveTableManager::insert(WaveTable * WT,int position)
{
  if (wtvector.size()<=position)
    wtvector.resize(position+1);
  wtvector[position]=WT;
}


WaveTable * WaveTableManager::get(int position)
{
  if (position<wtvector.size())
    return wtvector[position];
  else
    {
      printf("WaveTable * WaveTableManager::get(%d) fails",position);
      exit(1);
    }
}




/*
void WaveTableManager::setSize(int nbelem)
{
  size=nbelem;
}
*/
