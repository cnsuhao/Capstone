//
//  SimpleCharCamera.cpp
//  C4
//
//  Created by blayne tilban on 2014-11-26.
//
//

#include "SimpleCharCamera.h"

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

#include "SimpleCharSoldier.h"
#include "SimpleCharMultiplayer.h"

using namespace C4;

ChaseCamera::ChaseCamera() : FrustumCamera(1.0F, 1.0F)
{
    targetModel = nullptr;
}

ChaseCamera::~ChaseCamera()
{
}

void ChaseCamera::Move(void)
{
    GamePlayer *player = static_cast<GamePlayer*>(TheMessageMgr->GetLocalPlayer());
    if(player)
    {
        SoldierController* controller = player->GetController();
        if(controller)
        {
            Model *model = controller->GetTargetNode();
            if (model)
            {
                CollisionData	data;
                
                // Here, we calculate the local coordinate frame for the chase camera
                // based on the direction that the player is looking.
                //Vector2D t = CosSin(controller->GetModelAzimuth());
                //Vector2D u = CosSin(controller->GetModelAltitude());
                
                //Vector3D view(t.x * u.x, t.y * u.x, u.y);
                Vector3D view(1, 0, -1);
                Vector3D right(0, -1.0f, 0.0F);
                Vector3D down = view % right;
            
                // We are going to place the camera behind the player, but we don't
                // want the camera to go through any geometry, so we'll do a quick
                // check for a collision.
                
                const Point3D& position = model->GetWorldPosition();
                Point3D p1(position.x, position.y, position.z + 1.5F);
                Point3D p2 = p1 - view * 4.0F;
                
                if (GetWorld()->DetectCollision(p1, p2, 0.3F, kCollisionCamera, &data))
                {
                    // There's something in the way, so move the camera in closer
                    // to the player.
                    
                    float s = data.param;
                    p2 = p1 * (1.0F - s) + p2 * s;
                }
                
                SetNodeTransform(right, down, view, p2);
                
                //LookAtPoint(position);
            }
        }
    }
}
