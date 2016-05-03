#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/timer.hh>
#include <click/packet.hh>
#include <click/hashtable.hh>
#include "routingelement.hh" 
#include "packets.hh"

CLICK_DECLS 

RoutingElement::RoutingElement() : _timerUpdate(this), _timerPort(this){
	_seq = 0;
	_periodUpdate = 5;
	_periodPort = 10;
	_time_out = 1;
	_my_address = 0;
	_other_address = 0;
	_num_port = 1;	
	//TopoElement _topoElement;
}

RoutingElement::~RoutingElement(){
	
}

int RoutingElement::initialize(ErrorHandler *errh){
	_timerUpdate.initialize(this);
	_timerPort.initialize(this);
	_timerUpdate.schedule_after_sec(2*_periodUpdate);	
	return 0;
}

int RoutingElement::configure(Vector<String> &conf, ErrorHandler *errh){
	if(cp_va_kparse(conf, this, errh,
			"MY_ADDRESS", cpkP+cpkM, cpUnsigned, &_my_address,
			"TOPO_ELEMENT", cpkP+cpkM, cpElement, &_topoElement,
			"NUMBER_PORT", cpkP+cpkM, cpInteger, &_num_port,
                  	"OTHER_ADDRESS", cpkP, cpUnsigned, &_other_address,
                  	"PERIOD_UPDATE", cpkP, cpUnsigned, &_periodUpdate,
                  	"TIME_OUT", cpkP, cpUnsigned, &_time_out,
                  	cpEnd) < 0) {
    	return -1;
  }
  return 0;
}

void RoutingElement::run_timer(Timer *timer){ //periodically send update packet
	if(timer == &_timerUpdate){
		click_chatter("PERIODICAL: Sending new update packet");
		WritablePacket *packet = Packet::make(0,0,sizeof(struct PacketHeader) + sizeof(HashTable<uint16_t, struct TableEntry>), 0);
		memset(packet->data(),0,packet->length());
		struct PacketHeader *header = (struct PacketHeader*) packet->data();
		header->type = 2; //update packet
		header->source = _my_address;
		header->sequence = _seq;
		header->port = 0;
		header->length = sizeof(_topoElement._routing_table)*_topoElement._routing_table.size();
		//click_chatter("size of _routing table %d", _topoElement._routing_table.size());
		HashTable<uint16_t, struct TableEntry> *data = (HashTable<uint16_t, TableEntry> *)(packet->data() + sizeof(struct PacketHeader));
		memcpy(data, &_topoElement._routing_table, header->length);
		output(0).push(packet);

		//Print out routing table
		click_chatter("Router %u routing table", _my_address);
		click_chatter("size of routing table %d", _topoElement._routing_table.size());
		int entry = 0;
		for(HashTable<uint16_t, struct TableEntry>::iterator _topo_table_iter = _topoElement._routing_table.begin(); _topo_table_iter != _topoElement._routing_table.end(); ++_topo_table_iter){
			for(int i = 0; i < _topo_table_iter.value().nextHop.size(); i++){
				click_chatter("*********************Entry %d: Dst %u Cost %d NextHop %d*********************", entry, _topo_table_iter.key(), _topo_table_iter.value().cost, _topo_table_iter.value().nextHop[i]);
			
			}
			entry++;		
		}
		_timerPort.schedule_after_msec(_periodPort);
		_timerUpdate.schedule_after_sec(_periodUpdate);
	}else if(timer == &_timerPort){
		_port_cnt++;
		if(_port_cnt < _num_port){
			WritablePacket *packet = Packet::make(0,0,sizeof(struct PacketHeader) + sizeof(HashTable<uint16_t, struct TableEntry>), 0);
			memset(packet->data(),0,packet->length());
			struct PacketHeader *header = (struct PacketHeader*) packet->data();
			header->type = 2; //update packet
			header->source = _my_address;
			header->sequence = _seq;
			header->port = _port_cnt;
			header->length = sizeof(_topoElement._routing_table)*_topoElement._routing_table.size();
			click_chatter("size of _routing table %d", _topoElement._routing_table.size());
			HashTable<uint16_t, struct TableEntry> *data = (HashTable<uint16_t, TableEntry> *)(packet->data() + sizeof(struct PacketHeader));
			memcpy(data, &_topoElement._routing_table, header->length);
			output(0).push(packet);
			_timerPort.schedule_after_msec(_periodPort);
		}
	}
}

void RoutingElement::push(int port, Packet *packet) { //receiving a update packet
	assert(packet);
	struct PacketHeader *header = (struct PacketHeader *)packet->data();
	/*Receiving update packet*/
	click_chatter("Received Update from %u", header->source);
	HashTable<uint16_t, struct TableEntry> *_update_table_ptr = (HashTable<uint16_t, struct TableEntry> *)(packet->data() + sizeof(struct PacketHeader));
	HashTable<uint16_t, struct TableEntry> _update_table;
	memcpy(&_update_table, _update_table_ptr, header->length);

	bool flag;
 
	/*Adds 1 to all costs in the update table*/
	for(HashTable<uint16_t, struct TableEntry>::iterator _update_addr_iter = _update_table.begin(); _update_addr_iter != _update_table.end(); ++_update_addr_iter){
		struct TableEntry _update_entry = _update_table.get(_update_addr_iter.key());
		_update_entry.cost++;
		_update_table.set(_update_addr_iter.key(), _update_entry);
	}

	/*Compare update table with local routing table*/
	for(HashTable<uint16_t, struct TableEntry>::iterator _update_addr_iter = _update_table.begin(); _update_addr_iter != _update_table.end(); ++_update_addr_iter){
		/*Address in the update entry is not present in local routing table*/
		if(_topoElement._routing_table.find(_update_addr_iter.key()) == _topoElement._routing_table.end()){
			_topoElement._routing_table.set(_update_addr_iter.key(), _update_addr_iter.value());
		}else{ /*Address in the update entry is present in local routing table*/
			struct TableEntry _update_entry = _update_table.get(_update_addr_iter.key());
			struct TableEntry _routing_entry = _topoElement._routing_table.get(_update_addr_iter.key());
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
				_topoElement._routing_table.set(_update_addr_iter.key(), _new_routing_entry);
			}
		}		
	}
	
	/*Send back ack packet*/
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
