#include "topoelement.hh"

struct UpdateTable{
	Vector<uint16_t> _dstAddr;
	Vector<int> _cost;
	Vector< Vector<int> > _nextHop;	
};
