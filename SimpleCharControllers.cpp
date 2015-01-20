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

#include "SimpleCharControllers.h"
#include "SimpleCharMultiplayer.h"

#include "SimpleChar.h"

using namespace C4;

Vector3D ballVelocity = Vector3D(0.0f,0.0f,0.0f);//projectile velocity
float speed = 20.0F;//projectile initial speed

SpinController::SpinController() : Controller(kControllerSpin)
{
	// Set a default value for the spin rate of one revolution per second
	spinRate = K::two_pi / 1000.0F;
	spinAngle = 0.0F;
}

SpinController::SpinController(float rate) : Controller(kControllerSpin)
{
	spinRate = rate;
	spinAngle = 0.0F;
}

SpinController::~SpinController()
{
}

SpinController::SpinController(const SpinController& spinController) : Controller(spinController)
{
	spinRate = spinController.spinRate;
	spinAngle = 0.0F;
}

Controller *SpinController::Replicate(void) const
{
	return (new SpinController(*this));
}

bool SpinController::ValidNode(const Node *node)
{
	return (node->GetNodeType() == kNodeGeometry);
}

void SpinController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Controller::Pack(data, packFlags);

	// Write the spin rate
	data << spinRate;

	// Write the current angle
	data << spinAngle;

	// Write the original transform
	data << originalTransform;
}

void SpinController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Controller::Unpack(data, unpackFlags);

	// Read the spin rate
	data >> spinRate;

	// Read the current angle
	data >> spinAngle;

	// Read the original transform
	data >> originalTransform;
}

int32 SpinController::GetSettingCount(void) const
{
	// There's only one setting
	return (1);
}

Setting *SpinController::GetSetting(int32 index) const
{
	// Is it asking for the first setting?
	if (index == 0)
	{
		// Yes, return a new text setting and set its value in revolutions per second
		return (new TextSetting('rate', Text::FloatToString(spinRate * 1000.0F / K::two_pi),
			"Spin rate", 7, &EditTextWidget::FloatNumberFilter));
	}

	return (nullptr);
}

void SpinController::SetSetting(const Setting *setting)
{
	// Are we setting the spin rate?
	if (setting->GetSettingIdentifier() == 'rate')
	{
		// Yes, grab the value from the setting and convert it back to radians per millisecond
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		spinRate = Text::StringToFloat(text) * K::two_pi / 1000.0F;
	}
}

void SpinController::Preprocess(void)
{
	Controller::Preprocess();

	// Grab the original transform of the target node
	const Node *target = GetTargetNode();
	originalTransform = target->GetNodeTransform();

	// Set the kGeometryDynamic flag for any geometry nodes
	const Node *node = target;
	do
	{
		if (node->GetNodeType() == kNodeGeometry)
		{
			// Node is a geometry, so grab its object
			GeometryObject *object = static_cast<const Geometry *>(node)->GetObject();

			// Set the kGeometryDynamic flag
			object->SetGeometryFlags(object->GetGeometryFlags() | kGeometryDynamic);
		}

		// Iterate through entire subtree
		node = target->GetNextNode(node);
	} while (node);
}

void SpinController::Move(void)
{
	Matrix3D    rotator;

	// Calculate the new spin angle based on how much time has passed
	float angle = spinAngle + spinRate * TheTimeMgr->GetFloatDeltaTime();

	// Make sure it's in the [-pi, pi] range
	if (angle > K::pi) angle -= K::two_pi;
	else if (angle < -K::pi) angle += K::two_pi;

	spinAngle = angle;

	// Now make a 3x3 rotation matrix
	rotator.SetRotationAboutZ(angle);

	// We'll rotate about the center of the target node's bounding sphere
	Node *target = GetTargetNode();
	const Point3D& worldCenter = target->GetBoundingSphere()->GetCenter();
	Point3D objectCenter = target->GetInverseWorldTransform() * worldCenter;

	// Make a 3x4 transform that rotates about the center point
	Transform4D transform(rotator, objectCenter - rotator * objectCenter);

	// Apply the rotation transform to the original transform and
	// assign it to the node as its new transform
	target->SetNodeTransform(originalTransform * transform);

	// Invalidate the target node so that it gets updated properly
	target->Invalidate();
}

BallController::BallController() : RigidBodyController(kControllerBall)
{
    // This constructor is only called when a new ball model is created.
}

BallController::BallController(Vector3D& velocity) : RigidBodyController(kControllerBall)
{
    ballVelocity = velocity;
    // This constructor is  called when a new ball model is created with an initial velocity
}

BallController::BallController(const BallController& ballController) : RigidBodyController(ballController)
{
    // This constructor is called when a ball controller is cloned.
}

BallController::~BallController()
{
}

Controller *BallController::Replicate(void) const
{
    return (new BallController(*this));
}

bool BallController::ValidNode(const Node *node)
{
    // This function is called by the engine to determine whether
    // this particular type of controller can control the particular
    // node passed in through the node parameter. This function should
    // return true if it can control the node, and otherwise it should
    // return false. In this case, the controller can only be applied
    // to model nodes.
    
    return (node->GetNodeType() == kNodeModel);
}

void BallController::Preprocess(void)
{
    // This function is called once before the target node is ever
    // rendered or moved. The base class Preprocess() function should
    // always be called first, and then the subclass can do whatever
    // preprocessing it needs to do. In this case, we set the ball's
    // coefficient of restitution.
    
    RigidBodyController::Preprocess();
    
    SetGravityMultiplier(0.1F);
    //GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
    //Vector3D direction = world->GetCamera()->GetNodeTransform()[2];
    //Matrix3D cameraMatrix3D = world->GetCamera()->GetNodeTransform().GetMatrix3D();
    
    Model *model = static_cast<Model *>(Controller::GetTargetNode());
    
    //SetRigidBodyTransform(Transform4D(cameraMatrix3D, model->GetNodePosition()));
    
    model->Invalidate();
    SetRestitutionCoefficient(0.95F);
    SetLinearVelocity(ballVelocity);

}

RigidBodyStatus BallController::HandleNewRigidBodyContact(const RigidBodyContact *contact, RigidBodyController *contactBody)
{
    // This function is called when the ball makes contact with another rigid body.
    /*
    if (contactBody->GetControllerType() == kControllerBall)
    {
        // Add a sound effect and some sparks to the world.
        
        Node *node = GetTargetNode();
        World *world = node->GetWorld();
        Point3D position = node->GetWorldTransform() * contact->GetContactPosition();
        
        OmniSource *source = new OmniSource("model/Ball", 40.0F);
        source->SetNodePosition(position);
        world->AddNewNode(source);
        
        //SparkParticleSystem *sparks = new SparkParticleSystem(20);
        //sparks->SetNodePosition(position);
        //world->AddNewNode(sparks);
    }*/
    
    return (kRigidBodyUnchanged);
}