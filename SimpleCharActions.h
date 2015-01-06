//
//  SimpleCharActions.h
//  C4
//
//  Created by blayne tilban on 2014-11-26.
//
//

#ifndef __C4__SimpleCharActions__
#define __C4__SimpleCharActions__

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
    // These are action types used to define action bindings in the
    // Input Manager. If the four-character code for an action is
    // 'abcd', then any input control (there can be more than one)
    // bound to %abcd triggers the associated action.
    
    enum
    {
        kActionForward			= 'frwd',
        kActionBackward			= 'bkwd',
        kActionLeft				= 'left',
        kActionRight			= 'rght',
        kActionUp				= 'jump',
        kActionDown				= 'down',
        kActionUse				= 'fire'
    };
    
    // An Action object represents an input action that can be triggered by
    // some input control, such as a key on the keyboard or a button on a joystick.
    // The Begin() and End() methods are called when the button is pressed and
    // released, respectively. Actions are registered with the Input Manager when
    // the Game class is constructed.
    
    class MovementAction : public Action
    {
    private:
        
        unsigned_int32		movementFlag;
        
    public:
        
        MovementAction(unsigned_int32 type, unsigned_int32 flag);
        ~MovementAction();
        
        void Begin(void);
        void End(void);
    };
    
    
    class UseAction : public Action
    {
    public:
        
        UseAction(); 
        ~UseAction(); 
        
        void Begin(void);
        void End(void);
    };
}

#endif /* defined(__C4__SimpleCharActions__) */
