#include "displayqt.h"

#include <sstream>

using namespace std;

/// ####################################
///     Constructeurs et destructeurs
/// ####################################
//@{

DisplayQT::DisplayQT(Controller* contr):
    QWidget(), algorun(false)
{	
    controller = contr;
    QObject::connect(contr,SIGNAL(sendPath()), this, SLOT(presidentReceived()),Qt::DirectConnection);
    buffer = new QImage;
    color = new QColor(Qt::white);
    bufferPainter= new QPainter;
    rubber = NULL;

    setMinimumSize(controller->get_map()->get_m_w()*2, controller->get_map()->get_m_h()*2);
}

DisplayQT::~DisplayQT()
{
    delete buffer;
    delete color;
    delete bufferPainter;
    delete rubber;
    Controller::delete_controller();
}

//@}
///###################
///		Setters
///###################

void DisplayQT::setColor(Colors colorIndice)
{
    switch(colorIndice){
    case LightBlue:
        this->color->setRgb(150,190,220);
        break;
    case Black:
        this->color->setRgb(00,00,00);
        break;
    case Gray:
        this->color->setRgb(65,65,65);
        break;
    case Red:
        this->color->setRgb(255,0,0);
        break;
    case Green:
      this->color->setRgb(0,153,0);
      break;
    case LightYellow:
      this->color->setRgb(255,255,204);
      break;
    case Purple:
      this->color->setRgb(102,0,153);
      break;
    case GreenKaki:
      this->color->setRgb(102,102,0);
      break;
    case White :
    default :
        this->color->setRgb(255, 255, 255);
    }
}

void DisplayQT::setColor(int R, int G, int B)
{
  this->color->setRgb(R,G,B);
}

/// ####################
///		Affichages
/// ####################
//@{

void DisplayQT::drawCell(int colonne, int ligne)
{
    QPen pe;
//    pe.setWidth(4);
//    pe.setBrush(QColor(0,0,0));
    bufferPainter->setPen(pe);
    bufferPainter->setBrush(*color);
    
    bufferPainter->fillRect(colonne, (controller->get_map()->get_m_h()-1) - ligne, 1, 1, *(color));
}

// void DisplayQT::drawList( list< Coordinates* > * list_coordinates){
// 
//     for( const Coordinates* coord : *list_coordinates){
//         drawCell(coord->col, coord->row);
//     }
//     list_coordinates->clear();
// }

void DisplayQT::drawField()
{
    if (buffer->isNull())
        cerr<< "Impossible de dessiner, image vide"<< endl;
    bufferPainter->begin(buffer);
    const std::map<int, Case*>* cases= controller->get_map()->get_sommets();
    for(std::map<int,Case*>::const_iterator it = cases->begin(); it != cases->end() ; ++it){
        bool obstacle = (*it).second->isObstacle();
	std::string typeT = (*it).second->getTerrain().getType();

        // Cas d'une parcelle exploitable
        if(obstacle){
            setColor(Black);
            drawCell((*it).second->getX(), (*it).second->getY());
        }
        // Cas d'une parcelle non exploitable
        if(!obstacle){
            setColor(White);
            drawCell((*it).second->getX(), (*it).second->getY());
        }
        if(typeT == "Marais"){
	  setColor(Purple);
          drawCell((*it).second->getX(), (*it).second->getY());
	}
	if(typeT == "Foret"){
	  setColor(Green);
          drawCell((*it).second->getX(), (*it).second->getY());
	}
	if(typeT == "Riviere"){
	  setColor(LightBlue);
           drawCell((*it).second->getX(), (*it).second->getY());
	}
	if(typeT == "Plaine"){
	  setColor(LightYellow);
          drawCell((*it).second->getX(), (*it).second->getY());
	}
	if(typeT == "Colline"){
	  setColor(GreenKaki);
          drawCell((*it).second->getX(), (*it).second->getY());
	}
//         // Cas d'une route
//         else if(state == is_road){
//             setColor(Gray);
//             drawCell((*it).getX(), (*it).getY());
//         }
//         // Cas d'une entrée/sortie
//         else if(state == is_in_out){
//             setColor(Red);
//             drawCell((*it).getX(), (*it).getY());
//         }
//         else if (state == is_undefined){
//             setColor(Black);
//             cout << "Dessin d'une case non définie"<< endl;
//             drawCell((*it).getX(), (*it).getY());
//         }
    }
  
    bufferPainter->end();
}

// Test perf
#if PERF_REDRAW
int num_redraw= 0;
#endif

void DisplayQT::redraw()
{
    #if PERF_REDRAW
    ++num_redraw;
    cout << "test redraw firewidget"<< num_redraw<< endl;
    #endif

    if (!buffer->isNull()){
        delete(buffer);
    }
    buffer = new QImage(controller->get_map()->get_m_w(), controller->get_map()->get_m_h(), QImage::Format_ARGB32);

    drawField();
    /*drawChanged*/;
    update();
}

//@}

/// ##################
/// 	Events
/// ##################
//@{

void DisplayQT::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);
    QPainter paint(this);

    
    paint.scale(tailleCell, tailleCell);
    paint.drawImage(1, 0, *buffer);
    paint.scale(1.0/((float)tailleCell), 1.0/((float)tailleCell));
    for (unsigned x= 1; x <= (unsigned)controller->get_map()->get_m_w(); ++x) {
        float posX= x*tailleCell;
        paint.drawLine(posX, 0, posX, (controller->get_map()->get_m_h() +1)*tailleCell);
        ostringstream convert;
        convert<< (x-1);
        paint.drawText(QRect(posX +3, controller->get_map()->get_m_h() * tailleCell +2, tailleCell, tailleCell), QString::fromStdString(convert.str()));
    }

    for (unsigned y= 1; y <= (unsigned)controller->get_map()->get_m_h(); ++y) {
        float posY= y*tailleCell;
        paint.drawLine(0, posY, (controller->get_map()->get_m_w() +1) *tailleCell, posY);
        ostringstream convert;
        convert<< (controller->get_map()->get_m_h()-y);
        paint.drawText(QRect(2,posY -tailleCell +2, tailleCell, tailleCell), QString::fromStdString(convert.str()));
    }
}

void DisplayQT::resizeEvent(QResizeEvent* event)
{
	#if PERF_RESIZE
	cout << "test resizeEvent firewidget"<< endl;
	#endif
	
    int nbCol= controller->get_map()->get_m_w() +1;
    int nbRow= controller->get_map()->get_m_h()+1;
	tailleCell = min (event->size().width() / (float)nbCol , event->size().height() / (float)nbRow);

	redraw();
}

void DisplayQT::mousePressEvent(QMouseEvent* event)
{
	
	if (event->button()==Qt::LeftButton)
    {
      if(!algorun){
// 	controller->demande_chemin_algogen(0,90,90);
// 	controller->get_map()->A_star_GA()
	//algorun = true;
      }
    }
	else if (event->button()==Qt::MiddleButton)
    {}
	else if (event->button()==Qt::RightButton)
	{
// 		initRubber(event);
		
	}
	
	/*drawChanged*/;
	update();
}

// void DisplayQT::initRubber(QMouseEvent* event){
// 	origin = event->pos();
// 	
// 	if(!rubber)
// 		rubber = new QRubberBand(QRubberBand::Rectangle, this);
// 
// 	rubber->setGeometry(QRect(origin, QSize(0,0)));
// 	rubber->show();
// }

void DisplayQT::mouseMoveEvent(QMouseEvent* event)
{
//	int colonne= event->x()/tailleCell;
//	int ligne= event->y()/tailleCell;
	
	if (event->buttons().testFlag(Qt::LeftButton) )
    {}
	else if (event->buttons().testFlag(Qt::MiddleButton) )
    {}
	else if (event->buttons().testFlag(Qt::RightButton) ){
        if(rubber) {
			rubber->setGeometry(QRect(origin,event->pos()).normalized());
        }
	}
	
	//drawChanged();
	update();
}

void DisplayQT::mouseReleaseEvent(QMouseEvent* event)
{
    QWidget::mouseReleaseEvent(event);
	if(rubber){
		rubber->hide();
		// Sauvegarde des points du rubber pour parcours de la matrice
		depart.setX(rubber->x());
		depart.setY(rubber->y());
		/* Vérification du point d'origine du rubber
		 * Celui étant toujours le point le plus en haut à gauche, il faut simplement vérifier 
		 * qu'il n'est pas en dehors du cadre, auquel cas nous ramenons la (les) coordonnée(s) concernée(s)
		 * à 0.
		 */
		arrivee.setX(rubber->width()+depart.x());
		arrivee.setY(rubber->height()+depart.y());
		if(depart.x() < 0){
			depart.setX(0);
		}
		if(depart.y() < 0){
			depart.setY(0);
		}
		
		
		if(arrivee.x() > size().width() ){
			arrivee.setX(size().width());
		}
		if(arrivee.y() > size().height() ){
			arrivee.setY(size().height());
		}
		// Emission du signal pour récupérer l'action à effectuer par firescreen
//		emit releaseSignal(); // TODO signaux
	}
	
}

//@}
/// ############
///    Slots
/// ############
//@{

void DisplayQT::presidentReceived()
{
    controller->set_slot(true);
    if (buffer->isNull())
        cerr<< "Impossible de dessiner, image vide"<< endl;
    const std::vector<std::vector<int>>* vec_dep = controller->get_map()->get_changedPresident();
    srand(time(NULL));
    redraw();
     bufferPainter->begin(buffer);
    for(std::vector<std::vector<int>>::const_iterator it = vec_dep->begin(); it != vec_dep->end() ; ++it){
	 setColor(rand()%255,rand()%255,rand()%255);
	 std::vector<int> minion = (*it);
	for(std::vector<int>::const_iterator it2 = minion.begin(); it2 != minion.end();++it2){
	  if((*it2)>=0 && (*it2) < (controller->get_map()->get_m_h() * controller->get_map()->get_m_w())){
	  drawCell(controller->get_map()->get_Case((*it2))->getX(), controller->get_map()->get_Case((*it2))->getY());
      }
	}
      
    }
  
    bufferPainter->end();
    update();
    controller->set_slot(false);
    
}


//void Display::actionReceived(int x)
//{
//	// Transformation des QPoints depart et arrivée en coordonnée cellulaire
//	int xDep = depart.x() / tailleCell;
//	int yDep = depart.y() / tailleCell;
	
//    unsigned xArr = arrivee.x() / tailleCell;
	
//    if (xArr > Map::controller->get_map()->get_m_w()) xArr= Map::controller->get_map()->get_m_w();
	
//    unsigned yArr = arrivee.y() / tailleCell;
	
//    if (yArr> Map::controller->get_map()->get_m_h())	yArr= Map::controller->get_map()->get_m_h();

	
//	#if DEBUG_RETARD
//	cout << "Coordonnée en cellule du départ : " << xDep << ";" << yDep << endl;
//	cout << "Coordonnée en cellule de l'arrivée : " << xArr << ";" << yArr << endl;
//	#endif
	
//	// Appel à une fonction de forêt qui parcours la zone et effectue l'action
	
	
//	if(x == CUT){
//	}else if( x == DELAY){
//	}else cerr<< "mauvais index d'action clic droit"<< endl;
	
//	drawChanged();
//	update();
//}

//@}

void DisplayQT::initMap()
{   
}
