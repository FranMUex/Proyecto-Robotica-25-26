/*
 *    Copyright (C) 2025 by YOUR NAME HERE
 *
 *    This file is part of RoboComp
 *
 *    RoboComp is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    RoboComp is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with RoboComp.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "specificworker.h"
#include <vector>
#include <cppitertools/groupby.hpp>
#include <cppitertools/range.hpp>
#include <time.h>

State state = SPIRAL;

bool fol_wall = false;
bool derecha = false;

float spir_rot = 0.6;
float spir_speed = 1000.0;

SpecificWorker::SpecificWorker(const ConfigLoader& configLoader, TuplePrx tprx, bool startup_check) : GenericWorker(configLoader, tprx)
{
	this->startup_check_flag = startup_check;
	if(this->startup_check_flag)
	{
		this->startup_check();
	}
	else
	{
		#ifdef HIBERNATION_ENABLED
			hibernationChecker.start(500);
		#endif
		
		// Example statemachine:
		/***
		//Your definition for the statesmachine (if you dont want use a execute function, use nullptr)
		states["CustomState"] = std::make_unique<GRAFCETStep>("CustomState", period, 
															std::bind(&SpecificWorker::customLoop, this),  // Cyclic function
															std::bind(&SpecificWorker::customEnter, this), // On-enter function
															std::bind(&SpecificWorker::customExit, this)); // On-exit function

		//Add your definition of transitions (addTransition(originOfSignal, signal, dstState))
		states["CustomState"]->addTransition(states["CustomState"].get(), SIGNAL(entered()), states["OtherState"].get());
		states["Compute"]->addTransition(this, SIGNAL(customSignal()), states["CustomState"].get()); //Define your signal in the .h file under the "Signals" section.

		//Add your custom state
		statemachine.addState(states["CustomState"].get());
		***/

		statemachine.setChildMode(QState::ExclusiveStates);
		statemachine.start();

		auto error = statemachine.errorString();
		if (error.length() > 0){
			qWarning() << error;
			throw error;
		}
	}
}

SpecificWorker::~SpecificWorker()
{
	std::cout << "Destroying SpecificWorker" << std::endl;
}


void SpecificWorker::initialize()
{

    //initializeCODE

    /////////GET PARAMS, OPEND DEVICES....////////
    //int period = configLoader.get<int>("Period.Compute") //NOTE: If you want get period of compute use getPeriod("compute")
    //std::string device = configLoader.get<std::string>("Device.name")

	std::cout << "initialize worker" << std::endl;

	this->dimensions = QRectF(-6000, -3000, 12000, 6000);
	viewer = new AbstractGraphicViewer(this->frame, this->dimensions);
	this->resize(900,450);
	viewer->show();
	const auto rob = viewer->add_robot(ROBOT_LENGTH, ROBOT_LENGTH, 0, 190, QColor("Blue"));
	robot_polygon = std::get<0>(rob);
}


void SpecificWorker::compute()
{
	RoboCompLidar3D::TPoints filter_data;
	try
	{
		const auto data = lidar3d_proxy->getLidarDataWithThreshold2d("helios", 12000, 1);
		if (data.points.empty()) {qWarning() << "No points received"; return;}

		filter_data = filter_min_distance(data.points);

		draw_lidar(filter_data, &viewer->scene);
	}
	catch(const Ice::Exception &e) {std::cout << e << " Conexión con Laser\n";}
	std::tuple<State, float, float> result;
	switch (state)
{
		case IDLE:
			qInfo() << "IDLE";
			result = fwd(filter_data);
			break;
		case FORWARD:
			qInfo() << "FORWARD";
			result = fwd(filter_data);
			break;
		case TURN:
			qInfo() << "TURN";
			result = turn(filter_data);
			break;
		case FOLLOW_WALL:
			qInfo() << "FOLLOW_WALL";
			result = wall(filter_data);
			break;
		case SPIRAL:
			qInfo() << "SPIRAL";
			result = spiral(filter_data);
			break;

	}
	state = std::get<State>(result);


	try {
		omnirobot_proxy->setSpeedBase(0,std::get<1>(result),std::get<2>(result));
	}
	catch(const Ice::Exception &e) {
		std::cout << e << " Conexión con Laser\n";
	}

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::tuple<State, float, float> SpecificWorker::fwd(RoboCompLidar3D::TPoints puntos)
{
	auto pC = filter_ahead(puntos, 0);
	if(pC.empty())
		return {TURN, 0.0, 1.0};
	auto min_C = std::min_element(pC.begin(), pC.end(),
			[](const auto& p1, const auto& p2) { return p1.r < p2.r; });
	auto min = std::min_element(puntos.begin(), puntos.end(),
			[](const auto& p1, const auto& p2) { return p1.r < p2.r; });

	if (min_C->r<650)
	{
		derecha = min_C->phi >= 0 || (min->r < 650 && min->phi >= 0);

		return {TURN, 0.0, 0.0};
	}


	return{FORWARD, 1000.0, 0.0};
}

std::tuple<State, float, float> SpecificWorker::turn(RoboCompLidar3D::TPoints puntos) //NO GIRA IZQ
{
	auto pC = filter_ahead(puntos, 0);
	if(pC.empty())
		return {TURN, 0.0, 1.0};
	auto min_C = std::min_element(pC.begin(), pC.end(),
			[](const auto& p1, const auto& p2) { return p1.r < p2.r; });

	if (min_C->r<650)
	{
		if (derecha)
			return{TURN, 0.0, -1.0};
		return {TURN, 0.0, 1.0};
	}

	srand(time(NULL));
	int rand_num = rand() % 3;

	switch (rand_num)
	{
	case 0:
		return {SPIRAL, 0.0, 0.0};
	case 1:
		return {FORWARD, 0.0, 0.0};
	case 2:
		return {FOLLOW_WALL, 0.0, 0.0};

	}

}

std::tuple<State, float, float> SpecificWorker::wall(RoboCompLidar3D::TPoints puntos)
{
	auto pC = filter_ahead(puntos, 0);
	auto min_C = std::min_element(pC.begin(), pC.end(),
			[](const auto& p1, const auto& p2) { return p1.r < p2.r; });
	auto min = std::min_element(puntos.begin(), puntos.end(),
			[](const auto& p1, const auto& p2) { return p1.r < p2.r; });

	if (min->r > 650 || min_C->r > 700)
	{
			return {FORWARD, 0.0, 0.0};
	}

	if (derecha)
		return {FOLLOW_WALL, 0.0, -0.5};
	return {FOLLOW_WALL, 0.0, 0.5};
}

std::tuple<State, float, float> SpecificWorker::spiral(RoboCompLidar3D::TPoints puntos)
{
	auto pC = filter_ahead(puntos, 0);
	if(pC.empty())
		return {TURN, 0.0, 1.0};

	auto min_C = std::min_element(pC.begin(), pC.end(),
			[](const auto& p1, const auto& p2) { return p1.r < p2.r; });

	int sign = derecha ? -1 : 1;
	qInfo() << "Velocidad rotacion: " << spir_rot;

	if (min_C->r < 650)
	{
		spir_rot = 1.0;
		return {FORWARD, 0.0, 0.0};
	}

	if (spir_rot < 0.005)
	{
		spir_rot = 1.0;
		derecha = !derecha;
		return {SPIRAL, 0.0, 0.0};
	}
	spir_rot -= 0.007;

	return {SPIRAL, spir_speed, spir_rot * sign};
}


RoboCompLidar3D::TPoints SpecificWorker::filter_ahead(RoboCompLidar3D::TPoints points,int n)
{
	RoboCompLidar3D::TPoints puntos;
	float inicio,fin=0;

	switch(n)
	{
	case 0:
		inicio=std::numbers::pi/4; //IZQUIERDA
		fin=-std::numbers::pi/4; //DERECHA
		break;
	case 1:
		inicio=-std::numbers::pi/4;
		fin=-std::numbers::pi/2;
		break;
	case 2:
		inicio=std::numbers::pi/2;
		fin=std::numbers::pi/4;
		break;
	default:
		inicio=std::numbers::pi/2;
		fin=-std::numbers::pi/2;
	}
	for (auto i = 0; i < points.size(); ++i)
	{
		if (points[i].phi < inicio && points[i].phi > fin)
		{
			puntos.push_back(points[i]);
		}
	}
	return puntos;
}

RoboCompLidar3D::TPoints SpecificWorker::filter_min_distance(RoboCompLidar3D::TPoints points)
{
	RoboCompLidar3D::TPoints salida;
	// Agrupar por phi y obtener el mínimo de r por grupo en una línea, usando push_back para almacenar en el vector
	for (auto&& group : iter::groupby(points, [](const auto& p)
	{
		float factor = std::pow(10, 2);  // Potencia de 10 para mover el punto decimal
		return std::round(p.phi * factor) / factor;  // Redondear y devolver con la cantidad deseada de decimales
	})) {
		auto min_r = std::min_element(group.second.begin(), group.second.end(),
			[](const auto& p1, const auto& p2) { return p1.r < p2.r; });
		salida.emplace_back(*min_r);
	}

	return salida;
}
void SpecificWorker::draw_lidar(const auto &points, QGraphicsScene* scene)
{
	static std::vector<QGraphicsItem*> draw_points;
	for (const auto &p : draw_points)
	{
		scene->removeItem(p);
		delete p;
	}
	draw_points.clear();

	const QColor color("LightGreen");
	const QPen pen(color, 10);
	//const QBrush brush(color, Qt::SolidPattern);
	for (const auto &p : points)
	{
		const auto dp = scene->addRect(-25, -25, 50, 50, pen);
		dp->setPos(p.x, p.y);
		draw_points.push_back(dp);   // add to the list of points to be deleted next time
	}
}

void SpecificWorker::emergency()
{
    std::cout << "Emergency worker" << std::endl;
    //emergencyCODE
    //
    //if (SUCCESSFUL) //The componet is safe for continue
    //  emmit goToRestore()
}



//Execute one when exiting to emergencyState
void SpecificWorker::restore()
{
    std::cout << "Restore worker" << std::endl;
    //restoreCODE
    //Restore emergency component

}


int SpecificWorker::startup_check()
{
	std::cout << "Startup check" << std::endl;
	QTimer::singleShot(200, QCoreApplication::instance(), SLOT(quit()));
	return 0;
}



/**************************************/
// From the RoboCompDifferentialRobot you can call this methods:
// RoboCompDifferentialRobot::void this->differentialrobot_proxy->correctOdometer(int x, int z, float alpha)
// RoboCompDifferentialRobot::void this->differentialrobot_proxy->getBasePose(int x, int z, float alpha)
// RoboCompDifferentialRobot::void this->differentialrobot_proxy->getBaseState(RoboCompGenericBase::TBaseState state)
// RoboCompDifferentialRobot::void this->differentialrobot_proxy->resetOdometer()
// RoboCompDifferentialRobot::void this->differentialrobot_proxy->setOdometer(RoboCompGenericBase::TBaseState state)
// RoboCompDifferentialRobot::void this->differentialrobot_proxy->setOdometerPose(int x, int z, float alpha)
// RoboCompDifferentialRobot::void this->differentialrobot_proxy->setSpeedBase(float adv, float rot)
// RoboCompDifferentialRobot::void this->differentialrobot_proxy->stopBase()

/**************************************/
// From the RoboCompDifferentialRobot you can use this types:
// RoboCompDifferentialRobot::TMechParams

/**************************************/
// From the RoboCompLaser you can call this methods:
// RoboCompLaser::TLaserData this->laser_proxy->getLaserAndBStateData(RoboCompGenericBase::TBaseState bState)
// RoboCompLaser::LaserConfData this->laser_proxy->getLaserConfData()
// RoboCompLaser::TLaserData this->laser_proxy->getLaserData()

/**************************************/
// From the RoboCompLaser you can use this types:
// RoboCompLaser::LaserConfData
// RoboCompLaser::TData

