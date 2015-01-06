//
//  SimpleCharSoldier.cpp
//  C4
//
//  Created by blayne tilban on 2014-11-25.
//
//

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

using namespace C4;

namespace
{
    Storage<Soldier> soldierStorage;
}


Soldier::Soldier() :
soldierControllerRegistration(kControllerSoldier, nullptr),
soldierModelRegistration(kModelSoldier, nullptr, "soldier/Soldier", kModelPrecache | kModelPrivate)
{
}

Soldier::~Soldier()
{
}

void Soldier::Construct(void)
{
    new(soldierStorage) Soldier;
}

void Soldier::Destruct(void)
{
    soldierStorage->~Soldier();
}

SoldierInteractor::SoldierInteractor(SoldierController *controller)
{
    soldierController = controller;
}

SoldierInteractor::~SoldierInteractor()
{
}

void SoldierInteractor::HandleInteractionEvent(InteractionEventType type, Node *node, const Point3D *position)
{
    // Always call the base class counterpart.
    
    Interactor::HandleInteractionEvent(type, node, position);
    
    // If the node with which we are interacting has a controller,
    // then pass the event through to that controller.
    
    Controller *controller = node->GetController();
    if (controller)
    {
        controller->HandleInteractionEvent(type, position);
    }
}

SoldierController::SoldierController() : CharacterController(kControllerSoldier),
soldierInteractor(this)
{
    soldierMotion = kMotionNone;
    movementFlags = 0;
    
    modelAzimuth = 0.0F;
    modelAltitude = -45.0F;
    
    orientationMessageTimer = 0;
}

SoldierController::SoldierController(float azimuth) :
CharacterController(kControllerSoldier),
soldierInteractor(this)
{
    soldierMotion = kMotionNone;
    movementFlags = 0;
    
    modelAzimuth = azimuth;
    modelAltitude = 0.0F;
}

SoldierController::SoldierController(const SoldierController& soldierController) :
CharacterController(soldierController),
soldierInteractor(this)
{
    soldierMotion = kMotionNone;
    movementFlags = 0;
    
    modelAzimuth = 0.0F;
    modelAltitude = 0.0F;
}

SoldierController::~SoldierController()
{
}

Controller *SoldierController::Replicate(void) const
{
    return (new SoldierController(*this));
}

void SoldierController::Preprocess(void)
{
    // This function is called once before the target node is ever
    // rendered or moved. The base class Preprocess() function should
    // always be called first, and then the subclass can do whatever
    // preprocessing it needs to do.
    
    CharacterController::Preprocess();
    
    SetRigidBodyFlags(kRigidBodyKeepAwake | kRigidBodyFixedOrientation);
    SetFrictionCoefficient(0.0F);
    
    // We use a frame animator to play animation resources
    // for the soldier model.
    
    Model *soldier = GetTargetNode();
    frameAnimator.SetTargetModel(soldier);
    soldier->SetRootAnimator(&frameAnimator);
    
    // Initialize the previous center of mass to the current center of mass
    // so that this doesn't contain garbage the first time be call ActivateTriggers().
    
    previousCenterOfMass = GetFinalWorldTransform() * GetCenterOfMass();
    
    // Register our interactor with the world.
    
    soldier->GetWorld()->AddInteractor(&soldierInteractor);
}

void SoldierController::Move(void)
{
    // This function is called once per frame to allow the controller to
    // move its target node.
    
    // The movementIndexTable is a 16-entry table that maps all combinations of
    // the forward, backward, left, and right movement flags to one of eight directions.
    // The direction codes are as follows:
    //
    // 0 - forward
    // 1 - backward
    // 2 - left
    // 3 - right
    // 4 - forward and left
    // 5 - forward and right
    // 6 - backward and left
    // 7 - backward and right
    //
    // The number 8 in the table means no movement, and it appears where either no
    // movement buttons are being pressed or two opposing buttons are the only ones pressed
    // (e.g., left and right pressed simultaneously cancel each other out).
    
    static const unsigned char movementIndexTable[16] =
    {
        8, 0, 1, 8,
        2, 4, 6, 2,
        3, 5, 7, 3,
        8, 0, 1, 8
    };
        
        
    // Check if this soldier is the one being controlled by the local player
    GamePlayer *player = static_cast<GamePlayer*>(TheMessageMgr->GetLocalPlayer());
    if(player->GetController() == this)
    {
        // First, we grab the mouse deltas from the Input Manager.
        // We use these to change the angles representing the direction in
        // which the player is looking/moving.
        
        float azm = modelAzimuth + TheInputMgr->GetMouseDeltaX();
        //printf("%+.2f", TheInputMgr->GetMouseDeltaX());
        if (azm < -K::pi) azm += K::two_pi;
        else if (azm > K::pi) azm -= K::two_pi;
        
        float alt = modelAltitude + TheInputMgr->GetMouseDeltaY();
        if (alt < -1.45F) alt = -1.45F;
        else if (alt > 1.45F) alt = 1.45F;
        
        modelAzimuth = azm;
        modelAltitude = alt;
        
        // Update the timer. If we have reach 1/30th of a second, send a message
        // to the server to let it know the new orientation.
        orientationMessageTimer += TheTimeMgr->GetDeltaTime();
        if(orientationMessageTimer >= 33)
        {
            TheMessageMgr->SendMessage(kPlayerServer, ClientOrientationMessage(modelAzimuth));
            orientationMessageTimer = 0;
        }

    }
    
    
    
    
    // Now, we determine whether the player is attempting to move, and
    // we play the appropriate animation on the soldier model.
    
    long motion = soldierMotion;
    Vector2D propel(0.0F, 0.0F);
    
    long index = movementIndexTable[movementFlags & kMovementPlanarMask];
    if (index < 8)
    {
        // The movementDirectionTable maps each direction code looked up in the
        // movementIndexTable to an angle measured counterclockwise from the straight
        // ahead direction in units of pi/4.
        
        static const float movementDirectionTable[8] =
        {
            0.0F, 4.0F, 2.0F, -2.0F, 1.0F, -1.0F, 3.0F, -3.0F
        };
        
        float direction = movementDirectionTable[index] * K::pi_over_4 + modelAzimuth;
        
        // Set the propulsive force based on the direction of movement.
        
        propel = CosSin(direction) * 100.0F;
        
        // Determine whether we should play the forward or backward running animation.
        
        motion = ((index == 1) || (index >= 6)) ? kMotionBackward : kMotionForward;
    }
    else
    {
        // No movement flags are set, so we should play the standing animation.
        
        motion = kMotionStand;
    }
    
    // Update the external force for the rigid body representing the character.
    // The GroundContact() function is a member of the CharacterController base class.
    if (GetGroundContact())
    {
        SetExternalLinearResistance(Vector2D(10.0F, 10.0F));
        SetExternalForce(propel);
    }
    else
    {
        // If the soldier is not on the ground, reduce the propulsive force down to 2%.
        // This controls how well the player is able to control his movement while
        // falling through the air.
        
        SetExternalLinearResistance(Vector2D(0.0F, 0.0F));
        SetExternalForce(propel * 0.02F);
    }
    
    // Change the soldier's orientation based on horizontal mouse movement.
    // The SetCharacterOrientation() function is a member of the CharacterController base class.
    
    float direction = sqrt(pow(modelAzimuth,2) + pow(modelAltitude,2));
    
    SetCharacterOrientation(modelAzimuth);
    
    // If the animation needs to be changed, do it.
    
    if (motion != soldierMotion) SetSoldierMotion(motion);
    
    // Get the current center of mass and activate triggers along the line connecting to it
    // from the center of mass in the previous frame.
    
    Point3D cm = GetFinalWorldTransform() * GetCenterOfMass();
    Model *model = GetTargetNode();
    model->GetWorld()->ActivateTriggers(previousCenterOfMass, cm, 0.25F, model);
    previousCenterOfMass = cm;
    
    // Call the Model::Animate() function to update the animation playing for the model.
    
    GetTargetNode()->Animate();
    
}

void SoldierController::SetSoldierMotion(int32 motion)
{
    // This function sets the animation resource corresponding to
    // the current type of motion assigned to the soldier.
    
    Interpolator *interpolator = frameAnimator.GetFrameInterpolator();
    
    if (motion == kMotionStand)
    {
        frameAnimator.SetAnimation("soldier/Stand");
        interpolator->SetMode(kInterpolatorForward | kInterpolatorLoop);
    }
    else if (motion == kMotionForward)
    {
        frameAnimator.SetAnimation("soldier/Run");
        interpolator->SetMode(kInterpolatorForward | kInterpolatorLoop);
    }
    else if (motion == kMotionBackward)
    {
        frameAnimator.SetAnimation("soldier/Backward");
        interpolator->SetMode(kInterpolatorForward | kInterpolatorLoop);
    }
    
    soldierMotion = motion;
}

//void SoldierController::SetSoldierMotion(int32 motion)
//{
//	// This function sets the animation resource corresponding to
//	// the current type of motion assigned to the soldier.
//
//	Interpolator *interpolator = frameAnimator.GetFrameInterpolator();
//
//	if (motion == kMotionStand)
//	{
//		frameAnimator.SetAnimation("soldier/Stand");
//		interpolator->SetMode(kInterpolatorForward | kInterpolatorLoop);
//	}
//	else if (motion == kMotionForward)
//	{
//		frameAnimator.SetAnimation("soldier/Run");
//		interpolator->SetMode(kInterpolatorForward | kInterpolatorLoop);
//	}
//	else if (motion == kMotionBackward)
//	{
//		frameAnimator.SetAnimation("soldier/Backward");
//		interpolator->SetMode(kInterpolatorForward | kInterpolatorLoop);
//	}
//
//	soldierMotion = motion;
//}

void SoldierController::ReceiveMessage(const ControllerMessage *message)
{
    switch (message->GetControllerMessageType())
    {
        case kSoldierMessageState:
        {
            const SoldierStateMessage *msg = static_cast<const SoldierStateMessage *>(message);
            GetTargetNode()->SetNodePosition(msg->GetPosition());
            break;
        }
        case kSoldierMessageDestroy:
        {
            Destroy();
            break;
        }
        case kSoldierMessageBeginMovement:
        {
            const SoldierMovementMessage *m = static_cast<const SoldierMovementMessage *>(message);
            unsigned long flag = m->GetMovementFlag();
            movementFlags |= flag;
            break;
        }
        case kSoldierMessageEndMovement:
        {
            const SoldierMovementMessage *m = static_cast<const SoldierMovementMessage *>(message);
            movementFlags &= ~m->GetMovementFlag();
            break;
        }
        case kSoldierMessageOrientation:
        {
            // Make sure not to set the orientation if this is the local player
            GamePlayer *player = static_cast<GamePlayer*>(TheMessageMgr->GetLocalPlayer());
            if (player->GetController() != this)
            {
                const SoldierOrientationMessage *m = static_cast<const SoldierOrientationMessage *>(message);
                modelAzimuth = m->GetOrientation();
            }
            break;
        }
    }
    
    CharacterController::ReceiveMessage(message);
}

ControllerMessage *SoldierController::ConstructMessage(ControllerMessageType type) const
{
    switch (type)
    {
        case kSoldierMessageState:
            
            return (new SoldierStateMessage(GetControllerIndex()));
            
        case kSoldierMessageBeginMovement:
        case kSoldierMessageEndMovement:
            
            return (new SoldierMovementMessage(type, GetControllerIndex()));
            
        case kSoldierMessageOrientation:
            
            return (new SoldierOrientationMessage(GetControllerIndex()));
    }
    return (CharacterController::ConstructMessage(type));
}

void SoldierController::Destroy()
{
    Node *node = this->GetTargetNode();
    if (node)
    {
        delete node;
    }
}

void SoldierController::BeginMovement(unsigned long flag)
{
    const Point3D& position = GetTargetNode()->GetWorldPosition();
    Vector3D velocity = GetLinearVelocity();
    
    SoldierMovementMessage message(kSoldierMessageBeginMovement, GetControllerIndex(), position, velocity, flag);
    TheMessageMgr->SendMessageAll(message);
}

void SoldierController::EndMovement(unsigned long flag)
{
    const Point3D& position = GetTargetNode()->GetWorldPosition();
    Vector3D velocity = GetLinearVelocity();
    
    SoldierMovementMessage message(kSoldierMessageEndMovement, GetControllerIndex(), position, velocity, flag);
    TheMessageMgr->SendMessageAll(message);
}

void SoldierController::BeginOrientation(float orientation)
{
    SoldierOrientationMessage message(GetControllerIndex(), orientation);
    TheMessageMgr->SendMessageAll(message);
}

SoldierStateMessage::SoldierStateMessage(long contIndex) : ControllerMessage(SoldierController::kSoldierMessageState, contIndex)
{
    position.Set(0.0F, 0.0F, 0.0F);
}

SoldierStateMessage::SoldierStateMessage(long contIndex, Point3D p) : ControllerMessage(SoldierController::kSoldierMessageState, contIndex)
{
    position = p;
}

SoldierStateMessage::~SoldierStateMessage()
{
}

void SoldierStateMessage::Compress(Compressor &data) const
{
    ControllerMessage::Compress(data);
    data << position;
}

bool SoldierStateMessage::Decompress(Decompressor &data)
{
    if (ControllerMessage::Decompress(data))
    {
        data >> position;
        return true;
    }
    return false;
}

SoldierDestroyMessage::SoldierDestroyMessage(long contIndex) : ControllerMessage(SoldierController::kSoldierMessageDestroy, contIndex)
{
}

SoldierDestroyMessage::~SoldierDestroyMessage()
{
}

SoldierMovementMessage::SoldierMovementMessage(ControllerMessageType type, long controllerIndex) : CharacterStateMessage(type, controllerIndex)
{
}

SoldierMovementMessage::SoldierMovementMessage(ControllerMessageType type, long controllerIndex, const Point3D& position, const Vector3D& velocity, unsigned long flag) : CharacterStateMessage(type, controllerIndex, position, velocity)
{
    movementFlag = flag;
}

SoldierMovementMessage::~SoldierMovementMessage()
{
}

void SoldierMovementMessage::Compress(Compressor& data) const
{
    CharacterStateMessage::Compress(data);
    
    data << (unsigned char)movementFlag;
}

bool SoldierMovementMessage::Decompress(Decompressor& data)
{
    if (CharacterStateMessage::Decompress(data))
    {
        unsigned char	flag;
        
        data >> flag;
        movementFlag = flag;
        
        return (true);
    }
    
    return (false);
}

CharacterStateMessage::CharacterStateMessage(ControllerMessageType type, long controllerIndex) : ControllerMessage(type, controllerIndex)
{
}

CharacterStateMessage::CharacterStateMessage(ControllerMessageType type, long controllerIndex, const Point3D& position, const Vector3D& velocity) : ControllerMessage(type, controllerIndex)
{
    initialPosition = position;
    initialVelocity = velocity;
}

CharacterStateMessage::~CharacterStateMessage()
{
}

void CharacterStateMessage::Compress(Compressor& data) const
{
    ControllerMessage::Compress(data);
    
    data << initialPosition;
    data << initialVelocity;
}

bool CharacterStateMessage::Decompress(Decompressor& data)
{
    if (ControllerMessage::Decompress(data))
    {
        data >> initialPosition;
        data >> initialVelocity;
        return (true);
    }
    
    return (false);
}

SoldierOrientationMessage::SoldierOrientationMessage(long contIndex) : ControllerMessage(SoldierController::kSoldierMessageOrientation, contIndex)
{
}

SoldierOrientationMessage::SoldierOrientationMessage(long contIndex, float _orientation) : ControllerMessage(SoldierController::kSoldierMessageOrientation, contIndex)
{
    orientation = _orientation;
}

SoldierOrientationMessage::~SoldierOrientationMessage()
{
}

void SoldierOrientationMessage::Compress(Compressor& data) const
{
    ControllerMessage::Compress(data);
    
    data << orientation;
}

bool SoldierOrientationMessage::Decompress(Decompressor& data)
{
    if (ControllerMessage::Decompress(data))
    {
        data >> orientation;
        return (true);
    }
    
    return (false);
}

