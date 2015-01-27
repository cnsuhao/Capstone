//=============================================================
//
// C4 Engine version 3.5.1
// Copyright 1999-2013, by Terathon Software LLC
//
// This file is part of the C4 Engine and is provided under the
// terms of the license agreement entered by the registed user.
//
// Unauthorized redistribution of source code is strictly
// prohibited. Violators will be prosecuted.
//
//=============================================================


#ifndef SimpleChar_h
#define SimpleChar_h


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
#include "C4Particles.h""

#include "SimpleCharControllers.h"
#include "SimpleCharSoldier.h"
#include "SimpleCharActions.h"
#include "SimpleCharCamera.h"
#include "SimpleCharMultiplayer.h"

// Every application/game module needs to declare a function called ConstructApplication()
// exactly as follows. (It must be declared extern "C", and it must include the tag C4MODULEEXPORT.)
// The engine looks for this function in the DLL and calls it to construct an instance of 
// the subclass of the Application class that the application/game module defines.

extern "C"
{
	C4MODULEEXPORT C4::Application *ConstructApplication(void);
}

namespace C4
{


	// Model types are associated with a model resource using the ModelRegistration
	// class. Models are registered with the engine in the Game constructor.

	enum
	{
		kModelSoldier = 'sold'
	};



	// New locator types are registered with the engine in the Game constructor.
	// The 'spwn' locator is used to specify where the player should be positioned
	// when a world is loaded.

	enum
	{
		kLocatorSpawn = 'spwn'
	};


	// The application/game module will usually define a subclass of the World
	// class so that extra information can be associated with the current world.
	// In this case, a pointer to a spawn locator and an instance of the ChaseCamera
	// class is included with the world. A new instance of this World subclass should
	// be returned when the Game::ConstructWorld() function is called (see below).

	class GameWorld : public World
	{
	private:

		const LocatorMarker		*spawnLocator;

		ChaseCamera				chaseCamera;

		static const LocatorMarker *FindSpawnLocator(const Zone *zone);

	public:

		GameWorld(const char *name);
		~GameWorld();

		const LocatorMarker *GetSpawnLocator(void) const
		{
			return (spawnLocator);
		}

		ChaseCamera *GetChaseCamera(void)
		{
			return (&chaseCamera);
		}

		ResourceResult Preprocess(void);

		void Interact(void);
		void Render(void);
	};


	// Every application/game module needs to define a subclass of the Application
	// class to serve as the primary interface with the engine. This subclass is
	// constructed and returned to the engine in the ConstructApplication() function.
	// There should be only one instance of this class, so it inherits from the
	// Singleton template. A pointer to the Game instance is declared below.

	class Game : public Application, public Singleton<Game>
	{
	private:

		DisplayEventHandler				displayEventHandler;

		//ModelRegistration				soldierModelReg;
		LocatorRegistration				locatorReg;

		ModelRegistration				ballModelReg;
		//ControllerReg<BallController>	controllerReg;//new
		//ParticleSystemReg<SparkParticleSystem>	sparkSystemReg;//new


		void							*prevEscapeData;
		InputMgr::KeyProc				*prevEscapeProc;
		void							*prevEscapeCookie;

		// Define the controller registration
		ControllerReg<SpinController>	spinControllerReg;

		MovementAction					*forwardAction;
		MovementAction					*backwardAction;
		MovementAction					*leftAction;
		MovementAction					*rightAction;
		MovementAction					*upAction;
		MovementAction					*downAction;
		UseAction						*useAction;

		FireAction						*fireAction;

		SoldierController				*soldierController;
		SpinController					*spinController;


		CommandObserver<Game>							serverCommandObserver;
		CommandObserver<Game>							joinCommandObserver;
		CommandObserver<Game>							cjoinCommandObserver;
		CommandObserver<Game>							hostCommandObserver;

		// The server command will start a new server
		Command							serverCommand;
		// The join command will join an existing game.
		Command							joinCommand;
		Command							cjoinCommand;

		Command							hostCommand;

		//static World *ConstructWorld(const char *name, void *cookie);

		ResourceName									currentWorldName;



		static void HandleDisplayEvent(const DisplayEventData *eventData, void *cookie);


		//static void EscapeProc(void *cookie);

	public:

		Game();
		~Game();

		SoldierController *GetSoldierController(void) const
		{
			return (soldierController);
		}

		SpinController *GetSpinController(void) const
		{
			return (spinController);
		}

		// This method will be executed whenever the user uses the server command.
		void HandleServerCommand(Command *command, const char *text);

		// This method will be executed whenever the user uses the join command.
		void HandleJoinCommand(Command *command, const char *text);
		void HandlecJoinCommand(Command *command, const char *text);

		void HandleHostCommand(Command *command, const char *text);

		EngineResult HostMultiplayerGame(const char *name, unsigned_int32 flags);
		EngineResult JoinMultiplayerGame(const char *name, unsigned_int32 flags);

		// This method will be called by the engine whenever a chat is received.
		// It's used for a lot of other stuff, but that's outside the scope of this tutorial.
		void HandlePlayerEvent(PlayerEvent event, Player *player, const void *param) override;
		void HandleConnectionEvent(ConnectionEvent event, const NetworkAddress& address, const void *param);

		void SpawnSoldier(Player *player, Point3D location, int32 controllerIndex);

		void CreateBall(GamePlayer *sender, float azimuth, Point3D position);

		void HostGame();
		void JoinGame(String<> ipAddress);
		void QuitGame();

		Message *ConstructMessage(MessageType type, Decompressor &data) const;
		void ReceiveMessage(Player *sender, const NetworkAddress &address, const Message *message);

		static World *ConstructWorld(const char *name, void *data);
		static Player *ConstructPlayer(PlayerKey key, void *data);
		static void EscapeProc(void *data);

		EngineResult LoadWorld(const char *name) override;
		void UnloadWorld(void) override;
	};

	

	// This is a pointer to the one instance of the Game class through which
	// any other part of the application/game module can access it.

	extern Game *TheGame;
}


#endif

// ZYVRPLN