/*************************************************************************************
*	irrlamb - https://github.com/jazztickets/irrlamb
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
#include <all.h>
#include <objects/player.h>
#include <engine/constants.h>
#include <engine/globals.h>
#include <engine/physics.h>
#include <engine/input.h>
#include <engine/audio.h>
#include <engine/actions.h>
#include <engine/graphics.h>
#include <objects/sphere.h>
#include <objects/constraint.h>
#include <objects/template.h>
#include <engine/namespace.h>

// Constructor
_Player::_Player(const SpawnStruct &Object)
:	_Object(),
	Sound(NULL),
	Camera(NULL),
	Light(NULL),
	JumpTimer(0.0f),
	TorqueFactor(4.0f) {

	// Graphics
	Node = irrScene->addSphereSceneNode(Object.Template->Radius, 24);
	Node->setMaterialTexture(0, irrDriver->getTexture("textures/player_outer0.png"));
	Node->setMaterialType(EMT_TRANSPARENT_ADD_COLOR);
	Node->setMaterialFlag(EMF_LIGHTING, false);
	Node->setMaterialType(EMT_ONETEXTURE_BLEND);
	Node->getMaterial(0).MaterialTypeParam = pack_textureBlendFunc(EBF_ONE, EBF_ONE);

	// Emit Light
	if(Object.Template->EmitLight) {
		Light = irrScene->addLightSceneNode(0, vector3df(Object.Position[0], Object.Position[1], Object.Position[2]), video::SColorf(1.0f, 1.0f, 1.0f), 15.0f);
		Light->getLightData().Attenuation.set(0.5f, 0.05f, 0.05f);
		Light->getLightData().DiffuseColor.set(1.0f, 0.75f, 0.75f, 1.0f);
	}

	// Add glow
	ISceneNode *InnerNode;
	InnerNode = irrScene->addBillboardSceneNode(Node, dimension2df(1.5f, 1.5f));
	InnerNode->setMaterialFlag(EMF_LIGHTING, false);
	InnerNode->setMaterialFlag(EMF_ZBUFFER, false);
	InnerNode->setMaterialTexture(0, irrDriver->getTexture("textures/player_glow0.png"));
	InnerNode->setMaterialType(EMT_ONETEXTURE_BLEND);
	InnerNode->getMaterial(0).MaterialTypeParam = pack_textureBlendFunc(EBF_ONE, EBF_ONE);

	if(Physics.IsEnabled()) {

		// Create shape
		btSphereShape *Shape = new btSphereShape(Object.Template->Radius);

		// Set up physics
		CreateRigidBody(Object, Shape);
		RigidBody->setSleepingThresholds(0.1f, 0.1f);

		// Audio
		Sound = new _AudioSource(Audio.GetBuffer("player.ogg"), true, 0.0, 0.50f);
		Sound->SetPosition(Object.Position[0], Object.Position[1], Object.Position[2]);
		Sound->Play();
	}

	SetProperties(Object);
	Hookable = false;
	if(CollisionCallback == "")
		CollisionCallback = "OnHitPlayer";
}

// Destructor
_Player::~_Player() {
	
	if(Light)
		Light->remove();

	delete Sound;
}

// Update the player
void _Player::Update(float FrameTime) {
	_Object::Update(FrameTime);
	
	// Update audio
	const btVector3 &Position = GetPosition();
	Sound->SetPosition(Position[0], Position[1], Position[2]);

	// Update light
	if(Light) {
		Light->setPosition(vector3df(Position[0], Position[1], Position[2]));
	}

	// Get pitch for player idle sound
	float Pitch = GetAngularVelocity().length();
	if(Pitch < 3.0f)
		Pitch = 3.0f;
	else if(Pitch > 22.0f)
		Pitch = 22.0f;
	Pitch -= 3.0f;
	Pitch /= 30.0f;
	Pitch += 1.5f;
	Sound->SetPitch(Pitch);

	// Update jump timer
	if(JumpTimer > 0.0f) {
		JumpTimer -= FrameTime;
		if(JumpTimer < 0.0f)
			JumpTimer = 0.0f;

		if(TouchingGround) {
			RigidBody->activate();
			RigidBody->applyCentralImpulse(btVector3(0.0f, JUMP_POWER, 0.0f));
			JumpTimer = 0.0f;
		}
	}	
}

// Processes input from the keyboard
void _Player::HandleInput() {
	vector3df Push(0.0f, 0.0f, 0.0f);
	
	// Get input direction
	Push.X += -Actions.GetState(_Actions::MOVE_LEFT);
	Push.X += Actions.GetState(_Actions::MOVE_RIGHT);
	Push.Z += Actions.GetState(_Actions::MOVE_FORWARD);
	Push.Z += -Actions.GetState(_Actions::MOVE_BACK);

	if(Push.getLength() > 1.0f) {
		Push.normalize();
	}

	// Push the player
	if(!Push.equals(vector3df())) {

		// Get push direction relative to camera
		matrix4 DirectionTransform;
		DirectionTransform.makeIdentity();
		DirectionTransform.setRotationDegrees(vector3df(0.0f, Camera->GetYaw(), 0.0f)); 
		DirectionTransform.transformVect(Push);

		// Apply torque
		vector3df RotationAxis = Push.crossProduct(vector3df(0.0f, -1.0f, 0.0f)) * TorqueFactor;
		RigidBody->activate();
		RigidBody->applyTorque(btVector3(RotationAxis.X, RotationAxis.Y, RotationAxis.Z));
	}
}

// Request a jump
void _Player::Jump() {
	JumpTimer = JUMP_WINDOW;
}

// Update the graphic node position
void _Player::SetPositionFromReplay(const irr::core::vector3df &Position) {
	if(Node)
		Node->setPosition(Position);

	if(Light)
		Light->setPosition(Position);
}
