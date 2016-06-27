#include "plcFunc.h"



IO dcy(INPUT, 4, "DCY", "Bouton DCY", 500);
IO acq(INPUT, 5, "ACQ", "Bouton ACQ", 500);
IO kau(INPUT, 6, "KAU", "Arret d'urgence", 0);
IO ec1(INPUT, 7, "EC1", "", 0);
IO ec2(INPUT, 8, "EC2", "", 0);
IO ec3(INPUT, 9, "EC3", "", 0);
IO ec4(INPUT, 10, "EC4", "", 0);




IO km1(OUTPUT, 12, "KM1", "Pompe remplissage 1", 0);

TEMPO tempo1(5000);
TEMPO tempo2(20000);
PULSE voyant(3000);

GRAFCET cycle1("Cycle principale de lavage", 20, 0, 0);
GRAFCET securite("GRAFCET DE SECURITE", 2, 0, 0);

void setup()
{
  InitPlc();
  debugPlc("test de liaison");
  dcy.debug();
  acq.debug();
  km1.debug();
  cycle1.debug();
  securite.debug();
  //tempo1.debug();
  tempo1.INIT();
  //tempo2.debug();
  tempo2.INIT();
}

void loop()
{
  systemPlc();
  dcy.updateState();
  acq.updateState();
  kau.updateState();
  ec1.updateState();
  ec2.updateState();
  ec3.updateState();
  ec4.updateState();



  
  km1.updateState();

  
  if(dcy.Q() && cycle1.Q(0))
  {
    cycle1.NEXT("LANCEMENT DU CYCLE ATTENTE OUVERPURE VPE1");
    tempo1.INIT();
  }

  if(cycle1.Q(1) && tempo1.FIN())
  {
    cycle1.NEXT();
    tempo2.INIT();
  }

  if(cycle1.Q(2) && tempo2.FIN())
  {
    if(voyant.Q()) km1.I();
  }
  
  

  //SECURITE

  if(!kau.Q() && securite.Q(0)) securite.NEXT();

  if(kau.Q() && securite.Q(1)) securite.NEXT();

  if(kau.Q() && securite.Q(2) && ec1.Q() && ec2.Q() && ec3.Q() && ec4.Q()) securite.NEXT();

  
}
