#include <iostream>
#include "controller.h"
#include "displayqt.h"

void algo(Controller* controller){
   for(unsigned int i=0;i<5000;++i){

	controller->tic();
	usleep(15000);
	controller->toc();
	controller->deplacement_agent(0,controller->proch_case(0).first,controller->proch_case(0).second);
	if(i==20){
	  controller->demande_chemin_algogen(2,180,180);
	  controller->demande_chemin_algogen(1,125,10); // 5,5 -> 400,400
	}
	if(i>20){
	  controller->deplacement_agent(1,controller->proch_case(1).first,controller->proch_case(1).second);
	  controller->deplacement_agent(2,controller->proch_case(2).first,controller->proch_case(2).second);
	}
	if(i==40){
	  controller->demande_chemin_algogen(3,10,170);/*
	  controller->demande_chemin_algogen(6,90,105);*/
	}
	if(i>40){
	  controller->deplacement_agent(3,controller->proch_case(3).first,controller->proch_case(3).second);/*
	  controller->deplacement_agent(6,controller->proch_case(6).first,controller->proch_case(6).second);*/
	}
	if(i==50){
	  controller->demande_chemin_algogen(4,25,15);/*
	  controller->demande_chemin_algogen(5,2,150);
	  controller->demande_chemin_algogen(7,8,8);*/
	}
	if(i>50){
	  controller->deplacement_agent(4,controller->proch_case(4).first,controller->proch_case(4).second);/*
	  controller->deplacement_agent(5,controller->proch_case(5).first,controller->proch_case(5).second);
	  controller->deplacement_agent(7,controller->proch_case(7).first,controller->proch_case(7).second);*/
	}
      }
      controller->tic();
      controller->toc();
}
int main(int argc, char **argv) {
    Controller* controller = Controller::create();
    QApplication app(argc, argv);
    try{
      
      controller->initiateRules("../regle.xml");
      controller->initiateMap("../mapObstacle.txt");
      std::cout << "init map & règles OK" << std::endl;
      controller->creer_agent(25,10,"Cavalier",0);
      controller->creer_agent(10,190,"Cavalier",1);
      controller->creer_agent(10,180,"Artillerie",2);
      controller->creer_agent(175,125,"Cavalier",3);
      controller->creer_agent(75,25,"Infanterie",4);
      /*controller->creer_agent(3,3,"Artillerie",5);
      controller->creer_agent(7,5,"Cavalier",6);
      controller->creer_agent(126,198,"Infanterie",7);*/
      controller->create_algogen();
      std::cout << "Algogen crée" << std::endl;
      controller->demande_chemin_algogen(0,175,175); // 5,5 -> 400,400*/
      
     
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
