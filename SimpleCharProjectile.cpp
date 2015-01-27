#include "SimpleCharProjectile.h"
#include "SimpleCharControllers.h"

using namespace C4;




SparkParticleSystem::SparkParticleSystem(int32 count) :

// Initialize the base class for line particles
// and tell it where the particle pool is.
LineParticleSystem(kParticleSystemSpark, &particlePool, "particle/Spark1"),

// Initialize the particle pool by telling it where the array
// of Particle structs is and how big it is.
particlePool(kMaxParticleCount, particleArray)
{
	// The SparkEffect node creates a small burst of sparks and
	// then self-destructs when all of them have burned out.

	sparkCount = count;
	SetLengthMultiplier(4.0F);
	SetParticleSystemFlags(kParticleSystemSelfDestruct);
}

SparkParticleSystem::SparkParticleSystem() :

// Initialize the base class for line particles
// and tell it where the particle pool is.
LineParticleSystem(kParticleSystemSpark, &particlePool, "particle/Spark1"),

// Initialize the particle pool by telling it where the array
// of Particle structs is and how big it is.
particlePool(kMaxParticleCount, particleArray)
{
	// This constructor gets used when the particle effect is being loaded from
	// a saved game. In this case, we don't need to initialize anything.
}

SparkParticleSystem::~SparkParticleSystem()
{
}

bool SparkParticleSystem::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	// Just return a sphere that's big enough to always enclose
	// all of the particles. This is in local coordinates.

	sphere->SetCenter(0.0F, 0.0F, 0.0F);
	sphere->SetRadius(20.0F);
	return (true);
}

void SparkParticleSystem::Preprocess(void)
{
	// This function creates the spark particles.

	// Always call the base class Preprocess() function.
	LineParticleSystem::Preprocess();

	// If there's already a particle in the system, then the effect was loaded
	// from a saved game. We only create new particles if the system is empty.

	if (!GetFirstParticle())
	{
		// Calculate the world-space center.
		Point3D center = GetSuperNode()->GetWorldTransform() * GetNodePosition();
		const ConstVector2D *trig = Math::GetTrigTable();

		int32 count = sparkCount;
		for (machine a = 0; a < count; a++)
		{
			// Grab a new unused particle from the pool.
			Particle *particle = particlePool.NewParticle();
			if (!particle)
			{
				break;
			}

			particle->emitTime = 0;							// Particle appears right away.
			particle->lifeTime = 500 + Math::Random(750);	// Particle lives 500-1250 milliseconds.
			particle->radius = 0.02F;						// The radius is 20 mm.
			particle->color.Set(1.0F, 1.0F, 0.1F, 1.0F);	// It's yellow.
			particle->orientation = 0;						// This doesn't matter for line particles.
			particle->position = center;					// It starts at the effect's center.

			// Calculate a random velocity in a random direction.
			float speed = Math::RandomFloat(0.004F);
			Vector2D csp = trig[Math::Random(128)] * speed;
			const Vector2D& cst = trig[Math::Random(256)];
			particle->velocity.Set(cst.x * csp.y, cst.y * csp.y, csp.x);

			// Add the particle to the particle system.
			AddParticle(particle);
		}
	}
}

void SparkParticleSystem::AnimateParticles(void)
{
	// This function is called once per frame to move the particles.

	// Times are in milliseconds.
	int32 dt = TheTimeMgr->GetDeltaTime();
	float fdt = TheTimeMgr->GetFloatDeltaTime();

	Particle *particle = GetFirstParticle();
	while (particle)
	{
		// Get the next particle now in case the current one is removed from the system.
		Particle *next = particle->nextParticle;

		int32 life = (particle->lifeTime -= dt);
		if (life > 0)
		{
			// Update velocity with gravity.
			particle->velocity.z += K::gravity * fdt;

			// Move the particle and see if it hit the floor plane at z=0.
			float z1 = particle->position.z - particle->radius;
			particle->position += particle->velocity * fdt;
			float z2 = particle->position.z - particle->radius;
			if (z1 * z2 <= 0.0F)
			{
				// The particle hit the floor, so reflect its velocity and remove some energy.
				particle->position.z = 0.05F - z2;
				particle->velocity.z *= -0.5F;
			}

			// If the particle is nearing the end of its life, fade it out.
			if (life < 100)
			{
				particle->color.alpha = (float)life * 0.01F;
			}
		}
		else
		{
			// Particle burned out.
			FreeParticle(particle);
		}

		particle = next;
	}
}