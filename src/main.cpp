#include <iostream>
#include "controller.h"
#include "displayqt.h"

void algo(Controller* controller){
   for(unsigned int i=0;i<5000;++i){

	controller->tic();
	usleep(15000);
	controller->toc();
	std::cout << "Agent 0: " << controller->proch_case(0).first << " " << controller->proch_case(0).second << std::endl;
	controller->deplacement_agent(0,controller->proch_case(0).first,controller->proch_case(0).second);
	if(i==20){
	  controller->demande_chemin_algogen(2,125,196);
	  controller->demande_chemin_algogen(1,115,80); // 5,5 -> 400,400
	}
	if(i>20){
	  std::cout << "Itération n°" << i << std::endl;
	  std::cout << "Agent 1: " << controller->proch_case(1).first << " " << controller->proch_case(1).second << std::endl;
	  controller->deplacement_agent(1,controller->proch_case(1).first,controller->proch_case(1).second);
	  std::cout << "Agent 2: " << controller->proch_case(2).first << " " << controller->proch_case(2).second << std::endl;
	  controller->deplacement_agent(2,controller->proch_case(2).first,controller->proch_case(2).second);
	}
	if(i==40){
	  controller->demande_chemin_algogen(3,84,103);
	}
	if(i>40){
	  std::cout << "Agent 3: " << controller->proch_case(3).first << " " << controller->proch_case(3).second << std::endl;
	  controller->deplacement_agent(3,controller->proch_case(3).first,controller->proch_case(3).second);
	}
	if(i==50){
	  controller->demande_chemin_algogen(4,125,78);
	  std::cout << "1" << std::endl;
	  controller->demande_chemin_algogen(5,2,150);
	  std::cout << "2" << std::endl;
	}
	if(i>50){
	  std::cout << "Agent 4: " << controller->proch_case(4).first << " " << controller->proch_case(4).second << std::endl;
	  controller->deplacement_agent(4,controller->proch_case(4).first,controller->proch_case(4).second);
	  std::cout << "Agent 5: " << controller->proch_case(5).first << " " << controller->proch_case(5).second << std::endl;
	  controller->deplacement_agent(5,controller->proch_case(5).first,controller->proch_case(5).second);
	}
      }
      controller->tic();
      controller->toc();
}
int main(int argc, char **argv) {
    std::cout << "TRV" << std::endl;
    Controller* controller = Controller::create();
    QApplication app(argc, argv);
    try{
      
      controller->initiateRules("../regle.xml");
      controller->initiateMap("../map.txt");
      std::cout << "init map & règles OK" << std::endl;
      controller->creer_agent(5,5,"Cavalier",0);
      std::cout << "Cavalier 0 crée" << std::endl;
      controller->creer_agent(4,4,"Cavalier",1);
      std::cout << "Cavalier 1 crée" << std::endl;
      controller->creer_agent(10,10,"Artillerie",2);
      std::cout << "Artillerie 2 créz" << std::endl;
      controller->creer_agent(106,35,"Cavalier",3);
      std::cout << "Cavalier 3 crée" << std::endl;
      controller->creer_agent(79,149,"Infanterie",4);
      std::cout << "Infanterie 4 crée" << std::endl;
      controller->creer_agent(3,3,"Artillerie",5);
      std::cout << "Artillerie 5 crée" << std::endl;
      controller->create_algogen();
      std::cout << "Algogen crée" << std::endl;
      controller->demande_chemin_algogen(0,50,50); // 5,5 -> 400,400
      
     
    } catch(str_exception& e){
    
      std::cout << "str_exception" << e.what() << std::endl;
    }
    catch(std::exception& e1)
    {
      std::cout << e1.what() << std::endl;
    }
    DisplayQT dqt(controller);
    dqt.show();
   std::thread(algo, controller).detach();
    return app.exec();
}
