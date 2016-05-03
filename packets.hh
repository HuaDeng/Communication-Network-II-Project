#include<click/hashtable.hh>
#include "entries.hh"

struct PacketHeader{
     uint8_t type; //1=hello; 2=update; 3=ack; 4=data 
     uint8_t sequence;
     uint16_t source;
     uint16_t length;

     uint8_t k;
     uint16_t destination1; //the destination address for ack pkt
     uint16_t destination2;
     uint16_t destination3;

     int port;
};
