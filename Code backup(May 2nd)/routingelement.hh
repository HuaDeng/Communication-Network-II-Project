#ifndef CLICK_ROUTINGELEMENT_HH 
#define CLICK_ROUTINGELEMENT_HH 
#include <click/element.hh>
#include <click/timer.hh>
#include <click/hashtable.hh>
#include <click/vector.hh>
#include <click/pair.hh>
#include "topoelement.hh"

CLICK_DECLS

class RoutingElement : public Element {
    public:
        RoutingElement();
        ~RoutingElement();
        const char *class_name() const { return "RoutingElement";}
        const char *port_count() const { return "1/1";}
        const char *processing() const { return PUSH; }
	int configure(Vector<String> &conf, ErrorHandler *errh);

	void run_timer(Timer*);	
	void push(int port, Packet *packet);
        int initialize(ErrorHandler*);
	TopoElement _topoElement;
	
		
    private:
	
	Timer _timerUpdate;
	Timer _timerPort;
	//Timer _timeAck;

	uint8_t _seq;
	uint32_t _periodUpdate;
	uint32_t _periodPort;
	uint32_t _time_out;
	uint16_t _my_address;
	int _other_address;
	int _port_cnt;
	int _num_port;
}; 

CLICK_ENDDECLS
#endif 
