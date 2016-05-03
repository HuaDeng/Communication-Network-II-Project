#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/timer.hh>
#include <click/packet.hh>
#include <click/hashtable.hh>
#include "topoelement.hh" 

CLICK_DECLS 

TopoElement::TopoElement() : _timerHello(this), _timerPort(this), _timerAck(this){
	_seq = 0;
	_periodHello = 5;
	_periodPort = 10;
	_time_out = 1;
	_my_address = 0;
	_other_address = 0;
	_num_port = 0;	
	for(int i = 0; i < 10; i++){
		_ack_status[i] = false;
	}
}

TopoElement::~TopoElement(){
	
}

int TopoElement::initialize(ErrorHandler *errh){
	click_chatter("--------------topo initial--------------");
	_timerHello.initialize(this);
	_timerPort.initialize(this);
	_timerAck.initialize(this);
	_timerHello.schedule_after_sec(_periodHello);	
	return 0;
}

int TopoElement::configure(Vector<String> &conf, ErrorHandler *errh){
	RoutingElement* _temp_routingElement;
	if(cp_va_kparse(conf, this, errh,
			"MY_ADDRESS", cpkN+cpkM, cpUnsigned, &_my_address,
			"ROUTING_ELEMENT", cpkN, cpElementCast, "RoutingElement", &_temp_routingElement,
			"NUMBER_PORT", cpkN+cpkM, cpInteger, &_num_port,
                  	"OTHER_ADDRESS", cpkN, cpUnsigned, &_other_address,
                  	"PERIOD_HELLO", cpkN, cpUnsigned, &_periodHello,
                  	"TIME_OUT", cpkN, cpUnsigned, &_time_out,
                  	cpEnd) < 0) {
    	return -1;
  }
  _routingElement = _temp_routingElement;
  return 0;
}

void TopoElement::run_timer(Timer *timer){ //periodically send hello packet
	if(timer == &_timerHello){
		_port_cnt = 0;
		click_chatter("PERIODICAL: Sending new HELLO packet");
		WritablePacket *hello = Packet::make(0,0,sizeof(struct PacketHeader), 0);
		memset(hello->data(),0,hello->length());
		struct PacketHeader *header = (struct PacketHeader*) hello->data();
		header->type = 1;
		header->source = _my_address;
		header->sequence = _seq;
		header->port = _port_cnt;
		_ack_status[_port_cnt] = true;
		output(0).push(hello);

		/*Print out the current routing table*/
		/*
		click_chatter("TOPO ELEMENT: Router %u routing table", _my_address);

		int entry = 0;
		for(HashTable<uint16_t, struct TableEntry>::iterator _topo_table_iter = _routingElement->_routing_table.begin(); _topo_table_iter != _routingElement->_routing_table.end(); ++_topo_table_iter){
			for(int i = 0; i < _topo_table_iter.value().nextHop.size(); i++){
				click_chatter("*********************Entry %d: Dst %u Cost %d NextHop %d*********************", entry, _topo_table_iter.key(), _topo_table_iter.value().cost, _topo_table_iter.value().nextHop[i]);
			
			}
			entry++;		
		}
		*/
		_timerPort.schedule_after_msec(_periodPort);
		_timerAck.schedule_after_sec(_time_out);
		_timerHello.schedule_after_sec(_periodHello);

	}else if(timer == &_timerPort){ //send periodical hello packet to each port
		_port_cnt++;
		if(_port_cnt < _num_port){
			WritablePacket *hello = Packet::make(0,0,sizeof(struct PacketHeader), 0);
			memset(hello->data(),0,hello->length());
			struct PacketHeader *header = (struct PacketHeader*) hello->data();
			header->type = 1;
			header->source = _my_address;
			header->sequence = _seq;
			header->port = _port_cnt;
			_ack_status[_port_cnt] = true;
			output(0).push(hello);
			_timerPort.schedule_after_msec(_periodPort);
		}

	}else if(timer == &_timerAck){ //ack time out retransmit hello packet
		int _to_cnt = 0;		
		for(int i = 0; i < _num_port; i++){
			if(_ack_status[i]){ //retransmit hello packet to port i
				click_chatter("hello packet ACK time out on port %d", i);
				_to_cnt++;
				WritablePacket *hello = Packet::make(0,0,sizeof(struct PacketHeader), 0);
				memset(hello->data(),0,hello->length());
				struct PacketHeader *header = (struct PacketHeader*) hello->data();
				header->type = 1;
				header->source = _my_address;
				header->sequence = _seq;
				header->port = i;
				_ack_status[i] = true;
				output(0).push(hello);	
			}
		}		
	}
}

void TopoElement::push(int port, Packet *packet) { 
	assert(packet);
	struct PacketHeader *pktheader = (struct PacketHeader *)packet->data();
	if(pktheader->type == 1){ //receiving a hello packet		
		/*Receiving hello packet*/
		click_chatter("Received Hello from %u", pktheader->source);
		if(_routingElement->_routing_table.find(pktheader->source) == _routingElement->_routing_table.end()){
			struct TableEntry _topo_entry_tmp;
			_topo_entry_tmp.cost = 1;
			_topo_entry_tmp.nextHop.push_back(pktheader->port);
			_routingElement->_routing_table.set(pktheader->source, _topo_entry_tmp);
		}
	
		/*Send back ack packet*/
		click_chatter("Send ACK packet back");
		WritablePacket *ack = Packet::make(0,0,sizeof(struct PacketHeader), 0);
		memset(ack->data(),0,ack->length());
		struct PacketHeader *header = (struct PacketHeader*) ack->data();
		header->type = 3; //ACK packet
		header->sequence = pktheader->sequence;
		header->source = _my_address;
		header->destination1 = pktheader->source;
		header->port = pktheader->port;
		packet->kill();
		output(0).push(ack);	
	} else if(pktheader->type == 3){ //receiving ack packet
		click_chatter("Received ack %u from %u", pktheader->sequence, pktheader->source);
		_ack_status[pktheader->port] = false;
	}
	
}

CLICK_ENDDECLS 
EXPORT_ELEMENT(TopoElement)
