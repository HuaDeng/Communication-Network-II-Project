require(library /home/comnetsii/elements/lossyrouterport.click);

define($dev1 veth1, $addrDev1 4e:2b:87:a6:be:cd);
define($dev2 veth2, $addrDev2 4a:11:5f:3c:0e:00);
define($dev3 veth3, $addrDev3 8e:fd:95:c6:10:f5);
define($dev4 veth4, $addrDev4 8e:36:2f:2c:1c:f0);
define($dev5 veth5, $addrDev5 42:c1:aa:ec:16:fc);
define($dev6 veth6, $addrDev6 2a:e1:24:4e:7f:f9);
define($dev7 veth7, $addrDev7 ca:00:68:89:9e:aa);
define($dev8 veth8, $addrDev8 2e:21:77:0f:95:97);
define($dev9 veth9, $addrDev9 4a:4b:3e:f2:9c:d8);
define($dev10 veth10, $addrDev10 62:de:9c:5a:07:91);
define($dev11 veth11, $addrDev11 f2:e9:33:3c:2d:2d);
define($dev12 veth12, $addrDev12 16:92:ba:1f:42:6b);
define($dev13 veth13, $addrDev13 7e:44:c8:48:11:62);
define($dev14 veth14, $addrDev14 32:9b:be:11:22:10);
define($dev15 veth15, $addrDev15 92:d1:bc:f2:9f:3e);
define($dev16 veth16, $addrDev16 5e:ae:5b:12:8a:5e);
define($dev17 veth17, $addrDev17 b6:97:28:f7:e1:d6);
define($dev18 veth18, $addrDev18 aa:d5:4e:ae:ea:d8);
define($dev19 veth19, $addrDev19 32:78:de:98:20:de);
define($dev20 veth20, $addrDev20 52:77:c4:d0:0d:b0);

rp1::LossyRouterPort(DEV $dev1, IN_MAC $addrDev1, OUT_MAC $addrDev2, LOSS 0.98, DELAY 0.2);
rp2::LossyRouterPort(DEV $dev3, IN_MAC $addrDev3, OUT_MAC $addrDev4, LOSS 0.98, DELAY 0.2);

bc::BasicClassifier();
bs::BasicSwitch(NUMBER_PORT 2);
te::TopoElement(MY_ADDRESS 101, NUMBER_PORT 2);
//re::RoutingElement(MY_ADDRESS 101, TOPO_ELEMENT te, NUMBER_PORT 2);


rp1->[0]bc;
rp2->[1]bc;

bc[0]->[0]te;

bc[1]->Discard;
//bc[1]->[0]re;
bc[2]->Discard;

te[0]->[0]bs;
//re[0]->[1]bs;


bs[0]->rp1;
bs[1]->rp2;
