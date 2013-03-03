/*************************************************************************************
*	irrlamb - http://irrlamb.googlecode.com
*	Copyright (C) 2013  Alan Witkowski
*
*	This program is free software: you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation, either version 3 of the License, or
*	(at your option) any later version.
*
*	This program is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*	GNU General Public License for more details.
*
*	You should have received a copy of the GNU General Public License
*	along with this program.  If not, see <http://www.gnu.org/licenses/>.
**************************************************************************************/
#include "springjoint.h"
#include "../engine/globals.h"
#include "../engine/physics.h"
#include "object.h"
#include "template.h"

// Constructor
SpringJointClass::SpringJointClass(const SpawnStruct &Object)
:	BodyA(NULL),
	BodyB(NULL) {
/*
	// Attributes
	if(Physics::Instance().IsEnabled()) {
		BodyA = TObject.BodyA->GetBody();
		BodyB = TObject.BodyB->GetBody();
	}

	SpringConstant = TObject.SpringConstant;
	Distance = TObject.SpringDistance;
	Damping = TObject.LinearDamping;

	SetProperties(TObject);
	*/
}

// Destructor
SpringJointClass::~SpringJointClass() {

}

// Updates the spring system
void SpringJointClass::Update(float FrameTime) {
/*
	if(Physics::Instance().IsEnabled()) {

		// Get direction and length
		btVector3 Direction = BodyA->getWorldTransform().getOrigin() - BodyB->getWorldTransform().getOrigin();
		btScalar DirectionLength = Direction.length();

		// Get unit direction
		Direction /= DirectionLength;

		// Get force
		btVector3 Force = Direction * (-SpringConstant * (DirectionLength - Distance));

		// Apply damping
		if(Damping > 0.0f) {

			// Get relative velocity
			btVector3 RelativeVelocity;
			if(BodyA->getLinearVelocity().dot(BodyB->getLinearVelocity()))
				RelativeVelocity = BodyA->getLinearVelocity() - BodyB->getLinearVelocity();
			else
				RelativeVelocity = BodyB->getLinearVelocity() - BodyA->getLinearVelocity();
			
			Force += -RelativeVelocity * Damping;
		}

		// Apply force to the first object
		BodyA->applyCentralForce(Force);
		
		// Apply force to the second object
		Force *= -1;
		BodyB->applyCentralForce(Force);
	}
	*/
}
