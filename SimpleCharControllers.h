#ifndef SimpleCharControllers_h
#define SimpleCharControllers_h

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

	

	enum
	{
		kControllerSpin = 'spin'
	};

	class SpinController;

	class SpinController : public Controller
	{
	private:

		float         spinRate;             // In radians per millisecond
		float         spinAngle;            // The current angle, in radians
		Transform4D   originalTransform;    // The target's original transform

		Model		*targetModel;

		SpinController(const SpinController& spinController);

		Controller *Replicate(void) const;



	public:

		SpinController();
		SpinController(float rate);
		~SpinController();

		float GetSpinRate(void) const
		{
			return (spinRate);
		}

		void SetSpinRate(float rate)
		{
			spinRate = rate;
		}

		Model *GetTargetModel(void) const
		{
			return (targetModel);
		}

		void SetTargetModel(Model *model)
		{
			targetModel = model;
		}

		Model *GetTargetNode(void) const
		{
			return (static_cast<Model *>(Controller::GetTargetNode()));
		}

		static bool ValidNode(const Node *node);

		// Serialization functions
		void Pack(Packer& data, unsigned_int32 packFlags) const;
		void Unpack(Unpacker& data, unsigned_int32 unpackFlags);

		// User interface functions
		int32 GetSettingCount(void) const;
		Setting *GetSetting(int32 index) const;
		void SetSetting(const Setting *setting);

		void Preprocess(void);

		// The function that moves the target node
		void Move(void);
	};

}


#endif