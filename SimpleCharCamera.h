//
//  SimpleCharCamera.h
//  C4
//
//  Created by blayne tilban on 2014-11-26.
//
//

#ifndef __C4__SimpleCharCamera__
#define __C4__SimpleCharCamera__

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
    // The ChaseCamera class represents a camera that will track the player's movement.
    
    class ChaseCamera : public FrustumCamera
    {
    private:
        
        Model		*targetModel;
        
    public:
        
        ChaseCamera();
        ~ChaseCamera();
        
        Model *GetTargetModel(void) const
        {
            return (targetModel);
        }
        
        void SetTargetModel(Model *model)
        {
            targetModel = model;
        }
        
        void Move(void) override;
    };
    
    class ChaseOrthoCamera : public OrthoCamera
    {
    private:
       
        Model       *targetModel;
        
    public:
        
        ChaseOrthoCamera();
        ~ChaseOrthoCamera();
        
        Model *GetTargetModel(void) const
        {
            return (targetModel);
        }
        
        void SetTargetModel(Model *model)
        {
            targetModel = model;
        }
        
        void Move(void) override;
    };
}


#endif /* defined(__C4__SimpleCharCamera__) */
