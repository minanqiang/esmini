/*
 * esmini - Environment Simulator Minimalistic
 * https://github.com/esmini/esmini
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (c) partners of Simulation Scenarios
 * https://sites.google.com/view/simulationscenarios
 */

/*
 * This controller simulates a bad or dizzy driver by manipulating
 * the speed and lateral offset in a random way.
 * The purpose is purely to demonstrate how to implement a controller.
 */

#include "ControllerInteractive.hpp"
#include "CommonMini.hpp"
#include "Entities.hpp"
#include "ScenarioGateway.hpp"

#include <random>

using namespace scenarioengine;

Controller* scenarioengine::InstantiateControllerInteractive(void* args)
{
	Controller::InitArgs* initArgs = (Controller::InitArgs*)args;

	return new ControllerInteractive(initArgs);
}

ControllerInteractive::ControllerInteractive(InitArgs* args) : steering_rate_(4.0), Controller(args)
{
	if (args && args->properties && args->properties->ValueExists("steeringRate"))
	{
		steering_rate_ = strtod(args->properties->GetValueStr("steeringRate"));
	}
}

void ControllerInteractive::Init()
{

	Controller::Init();
}

void ControllerInteractive::Step(double timeStep)
{
	double speed_limit = object_->pos_.GetSpeedLimit();

	if (speed_limit < SMALL_NUMBER)
	{
		// no speed limit defined, set something with regards to number of lanes
		if (object_->pos_.GetRoadById(object_->pos_.GetTrackId())->GetNumberOfDrivingLanesSide(
			object_->pos_.GetS(), SIGN(object_->pos_.GetLaneId())) > 1)
		{
			speed_limit = 110 / 3.6;
		}
		else
		{
			speed_limit = 60 / 3.6;
		}
	}
	vehicle_.SetMaxSpeed(MIN(speed_limit, object_->GetMaxSpeed()));

	if (!(IsActiveOnDomains(ControlDomains::DOMAIN_LONG)))
	{
		// Fetch speed from Default Controller
		vehicle_.speed_ = object_->GetSpeed();
	}

	// Update vehicle motion
	vehicle_.SetThrottleDisabled(!IsActiveOnDomains(ControlDomains::DOMAIN_LONG));
	vehicle_.SetSteeringDisabled(!IsActiveOnDomains(ControlDomains::DOMAIN_LAT));
	vehicle_.DrivingControlBinary(timeStep, accelerate, steer);

	if (domain_ == ControlDomains::DOMAIN_LONG)
	{
		// Only longitudinal control, move along road
		double steplen = vehicle_.speed_* timeStep;

		object_->MoveAlongS(steplen);

		// Fetch updated position
		vehicle_.posX_ = object_->pos_.GetX();
		vehicle_.posY_ = object_->pos_.GetY();
		vehicle_.heading_ = object_->pos_.GetH();
	}

	gateway_->updateObjectWorldPosXYH(object_->id_, 0.0, vehicle_.posX_, vehicle_.posY_, vehicle_.heading_);

	// Fetch Z and Pitch from OpenDRIVE position
	vehicle_.posZ_ = object_->pos_.GetZRoad();
	vehicle_.pitch_ = object_->pos_.GetPRoad();

	if (IsActiveOnDomains(ControlDomains::DOMAIN_LONG))
	{
		gateway_->updateObjectSpeed(object_->id_, 0.0, vehicle_.speed_);
	}

	if (IsActiveOnDomains(ControlDomains::DOMAIN_LAT))
	{
		gateway_->updateObjectWheelAngle(object_->id_, 0.0, vehicle_.wheelAngle_);
	}

	Controller::Step(timeStep);
}

void ControllerInteractive::Activate(ControlDomains domainMask)
{
	if (object_)
	{
		vehicle_.Reset();
		vehicle_.SetPos(object_->pos_.GetX(), object_->pos_.GetY(), object_->pos_.GetZ(), object_->pos_.GetH());
		vehicle_.SetLength(object_->boundingbox_.dimensions_.length_);
		vehicle_.speed_ = object_->GetSpeed();
		vehicle_.SetMaxAcc(object_->GetMaxAcceleration());
		vehicle_.SetMaxDec(object_->GetMaxDeceleration());
		vehicle_.SetSteeringRate(steering_rate_);
	}

	steer = vehicle::STEERING_NONE;
	accelerate = vehicle::THROTTLE_NONE;

	object_->pos_.SetAlignModeZ(roadmanager::Position::ALIGN_MODE::ALIGN_HARD);
	object_->pos_.SetAlignModeP(roadmanager::Position::ALIGN_MODE::ALIGN_HARD);

	object_->SetJunctionSelectorStrategy(roadmanager::Junction::JunctionStrategyType::SELECTOR_ANGLE);
	object_->SetJunctionSelectorAngle(0.0);

	Controller::Activate(domainMask);
}

void ControllerInteractive::ReportKeyEvent(int key, bool down)
{
	if (key == Controller::KEY_Left)
	{
		if (down)
		{
			steer = vehicle::STEERING_LEFT;
		}
		else
		{
			steer = vehicle::STEERING_NONE;
		}
	}
	else if(key == Controller::KEY_Right)
	{
		if (down)
		{
			steer = vehicle::STEERING_RIGHT;
		}
		else
		{
			steer = vehicle::STEERING_NONE;
		}
	}
	else if (key == Controller::KEY_Up)
	{
		if (down)
		{
			accelerate = vehicle::THROTTLE_ACCELERATE;
		}
		else
		{
			accelerate = vehicle::THROTTLE_NONE;
		}
	}
	else if (key == Controller::KEY_Down)
	{
		if (down)
		{
			accelerate = vehicle::THROTTLE_BRAKE;
		}
		else
		{
			accelerate = vehicle::THROTTLE_NONE;
		}
	}
}
