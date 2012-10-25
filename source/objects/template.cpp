/*************************************************************************************
*	irrlamb - http://irrlamb.googlecode.com
*	Copyright (C) 2011  Alan Witkowski
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
#include "template.h"
#include "object.h"
#include "../engine/physics.h"

TemplateStruct::TemplateStruct() {

	// Generic properties
	TemplateID = -1;
	Name = "";
	Type = ObjectClass::NONE;
	Lifetime = 0.0f;

	// Collisions
	CollisionCallback = "";
    CollisionGroup = PhysicsClass::FILTER_RIGIDBODY | PhysicsClass::FILTER_CAMERA;
    CollisionMask = PhysicsClass::FILTER_BASICBODIES | PhysicsClass::FILTER_ZONE;

	// Physical properties
	CollisionFile = "";
	Shape.setValue(1.0f, 1.0f, 1.0f);
	Radius = 0.5f;
	Mass = 1.0f;
	Friction = 1.0f;
	Restitution = 0.2f;
	LinearDamping = 0.1f;
	AngularDamping = 0.3f;

	// Constraints
	for(int i = 0; i < 4; i++)
		ConstraintData[i].setValue(0.0f, 0.0f, 0.0f);

	// Graphics
	Scale = 1.0f;
	Mesh = "";
	Fog = false;
	CustomMaterial = -1;
	EmitLight = false;

	// Zones
	Active = true;
}

SpawnStruct::SpawnStruct() {
	Name = "";
	Position.setValue(0.0f, 0.0f, 0.0f);
	Rotation.setValue(0.0f, 0.0f, 0.0f);
	Template = NULL;
}

ConstraintStruct::ConstraintStruct() {
	Name = "";
	BodyA = NULL;
	BodyB = NULL;
	Template = NULL;
}
