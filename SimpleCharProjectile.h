#ifndef SimpleCharProjectile_h
#define SimpleCharProjectile_h


#include "C4World.h"
#include "C4Input.h"
#include "C4Cameras.h"
#include "C4Application.h"
#include "C4Interface.h"
#include "C4Particles.h"


// Every application/game module needs to declare a function called CreateApplication()
// exactly as follows. (It must be declared extern "C", and it must include the tag C4MODULEEXPORT.)
// The engine looks for this function in the DLL and calls it to create an instance of
// the subclass of the Application class that the application/game module defines.


namespace C4
{

	// This is the type of our custom particle system.

	enum
	{
		kParticleSystemSpark = 'sprk'
	};


	// New locator types are registered with the engine in the Game constructor.
	// The 'spec' locator is used to specify where the spectator camera should
	// be positioned when a world is loaded.

	enum
	{
		kLocatorSpectator = 'spec'
	};



	// The SparkParticleSystem class implements a simple particle system that
	// creates a small burst of sparks.

	class SparkParticleSystem : public LineParticleSystem
	{
		// This friend declaration allows the particle system registration object
		// to construct a SparkParticleSystem object using the private default constructor.

		friend class ParticleSystemReg < SparkParticleSystem > ;

	private:

		enum
		{
			kMaxParticleCount = 100
		};

		int32				sparkCount;

		// This is where information about each particle is stored.

		ParticlePool<>		particlePool;
		Particle			particleArray[kMaxParticleCount];

		SparkParticleSystem();

		bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

	public:

		SparkParticleSystem(int32 count);
		~SparkParticleSystem();

		void Preprocess(void) override;
		void AnimateParticles(void) override;
	};

}
	

#endif