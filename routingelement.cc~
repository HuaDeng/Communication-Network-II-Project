#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/timer.hh>
#include <click/packet.hh>
#include <click/hashtable.hh>
#include "routingelement.hh" 

CLICK_DECLS 

RoutingElement::RoutingElement() : _timerUpdate(this), _timerPort(this){
	_seq = 0;
	_periodUpdate = 5;
	_periodPort = 10;
	_time_out = 1;
	_my_address = 0;
	_other_address = 0;
	_num_port = 1;	
}

RoutingElement::~RoutingElement(){
	
}

int RoutingElement::initialize(ErrorHandler *errh){
	_timerUpdate.initialize(this);
	_timerPort.initialize(this);
	_timerUpdate.schedule_after_sec(5*_periodUpdate + 2);	
	return 0;
}

int RoutingElement::configure(Vector<String> &conf, ErrorHandler *errh){
	if(cp_va_kparse(conf, this, errh,
			"MY_ADDRESS", cpkN+cpkM, cpUnsigned, &_my_address,
			"NUMBER_PORT", cpkN+cpkM, cpInteger, &_num_port,
                  	"OTHER_ADDRESS", cpkN, cpUnsigned, &_other_address,
                  	"PERIOD_UPDATE", cpkN, cpUnsigned, &_periodUpdate,
                  	"TIME_OUT", cpkN, cpUnsigned, &_time_out,
                  	cpEnd) < 0) {
    	return -1;
  }
  return 0;
}

void RoutingElement::run_timer(Timer *timer){ //periodically send update packet
	if(timer == &_timerUpdate){
		click_chatter("PERIODICAL: Sending new UPDATE packet");
		WritablePacket *packet = Packet::make(0,0,sizeof(struct PacketHeader) + sizeof(HashTable<uint16_t, struct TableEntry>), 0);
		memset(packet->data(),0,packet->length());
		struct PacketHeader *header = (struct PacketHeader*) packet->data();
		header->type = 2; //update packet
		header->source = _my_address;
		header->sequence = _seq;
		header->port = 0;

		click_chatter("ROUTING ELEMENT: Info of ready update packet for port %d", header->port);
		click_chatter("ROUTING ELEMENT: Type %u Source %u", header->type, header->source);

		//Print out routing table
		click_chatter("ROUTING ELEMENT: Router %u routing table before putting in package", _my_address);
		int entry = 0;
		for(HashTable<uint16_t, struct TableEntry>::iterator _topo_table_iter = _routing_table.begin(); _topo_table_iter != _routing_table.end(); ++_topo_table_iter){
			for(int i = 0; i < _topo_table_iter.value().nextHop.size(); i++){
				click_chatter("*********************Entry %d: Dst %u Cost %d NextHop %d*********************", entry, _topo_table_iter.key(), _topo_table_iter.value().cost, _topo_table_iter.value().nextHop[i]);		
			}
			//click_chatter("ROUTING ELEMENT: BREAK3!");
			entry++;		
		}
		header->length = sizeof(HashTable<uint16_t, struct TableEntry>);
		char *data = (char *)(packet->data() + sizeof(struct PacketHeader));
		memcpy(data, &_routing_table, header->length);

		click_chatter("ROUTING ELEMENT DEBUG: Router %u routing table after putting in package", _my_address);		
		HashTable<uint16_t, struct TableEntry> _update_table;
		//char *dataR = (char *)(packet->data() + sizeof(struct PacketHeader));
		HashTable<uint16_t, struct TableEntry> *dataR = (HashTable<uint16_t, struct TableEntry> *)(packet->data() + sizeof(struct PacketHeader));
		memcpy(&_update_table, dataR, header->length);
		
		click_chatter("ROUTING ELEMENT DEBUG: Routing table read");	
		entry = 0;
		for(HashTable<uint16_t, struct TableEntry>::iterator _topo_table_iter = _update_table.begin(); _topo_table_iter != _update_table.end(); ++_topo_table_iter){
			for(int i = 0; i < _topo_table_iter.value().nextHop.size(); i++){
				click_chatter("******************TEST Entry %d: Dst %u Cost %d NextHop %d******************", entry, _topo_table_iter.key(), _topo_table_iter.value().cost, _topo_table_iter.value().nextHop[i]);		
			}
			//click_chatter("ROUTING ELEMENT: BREAK3!");
			entry++;		
		}
		
		output(0).push(packet);
		click_chatter("ROUTING ELEMENT: UPDATE packet is out for port %d", header->port);

		
		
		_timerPort.schedule_after_msec(_periodPort);
		_timerUpdate.schedule_after_sec(_periodUpdate);
	}else if(timer == &_timerPort){
		/*
		_port_cnt++;
		if(_port_cnt < _num_port){
			//WritablePacket *packet = Packet::make(0,0,sizeof(struct PacketHeader) + sizeof(struct UpdateTable), 0);
			WritablePacket *packet = Packet::make(0,0,sizeof(struct PacketHeader), 0);
			memset(packet->data(),0,packet->length());
			struct PacketHeader *header = (struct PacketHeader*) packet->data();
			header->type = 2; //update packet
			header->source = _my_address;
			header->sequence = _seq;
			header->port = _port_cnt;

			click_chatter("ROUTING ELEMENT: Info of ready update packet for port %d", header->port);
			click_chatter("ROUTING ELEMENT: Type %u Source %u", header->type, header->source);
			
			output(0).push(packet);
			click_chatter("ROUTING ELEMENT: UPDATE packet is out for port %d", header->port);
			_timerPort.schedule_after_msec(_periodPort);
		}
		*/
	}
}

void RoutingElement::push(int port, Packet *packet) { //receiving a update packet
	assert(packet);
	click_chatter("ROUTING ELEMENT: Received UPDATE packet");
	struct PacketHeader *header = (struct PacketHeader *)packet->data();
	/*Receiving update packet*/
	click_chatter("ROUTING ELEMENT: Received Update from %u of length %u", header->source, header->length);

	click_chatter("ROUTING ELEMENT: Received packet header type %u", header->type);
	click_chatter("ROUTING ELEMENT: Received packet header source %u", header->source);

	//char *dataR = (char*)(packet->data() + sizeof(struct PacketHeader));
	HashTable<uint16_t, struct TableEntry> *dataR = (HashTable<uint16_t, struct TableEntry> *)(packet->data() + sizeof(struct PacketHeader));
	HashTable<uint16_t, struct TableEntry> _update_table;
	
	click_chatter("ROUTING ELEMENT: Read Packet!");
	memcpy(&_update_table, dataR, header->length);

	click_chatter("ROUTING ELEMENT: -----------------------------------------");
	int entry = 0;
	
	for(HashTable<uint16_t, struct TableEntry>::iterator _topo_table_iter = _update_table.begin(); _topo_table_iter != _update_table.end(); ++_topo_table_iter){
		for(int i = 0; i < _topo_table_iter.value().nextHop.size(); i++){
			click_chatter("*********************Entry %d: Dst %u Cost %d NextHop %d*********************", entry, _topo_table_iter.key(), _topo_table_iter.value().cost, _topo_table_iter.value().nextHop[i]);
		
		}
		entry++;		
	}
	
	click_chatter("ROUTING ELEMENT: -----------------------------------------");
	
	bool flag;
 
	//Adds 1 to all costs in the update table
	for(HashTable<uint16_t, struct TableEntry>::iterator _update_addr_iter = _update_table.begin(); _update_addr_iter != _update_table.end(); ++_update_addr_iter){
		click_chatter("ROUTING ELEMENT: Entering add update table");		
		struct TableEntry _update_entry = _update_table.get(_update_addr_iter.key());
		_update_entry.cost++;
		_update_table.set(_update_addr_iter.key(), _update_entry);
	}

	click_chatter("ROUTING ELEMENT: update packet add 1");
	//Compare update table with local routing table
	for(HashTable<uint16_t, struct TableEntry>::iterator _update_addr_iter = _update_table.begin(); _update_addr_iter != _update_table.end(); ++_update_addr_iter){
		//Address in the update entry is not present in local routing table
		if(_routing_table.find(_update_addr_iter.key()) == _routing_table.end()){
			_routing_table.set(_update_addr_iter.key(), _update_addr_iter.value());
		}else{ //Address in the update entry is present in local routing table
			struct TableEntry _update_entry = _update_table.get(_update_addr_iter.key());
			struct TableEntry _routing_entry = _routing_table.get(_update_addr_iter.key());
			if(_update_entry.cost == _routing_entry.cost){ //add entry of update table with different next hop to local routing table
				for(int i = 0; i < _update_entry.nextHop.size(); i++){
					flag = true;
					for(int j = 0; j < _routing_entry.nextHop.size(); j++){
						if(_update_entry.nextHop[i] == _routing_entry.nextHop[j]){
							flag = false;
							break;
						}
					}
					if(flag){
						_routing_entry.nextHop.push_back(_update_entry.nextHop[i]);
					}
				}
			}else if(_update_entry.cost < _routing_entry.cost){ //replace entry of local routing table with entry of update table
				struct TableEntry _new_routing_entry;
				_new_routing_entry.cost = _update_entry.cost;
				for(int i = 0; i < _update_entry.nextHop.size(); i++){
					_new_routing_entry.nextHop.push_back(_update_entry.nextHop[i]);
				}
				_routing_table.set(_update_addr_iter.key(), _new_routing_entry);
			}
		}		
	}
	
	//Send back ack packet
	click_chatter("Send ACK packet back");
	WritablePacket *ack = Packet::make(0,0,sizeof(struct PacketHeader), 0);
	memset(ack->data(),0,ack->length());
	struct PacketHeader *format = (struct PacketHeader*) ack->data();
	format->type = 3; //ACK packet
	format->sequence = header->sequence;
	format->source = _my_address;
	format->destination1 = header->source;
	format->port = header->port;
	packet->kill();
	output(0).push(ack);
			
}

CLICK_ENDDECLS 
EXPORT_ELEMENT(RoutingElement)
