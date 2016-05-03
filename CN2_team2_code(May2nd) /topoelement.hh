#ifndef CLICK_TOPOELEMENT_HH 
#define CLICK_TOPOELEMENT_HH 
#include <click/element.hh>
#include <click/timer.hh>
#include <click/hashtable.hh>
#include <click/vector.hh>
#include "entries.hh"

CLICK_DECLS

class TopoElement : public Element {
    public:
        TopoElement();
        ~TopoElement();
        const char *class_name() const { return "TopoElement";}
        const char *port_count() const { return "1/1";}
        const char *processing() const { return PUSH; }
	int configure(Vector<String> &conf, ErrorHandler *errh);

	void run_timer(Timer*);	
	void push(int port, Packet *packet);
        int initialize(ErrorHandler*);

	HashTable<uint16_t, struct TableEntry> _routing_table;
		
    private:
	//HashTable<uint16_t, struct TableEntry> _routing_table;
	
	Timer _timerHello;
	Timer _timerPort;
	Timer _timerAck;

	uint8_t _seq;
	uint32_t _periodHello;
	uint32_t _periodPort;
	uint32_t _time_out;
	uint16_t _my_address;
	int _other_address;
	int _num_port;
	int _port_cnt;
	bool _ack_status[10];
	
}; 

CLICK_ENDDECLS
#endif 
