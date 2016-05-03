#ifndef CLICK_BASICROUTER_HH 
#define CLICK_BASICROUTER_HH 
#include <click/element.hh>
#include <click/timer.hh>
#include <click/hashtable.hh>
#include <click/vector.hh>
#include <click/pair.hh>
#include "packets.hh"
#include "entries.hh"

CLICK_DECLS

class BasicRouter : public Element {
    public:
        BasicRouter();
        ~BasicRouter();
        const char *class_name() const { return "BasicRouter";}
        const char *port_count() const { return "1-/1-";}
        const char *processing() const { return PUSH; }
	int configure(Vector<String> &conf, ErrorHandler *errh);

	void run_timer(Timer*);	
	void push(int port, Packet *packet);
        int initialize(ErrorHandler*);

	HashTable<uint16_t, struct TableEntry> _routing_table;
		
    private:
	Timer _timerHello;
	Timer _timerPortHello;
	Timer _timerUpdate;
	Timer _timerPortUpdate;
	//Timer _timeAck;

	uint8_t _seq;
	uint32_t _periodHello;
	uint32_t _periodUpdate;
	uint32_t _periodPort;
	uint32_t _time_out;
	uint16_t _my_address;
	int _other_address;
	int _num_port;
	int _port_cnt_hello;
	int _port_cnt_update;
}; 

CLICK_ENDDECLS
#endif 
