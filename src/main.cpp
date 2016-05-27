#include <iostream>
#include "controller.h"
#include "displayqt.h"

void algo(Controller* controller){
      controller->create_algogen();
      int x0,y0,x1,y1,x2,y2,x3,y3,x4,y4;
      x0=15;
      y0=185;
      x1=150;
      y1=150;
      x2=50;
      y2=100;
      x3=25;
      y3=175;
      x4=25;
      y4=25;
      
      controller->demande_chemin_algogen(0,x0,y0);
      controller->demande_chemin_algogen(1,x1,y1);
      controller->demande_chemin_algogen(2,x2,y2);	
      controller->demande_chemin_algogen(3,x3,y3);
      controller->demande_chemin_algogen(4,x4,y4);
      while(1){
	controller->tic();
	usleep(1000);
	controller->toc();
	if(controller->proch_case(0).first==x0 && controller->proch_case(0).second==y0 &&
	  controller->proch_case(1).first==x1 && controller->proch_case(1).second==y1 && 
	  controller->proch_case(2).first==x2 && controller->proch_case(2).second==y2 &&
	  controller->proch_case(3).first==x3 && controller->proch_case(3).second==y3 &&
	  controller->proch_case(4).first==x4 && controller->proch_case(4).second==y4 
	){
	  break;
	}else{
	  controller->deplacement_agent(0,controller->proch_case(0).first,controller->proch_case(0).second);
	  controller->deplacement_agent(1,controller->proch_case(1).first,controller->proch_case(1).second);
	  controller->deplacement_agent(2,controller->proch_case(2).first,controller->proch_case(2).second);
	  controller->deplacement_agent(3,controller->proch_case(3).first,controller->proch_case(3).second);
	  controller->deplacement_agent(4,controller->proch_case(4).first,controller->proch_case(4).second);
	}
      }
}

int main(int argc, char **argv) {
    Controller* controller = Controller::create();
    QApplication app(argc, argv);
    try{
      
      controller->initiateRules("../regle.xml");
      controller->initiateMap("../mapObstacle.txt");
//       std::cout << "init map & règles OK" << std::endl;
      controller->creer_agent(185,15,"Cavalier",0);
      controller->creer_agent(10,10,"Cavalier",1);
      controller->creer_agent(180,100,"Artillerie",2);
      controller->creer_agent(175,25,"Cavalier",3);
      controller->creer_agent(120,120,"Infanterie",4);
      
      /*
      	std::cout << "x0 :" << controller->proch_case(0).first << " y0 : " << controller->proch_case(0).second << std::endl;
	std::cout << "x1 :" << controller->proch_case(1).first << " y1 : " << controller->proch_case(1).second << std::endl;
	std::cout << " x2 :" << controller->proch_case(2).first << " y2 : " << controller->proch_case(2).second <<  std::endl;
	std::cout << " x3 :" << controller->proch_case(3).first << " y3 : " << controller->proch_case(3).second <<  std::endl;
	std::cout << " x4 :" << controller->proch_case(4).first << " y4 : " << controller->proch_case(4).second << std::endl;*/
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
