//
//  SimpleCharSoldier.h
//  C4
//
//  Created by blayne tilban on 2014-11-25.
//
//

#ifndef __C4__SimpleCharSoldier__
#define __C4__SimpleCharSoldier__

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

namespace C4
{
    class SoldierController;
    
    // The Interactor class is used to track player interactions with objects in the scene.
    // These are movement flags used by the soldier controller. They are set or cleared
    // by the Begin() and End() functions in the MovementAction class.
    
    enum
    {
        kMovementForward = 1 << 0,
        kMovementBackward = 1 << 1,
        kMovementLeft = 1 << 2,
        kMovementRight = 1 << 3,
        kMovementUp = 1 << 4,
        kMovementDown = 1 << 5,
        kMovementPlanarMask = 15
    };
    
    // These are motion states that are used to keep track
    // of which animation should be played.
    
    enum
    {
        kMotionNone,
        kMotionStand,
        kMotionForward,
        kMotionBackward
    };
    
    // This is the type for the controller that we use to move the soldier.
    
    enum
    {
        kControllerSoldier = 'sold'
    };
    
    class Soldier
    {
    private:
        
        ControllerReg<SoldierController>	soldierControllerRegistration;
        ModelRegistration					soldierModelRegistration;
        
        Soldier();
        ~Soldier();
        
    public:
        
        static void Construct(void);
        static void Destruct(void);
    };
    
    class SoldierInteractor : public Interactor
    {
    private:
        
        SoldierController	*soldierController;
        
    public:
        
        SoldierInteractor(SoldierController *controller);
        ~SoldierInteractor();
        
        void HandleInteractionEvent(InteractionEventType type, Node *node, const Point3D *position) override;
    };
    
    
    
    
    
    // Controllers are used to control anything that moves in the world.
    // New types of controllers defined by the application/game module can be
    // registered with the engine when the Game class is constructed.
    //
    // This particular controller is used to animate the soldier. It uses the
    // built-in character controller as a base class so that the engine's native
    // physics can be used to move the character.
    
    class SoldierController : public CharacterController
    {
    private:
        
        
        
        // The movement flags tell how the user is trying to move the player.
        
        unsigned_int32		movementFlags;
        
        // The soldier motion keeps track of what animation is currently playing.
        
        int32				soldierMotion;
        
        // The azimuth and altitude represent the direction the player is looking
        // by using the mouse.
        
        float				modelAzimuth;
        float				modelAltitude;
        
        // The frame animator controls playback of an animation resource.
        
        FrameAnimator		frameAnimator;
        
        // The previous center of mass stores the center point of the character on the
        // previous frame. This is used with the new center point to activate triggers.
        
        Point3D				previousCenterOfMass;
        
        // We keep an interactor object here in the controller.
        
        SoldierInteractor	soldierInteractor;
        
        SoldierController(const SoldierController& soldierController);
        
        Controller *Replicate(void) const override;
        
        void SetSoldierMotion(int32 motion);
        
        // We don't need to send an orientation message every frame, so instead lets keep
        // a simple timer and only send it every few frames.
        long orientationMessageTimer;
        
    public:
        SoldierController();
        SoldierController(float azimuth);
        ~SoldierController();
        
        Model *GetTargetNode(void) const
        {
            return (static_cast<Model *>(Controller::GetTargetNode()));
        }
        
        unsigned_int32 GetMovementFlags(void) const
        {
            return (movementFlags);
        }
        
        void SetMovementFlags(unsigned_int32 flags)
        {
            movementFlags = flags;
        }
        
        float GetModelAzimuth(void) const
        {
            return (modelAzimuth);
        }
        
        float GetModelAltitude(void) const
        {
            return (modelAltitude);
        }
        
        SoldierInteractor *GetSoldierInteractor(void)
        {
            return (&soldierInteractor);
        }
        
        void Preprocess(void) override;
        void Move(void) override;
        void Destroy(void);
        
        void BeginMovement(unsigned long flag);
        void EndMovement(unsigned long flag);
        void BeginOrientation(float orientation);
        void BeginFiring(float azimuth, Point3D position);
        
        void ReceiveMessage(const ControllerMessage *message);
        ControllerMessage *ConstructMessage(ControllerMessageType type) const;
        
        enum
        {
            kSoldierMessageBeginMovement = kRigidBodyMessageBaseCount,
            kSoldierMessageEndMovement,
            kSoldierMessageState,
            kSoldierMessageDestroy,
            kSoldierMessageOrientation,
            kSoldierMessageFired
        };
    };
    
    class SoldierStateMessage : public ControllerMessage
    {
    public:
        
        SoldierStateMessage(long controllerIndex);
        SoldierStateMessage(long controllerIndex, Point3D p);
        ~SoldierStateMessage();
        
        void Compress(Compressor &data) const override;
        bool Decompress(Decompressor &data) override;
        
        Point3D GetPosition(void) const
        {
            return position;
        }
        
        void SetPosition(Point3D p)
        {
            position = p;
        }
        
    private:
        
        Point3D position;
    };
    
    class SoldierDestroyMessage : public ControllerMessage
    {
    public:
        
        SoldierDestroyMessage(long controllerIndex);
        ~SoldierDestroyMessage();
    };
    
    class CharacterStateMessage : public ControllerMessage
    {
        friend class CharacterController;
        
    private:
        
        Point3D		initialPosition;
        Vector3D	initialVelocity;
        
    public:
        
        CharacterStateMessage(ControllerMessageType type, long controllerIndex);
        CharacterStateMessage(ControllerMessageType type, long controllerIndex, const Point3D& position, const Vector3D& velocity);
        ~CharacterStateMessage();
        
        const Point3D& GetInitialPosition(void) const
        {
            return (initialPosition);
        }
        
        const Vector3D& GetInitialVelocity(void) const
        {
            return (initialVelocity);
        }
        
        void Compress(Compressor& data) const;
        bool Decompress(Decompressor& data);
    };
    
    class SoldierMovementMessage : public CharacterStateMessage
    {
        friend class SoldierController;
        
    private:
        
        unsigned long	movementFlag;
        
        SoldierMovementMessage(ControllerMessageType type, long controllerIndex);
        
    public:
        
        SoldierMovementMessage(ControllerMessageType type, long controllerIndex, const Point3D& position, const Vector3D& velocity, unsigned long flag);
        ~SoldierMovementMessage();
        
        long GetMovementFlag(void) const
        {
            return (movementFlag);
        }
        
        void Compress(Compressor& data) const;
        bool Decompress(Decompressor& data);
    };
    
    class SoldierOrientationMessage : public ControllerMessage
    {
    private:
        
        float orientation;
        
    public:
        
        SoldierOrientationMessage(long contIndex);
        SoldierOrientationMessage(long contIndex, float _orientation);
        ~SoldierOrientationMessage();
        
        float GetOrientation(void) const
        {
            return orientation;
        }
        
        void Compress(Compressor& data) const;
        bool Decompress(Decompressor& data);
    };
    
    class SoldierFiredMessage : public ControllerMessage
    {
    private:
        
        float azimuth;
        Point3D position;
        
    public:
        
        SoldierFiredMessage(long contIndex);
        SoldierFiredMessage(long contIndex, float _azimuth, Point3D _position);
        ~SoldierFiredMessage();
        
        float GetAzimuth(void) const
        {
            return azimuth;
        }
        
        Point3D GetPosition(void) const
        {
            return position;
        }
        
        void Compress(Compressor& data) const;
        bool Decompress(Decompressor& data);
    };
}


#endif /* defined(__C4__SimpleCharSoldier__) */
