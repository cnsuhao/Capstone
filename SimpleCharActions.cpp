//
//  SimpleCharActions.cpp
//  C4
//
//  Created by blayne tilban on 2014-11-26.
//
//
#include "SimpleCharActions.h"

#include "C4World.h"
#include "C4Input.h"
#include "C4Application.h"
#include "C4Interface.h"
#include "C4Character.h"
#include "C4Engine.h"
#include "C4Configuration.h"
#include "C4Logo.h"
#include "C4Markings.h"
#include "C4Adjusters.h"

#include "SimpleCharMultiplayer.h"
#include "SimpleCharSoldier.h"
#include "SimpleChar.h"
#include "SimpleCharControllers.h"

#include "SimpleCharProjectile.h"

using namespace C4;

MovementAction::MovementAction(unsigned_int32 type, unsigned_int32 flag) : Action(type)
{
	// Each instance of the MovementAction class represents a movement
	// in a single direction, as indicated by the flag parameter.
	// All of the MovementAction instances are constructed in the
	// Game class constructor.

	movementFlag = flag;
}

MovementAction::~MovementAction()
{
}

void MovementAction::Begin(void)
{
	const Player *player = TheMessageMgr->GetLocalPlayer();
	if (player)
	{
		const SoldierController *controller = static_cast<const GamePlayer *>(player)->GetController();
		if (controller)
		{
			ClientMovementMessage message(kMessageMovementBegin, movementFlag);
			TheMessageMgr->SendMessage(kPlayerServer, message);
			return;
		}
	}
}

void MovementAction::End(void)
{
	const Player *player = TheMessageMgr->GetLocalPlayer();
	if (player)
	{
		const SoldierController *controller = static_cast<const GamePlayer *>(player)->GetController();
		if (controller)
		{
			ClientMovementMessage message(kMessageMovementEnd, movementFlag);
			TheMessageMgr->SendMessage(kPlayerServer, message);
			return;
		}
	}
}

FireAction::FireAction() : Action(kActionFire)
{
}

FireAction::~FireAction()
{
}

void FireAction::Begin(void)
{
	const Player *player = TheMessageMgr->GetLocalPlayer();
	if (player)
	{
		const SoldierController *controller = static_cast<const GamePlayer *>(player)->GetController();
		if (controller)
		{

			TheMessageMgr->SendMessage(kPlayerServer, ClientFiringMessage(controller->GetModelAzimuth()));
			return;
			/*
			GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());

			Controller *controller1;
			Model *model1 = nullptr;
			Point3D zonePosition;
			float speed = 20.0F;// increase or decrease to change the speed
			float azimuth = controller->GetModelAzimuth();

			Vector3D direction = *new Vector3D(cos(azimuth), sin(azimuth), 0.0f);
			Point3D startPos = controller->GetTargetNode()->GetWorldPosition() + Point3D(0.0F,0.0F,1.0F);

			direction = direction*speed;
			controller1 = new BallController(direction);
			model1 = Model::Get(kModelBall);
			if (model1)
			{
			{
			model1->SetController(controller1);

			zonePosition = startPos;
			model1->SetNodePosition(zonePosition);

			OmniSource *source = new OmniSource("model/Ball", 40.0F);
			source->SetNodePosition(zonePosition);

			world->AddNewNode(source);
			world->AddNewNode(model1);
			model1->Update();

			}

			}


			return;*/
		}
	}
}

UseAction::UseAction() : Action(kActionUse)
{
}

UseAction::~UseAction()
{
}

void UseAction::Begin(void)
{
	// The player has pressed the fire/use button. If we are currently interacting with
	// a node in the scene and that node has a controller, then we send an activate event
	// to that controller to let it know that the player is doing something with it.

	SoldierController *controller = TheGame->GetSoldierController();
	if (controller)
	{
		const SoldierInteractor *interactor = controller->GetSoldierInteractor();
		const Node *interactionNode = interactor->GetInteractionNode();
		if (interactionNode)
		{
			Controller *interactionController = interactionNode->GetController();
			if (interactionController)
			{
				interactionController->HandleInteractionEvent(kInteractionEventActivate, &interactor->GetInteractionPosition(), controller->GetTargetNode());
			}
		}
	}
}

void UseAction::End(void)
{
	// The player has released the fire/use button. Let the node with which we are interacting
	// know that we are done with it by sending its controller a deactivate event.

	SoldierController *controller = TheGame->GetSoldierController();
	if (controller)
	{
		const SoldierInteractor *interactor = controller->GetSoldierInteractor();
		const Node *interactionNode = interactor->GetInteractionNode();
		if (interactionNode)
		{
			Controller *interactionController = interactionNode->GetController();
			if (interactionController)
			{
				interactionController->HandleInteractionEvent(kInteractionEventDeactivate, &interactor->GetInteractionPosition(), controller->GetTargetNode());
			}
		}
	}
}