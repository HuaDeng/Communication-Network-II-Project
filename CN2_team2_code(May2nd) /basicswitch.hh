#ifndef CLICK_BASICSWITCH_HH 
#define CLICK_BASICSWITCH_HH 
#include <click/element.hh>
#include <click/timer.hh>

CLICK_DECLS

class BasicSwitch : public Element {
    public:
        BasicSwitch();
        ~BasicSwitch();
        const char *class_name() const { return "BasicSwitch";}
        const char *port_count() const { return "1-/1-";}
        const char *processing() const { return PUSH; }
	int configure(Vector<String> &conf, ErrorHandler *errh);
		
	void push(int port, Packet *packet);
        int initialize(ErrorHandler*);

    private:
	int _num_port;
		
}; 

CLICK_ENDDECLS
#endif 
