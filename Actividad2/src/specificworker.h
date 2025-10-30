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

/**
	\brief
	@author authorname
*/



#ifndef SPECIFICWORKER_H
#define SPECIFICWORKER_H


// If you want to reduce the period automatically due to lack of use, you must uncomment the following line
//#define HIBERNATION_ENABLED

#include <genericworker.h>
#include <abstract_graphic_viewer/abstract_graphic_viewer.h>
#include "hungarian.h"
#include "room_detector.h"
#include <expected>


/**
 * \brief Class SpecificWorker implements the core functionality of the component.
 */
class SpecificWorker : public GenericWorker
{
Q_OBJECT
public:



    /**
     * \brief Constructor for SpecificWorker.
     * \param configLoader Configuration loader for the component.
     * \param tprx Tuple of proxies required for the component.
     * \param startup_check Indicates whether to perform startup checks.
     */
	SpecificWorker(const ConfigLoader& configLoader, TuplePrx tprx, bool startup_check);




	/**
     * \brief Destructor for SpecificWorker.
     */
	~SpecificWorker();


public slots:
	/**
	 * \brief Initializes the worker one time.
	 */
	void initialize();

/**
	 * \brief Main compute loop of the worker.
	 */
	void compute();

	/**
	 * \brief Handles the emergency state loop.
	 */
	void emergency();

	/**
	 * \brief Restores the component from an emergency state.
	 */
	void restore();

    /**
     * \brief Performs startup checks for the component.
     * \return An integer representing the result of the checks.
     */
	int startup_check();

private:

	enum State { IDLE, FORWARD, TURN, FOLLOW_WALL, SPIRAL};
	State state = SPIRAL;

	bool fol_wall = false;
	bool derecha = false;

	float spir_rot = 0.6;
	float spir_speed = 1000.0;

	struct NominalRoom
	{
		float width; //  mm
		float length;
		Corners corners;

		QRectF GRID_MAX_DIM;
		float ROBOT_WIDTH;
		float ROBOT_LENGTH;

		explicit NominalRoom(const float width_=10000.f, const float length_=5000.f, Corners  corners_ = {}) : width(width_), length(length_), corners(std::move(corners_)){};
		Corners transform_corners_to(const Eigen::Affine2d &transform) const  // for room to robot pass the inverse of robot_pose
		{
			Corners transformed_corners;
			for(const auto &[p, _, __] : corners)
			{
				auto ep = Eigen::Vector2d{p.x(), p.y()};
				Eigen::Vector2d tp = transform * ep;
				transformed_corners.emplace_back(QPointF{static_cast<float>(tp.x()), static_cast<float>(tp.y())}, 0.f, 0.f);
			}
			return transformed_corners;
		}
	};
	NominalRoom room{10000.f, 5000.f,
				{{QPointF{-5000.f, -2500.f}, 0.f, 0.f},
					   {QPointF{5000.f, -2500.f}, 0.f, 0.f},
					   {QPointF{5000.f, 2500.f}, 0.f, 0.f},
					   {QPointF{-5000.f, 2500.f}, 0.f, 0.f}}};


	QRectF dimensions;
	AbstractGraphicViewer *viewer;
	const int ROBOT_LENGTH = 400;
	QGraphicsPolygonItem *robot_polygon;
	QGraphicsPolygonItem *robot_room_draw;

	AbstractGraphicViewer *viewer_room;
	Eigen::Affine2d robot_pose;
	rc::Room_Detector room_detector;
	rc::Hungarian hungarian;

	RoboCompLidar3D::TPoints get_filtered_lidar_data();
	void draw_lidar(const auto &points, QGraphicsScene* scene);

	RoboCompLidar3D::TPoints filter_min_distance(RoboCompLidar3D::TPoints points);

	RoboCompLidar3D::TPoints filter_ahead(RoboCompLidar3D::TPoints points,int lado);//0 adelante, 1 drcha, 2 izq

	std::tuple<State, float, float> state_machine(RoboCompLidar3D::TPoints puntos, State state);

	std::tuple<State, float, float> fwd(RoboCompLidar3D::TPoints puntos);
	std::tuple<State, float, float> turn(RoboCompLidar3D::TPoints puntos);
	std::tuple<State, float, float> wall(RoboCompLidar3D::TPoints puntos);
	std::tuple<State, float, float> spiral(RoboCompLidar3D::TPoints puntos);

	void set_speeds(std::tuple<State, float, float> general_state);



	/**
     * \brief Flag indicating whether startup checks are enabled.
     */
	bool startup_check_flag;

signals:
	//void customSignal();
};

#endif
