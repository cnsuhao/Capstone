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


#include "SimpleChar.h"
#include "C4Panels.h"

#include "SimpleCharControllers.h"
#include "SimpleCharMultiplayer.h"
#include "SimpleCharSoldier.h"
#include "SimpleCharActions.h"
#include "SimpleCharCamera.h"

using namespace C4;


// This is the definition of the pointer to the Game class singleton.
// It should be initialized to nullptr, and its value will be set by
// the Game class constructor.

Game *C4::TheGame = nullptr;


C4::Application *ConstructApplication(void)
{
	// This function should simply return a pointer to a new instance of
	// the Application class. Normally, the application/game module will
	// define a subclass of the Application class (in this case, the
	// Game class) and return a pointer to a new instance of that type.
	//
	// This function is called exactly one time right after the
	// application/game module DLL is loaded by the engine. The returned
	// class is destroyed via the virtual destructor of the Application
	// class right before the application/game module DLL is unloaded.

	return (new Game);
}


GameWorld::GameWorld(const char *name) : World(name)
{
	// This constructor is called when the Game::ConstructWorld() function is
	// called to create a new world class. The world hasn't actually been loaded
	// from disk yet when we get here.

	//spawnLocator = nullptr;
}

GameWorld::~GameWorld()
{
}

const LocatorMarker *GameWorld::FindSpawnLocator(const Zone *zone)
{
	// Iterate through all of the markers in the zone.

	const Marker *marker = zone->GetFirstMarker();
	while (marker)
	{
		if (marker->Enabled())
		{
			MarkerType type = marker->GetMarkerType();
			if (type == kMarkerLocator)
			{
				const LocatorMarker *locator = static_cast<const LocatorMarker *>(marker);
				if (locator->GetLocatorType() == kLocatorSpawn)
				{
					return (locator);
				}
			}
		}

		// Get the next marker in the list.

		marker = marker->Next();
	}

	// Look in all of the subzones.

	const Zone *subzone = zone->GetFirstSubzone();
	while (subzone)
	{
		const LocatorMarker *locator = FindSpawnLocator(subzone);
		if (locator)
		{
			return (locator);
		}

		subzone = subzone->Next();
	}

	return (nullptr);
}

WorldResult GameWorld::Preprocess(void)
{
	// The Preprocess() function is called after the world has been constructed.
	// We must always call the base class Preprocess() function first. If it returns
	// an error, then we just return the same result code.

	WorldResult result = World::Preprocess();
	if (result != kWorldOkay)
	{
		return (result);
	}

	// The world is now completely loaded. We search for a locator node that represents the
	// player's spawn position. It has a locator type of kLocatorSpawn.

	spawnLocator = FindSpawnLocator(GetRootNode());

	return (kWorldOkay);
}

void GameWorld::Interact(void)
{
	// The Interact() function is called once per frame. Before calling the base
	// class's Interact() function, we set up the interaction probe to be a line
	// segment extending two meters from the players head in the direction that
	// the camera is looking.

	SoldierController *controller = TheGame->GetSoldierController();
	if (controller)
	{
		const Point3D& p = controller->GetTargetNode()->GetWorldPosition();
		Point3D position(p.x, p.y, p.z + 1.5F);

		const Vector3D& direction = chaseCamera.GetWorldTransform()[2];
		controller->GetSoldierInteractor()->SetInteractionProbe(position, position + direction * 2.0F);
	}

	// Always call the base class counterpart.

	World::Interact();
}

void GameWorld::Render(void)
{
	// This function is called once per frame to render the world.
	// The subclass may do whatever it needs to before or after rendering,
	// but at some point must call World::Render().

	World::Render();
}


Game::Game() :

		// This is the constructor for the main application/game module class.
		// This class is constructed by the ConstructApplication() function,
		// which is called right after the application/game DLL is loaded by
		// the engine. There is only one instance of this class, so it inherits
		// from the Singleton template, which we initialize first.

		Singleton<Game>(TheGame),

		// The display event handler encapsulates a function that gets called
		// when the Display Manager changes something like the screen resolution.

		displayEventHandler(&HandleDisplayEvent),

		// A model registration represents a model that can be instanced.
		// This particular declaration associates the kModelSoldier type with the
		// model named "Data/*/soldier/Soldier.mdl". The fourth parameter tells the
		// engine to pre-cache the model resource and not to display the model in
		// the World Editor. The last parameter specifies the default controller
		// type to assign to models of type kModelSoldier.

		//soldierModelReg(kModelSoldier, nullptr, "soldier/Soldier", kModelPrecache | kModelPrivate, kControllerSoldier),
        //soldierModelReg(kModelSoldier, nullptr, "model/duck", kModelPrecache | kModelPrivate, kControllerSoldier),


		spinControllerReg(kControllerSpin, "Spin"),

		// Locator markers are registered so that the World Editor
		// can display their names in the Get Info dialog box.

		locatorReg(kLocatorSpawn, "Spawn Location"),

		serverCommandObserver(this, &Game::HandleServerCommand),
		joinCommandObserver(this, &Game::HandleJoinCommand),
		cjoinCommandObserver(this, &Game::HandlecJoinCommand),
		hostCommandObserver(this, &Game::HandleHostCommand),

		serverCommand("server", &serverCommandObserver),
		joinCommand("join", &joinCommandObserver),
		cjoinCommand("cjoin", &cjoinCommandObserver),
		hostCommand("host", &hostCommandObserver)
{
	// This installs an event handler for display events. This is only
	// necessary if we need to perform some action in response to
	// display events for some reason.

	TheDisplayMgr->InstallDisplayEventHandler(&displayEventHandler);

	// This sets the function that is called when the user hits the
	// escape key during gameplay. We save the old function so that
	// it can be restored when the game DLL is unloaded.

	prevEscapeProc = TheInputMgr->GetEscapeProc();
	prevEscapeCookie = TheInputMgr->GetEscapeCookie();
	TheInputMgr->SetEscapeProc(&EscapeProc, this);
    
    Soldier::Construct();

	// This registers our world class constructor with the World Manager.
	// We only need to do this if we have defined a subclass of the World
	// class that holds extra information.

	TheWorldMgr->SetWorldConstructor(&ConstructWorld);
    TheMessageMgr->SetPlayerConstructor(&ConstructPlayer);

	// These create the movement actions that are used to
	// move the player around and interact with objects.

	forwardAction = new MovementAction(kActionForward, kMovementForward);
	backwardAction = new MovementAction(kActionBackward, kMovementBackward);
	leftAction = new MovementAction(kActionLeft, kMovementLeft);
	rightAction = new MovementAction(kActionRight, kMovementRight);
	upAction = new MovementAction(kActionUp, kMovementUp);
	downAction = new MovementAction(kActionDown, kMovementDown);
	useAction = new UseAction;

	prevEscapeProc = TheInputMgr->GetEscapeProc();
	prevEscapeData = TheInputMgr->GetEscapeCookie();
	TheInputMgr->SetEscapeProc(&EscapeProc, this);

	// These register our new actions with the Input Manager.

	TheInputMgr->AddAction(forwardAction);
	TheInputMgr->AddAction(backwardAction);
	TheInputMgr->AddAction(leftAction);
	TheInputMgr->AddAction(rightAction);
	TheInputMgr->AddAction(upAction);
	TheInputMgr->AddAction(downAction);
	TheInputMgr->AddAction(useAction);

	// Let the Interface Manager determine when to change input devices to gameplay mode.

	TheInterfaceMgr->SetInputManagementMode(kInputManagementAutomatic);

	// Register the console commands with the engine.
	TheEngine->AddCommand(&serverCommand);
	TheEngine->AddCommand(&joinCommand);
	TheEngine->AddCommand(&cjoinCommand);
	TheEngine->AddCommand(&hostCommand);

	// Set some settings in the network manager.
    TheNetworkMgr->SetProtocol(C4::kGameProtocol);
    TheNetworkMgr->SetPortNumber(C4::kGamePort);
    

	soldierController = nullptr;
	
	MainWindow::Open();
}

Game::~Game()
{
    TheMessageMgr->EndGame();
	// When the game DLL is about to be unloaded, this destructor is called.

	TheWorldMgr->UnloadWorld();
	TheWorldMgr->SetWorldConstructor(nullptr);
    

	delete useAction;
	delete downAction;
	delete upAction;
	delete rightAction;
	delete leftAction;
	delete backwardAction;
	delete forwardAction;

	// Restore the previous escape key handling function.

	TheInputMgr->SetEscapeProc(prevEscapeProc, prevEscapeData);
}

World *Game::ConstructWorld(const char *name, void *cookie)
{
	// This function is called when a new world is being loaded. It should
	// return a pointer to a newly constructed subclass of the World class.

	return (new GameWorld(name));
}

Player *Game::ConstructPlayer(PlayerKey key, void *data)
{
    return (new GamePlayer(key));
}

void Game::HandleDisplayEvent(const DisplayEventData *eventData, void *cookie)
{
	// This function is called when a display event occurs (because we
	// registered it in the Game constructor).

	if (eventData->eventType == kEventDisplayChange)
	{
		// The screen resolution has changed. Handle accordingly.
	}
}

void Game::EscapeProc(void *cookie)
{
	// This function is called when the user hits the escape key in gameplay
	// mode because we registered it using the InputMgr::SetEscapeProc() function.
}

EngineResult Game::LoadWorld(const char *name)
{
	// Attempt to load the world.

	WorldResult result = TheWorldMgr->LoadWorld(name);
	if (result == kWorldOkay)
	{
		GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
        
        //if (TheMessageMgr->Server())
        //{
            const LocatorMarker *locator = world->GetSpawnLocator();
            if (locator)
            {
                // If a spawn locator was found in the world, put a soldier character there.
                
                // The BeginSinglePlayerGame() function puts the Message Manager in single player mode.
                
                //TheMessageMgr->BeginSinglePlayerGame();
                
                // Calculate the angle corresponding to the direction the character is initially facing.
                
                //const Vector3D direction = locator->GetWorldTransform()[0];
                //float azimuth = Atan(direction.y, direction.x);
                
                // Load a soldier model and attach a controller to it.
                /*
                Model *model = Model::Get(kModelSoldier);
                SoldierController *controller = new SoldierController(azimuth);
                model->SetController(controller);
                TheGame->soldierController = controller;
                
                
                // Put the model in the world at the locator's position.
                
                model->SetNodePosition(locator->GetWorldPosition());
                locator->GetWorld()->AddNewNode(model);*/
                
                //SpinController *controller1 = new SpinController(5.0);
                //controller1->SetTargetModel(model);
                
                // Set the world's current camera to be our chase camera.
                // The world will not render without a camera being set.
                
                ChaseCamera *camera = world->GetChaseCamera();
                //camera->SetTargetModel(model);
                world->SetCamera(camera);
                
                //TheMessageMgr->SendMessageAll(RequestMessage());
                
            }
            
            TheEngine->Report("Player world loaded.");
       

        //}
		
	}

	return (result);
}

void Game::ReceiveMessage(Player *sender, const NetworkAddress &address, const Message *message)
{
	// Any message that gets created in the Game::CreateMessage function
	// will be recieved here. Messages can be handled either here or directly
	// in the message class's HandleMessage function.
	switch (message->GetMessageType())
	{
	case C4::kMessageRequest:
	{
		if (TheMessageMgr->Server())
		{
			Point3D loc;
			loc.x = -10.0F + Math::Random(20.0F);
			loc.y = -10.0F + Math::Random(20.0F);
			loc.z = 1.0F;

			long contIndex = TheWorldMgr->GetWorld()->NewControllerIndex();
			TheMessageMgr->SendMessageAll(SpawnMessage(sender->GetPlayerKey(), contIndex, loc));
		}
		break;
	}
	}
}

Message *Game::ConstructMessage(MessageType type, Decompressor &data) const
{
	switch (type)
	{
        case kMessageSpawn:
            
            return (new SpawnMessage);
            
        case kMessageRequest:
            
            return (new RequestMessage);
            
        case kMessageMovementBegin:
            
            return (new ClientMovementMessage(type));
            
        case kMessageMovementEnd:
            
            return (new ClientMovementMessage(type));
            
        case kMessageOrientation:
            
            return (new ClientOrientationMessage);
	}

	return nullptr;
}

//chat server
void Game::HandleServerCommand(Command *command, const char *text)
{
	// Next, we start the server. The 'true' parameter 
	// is to indicate that this machine is the server.
	TheMessageMgr->BeginMultiplayerGame(true);
	TheEngine->Report("Server initialized", kReportError);
}

//chat join
void Game::HandlecJoinCommand(Command *command, const char *text)
{
	// We'll first want to provide the user with some feedback - so he'll know what he's doing.
	String<128> str("Attempting to join ");
	str += text;
	TheEngine->Report(str, kReportError);

	// Next, we convert the entered parameters into a C4 NetworkAddress.
	// This format is used internally. It has both an IP address and a port number.
	NetworkAddress address = MessageMgr::StringToAddress(text);

	// We explicitly set a port in this example - it defaults to 0.
	address.SetPort(kGamePort);

	// Now we're just going to (try to) connect to the entered address.
	TheMessageMgr->Connect(address);
}

void Game::HandleHostCommand(Command *command, const char *text)
{
    /*
    TheNetworkMgr->SetPortNumber(kGamePort);
    TheNetworkMgr->SetBroadcastPortNumber(kGamePort);

    TheMessageMgr->BeginMultiplayerGame(true);
    TheEngine->Report(String<>("Initialized. Hosting on: ") + MessageMgr::AddressToString(TheNetworkMgr->GetLocalAddress(), true));
    
    currentWorldName = text;
    
    TheGame->LoadWorld(text);
    
    //TheMessageMgr->SendMessageAll(RequestMessage());*/
    
    if (*text != 0)
    {
        ResourceName	name;
        
        Text::ReadString(text, name, kMaxResourceNameLength);
        HostMultiplayerGame(name, 0);
    }
    else
    {
        //HostGameWindow::Open();
    }
    
}

EngineResult Game::HostMultiplayerGame(const char *name, unsigned_int32 flags)
{
    //TheNetworkMgr->SetPortNumber(kGamePort);
    //TheNetworkMgr->SetBroadcastPortNumber(kGamePort);
    
    TheMessageMgr->BeginMultiplayerGame(true);
    TheEngine->Report("Server initialized", kReportError);
    TheEngine->Report(String<>("Initialized. Hosting on: ") + MessageMgr::AddressToString(TheNetworkMgr->GetLocalAddress(), true));
    
    
    WorldResult result =TheGame->LoadWorld(name);
    TheGame->currentWorldName = name;
    
    
    TheMessageMgr->SendMessageAll(RequestMessage());
    
    if(result !=kWorldOkay){
        String<128> str("Error loading world: ");
        str+=name;
        TheEngine->Report(str, kReportError);
    }
    
    return (result);
}

void Game::HandleJoinCommand(Command *command, const char *text)
{
    TheGame->LoadWorld("world/inout");
    
    TheMessageMgr->BeginMultiplayerGame(false);
    // We'll first want to provide the user with some feedback - so he'll know what he's doing.
    String<128> str("Attempting to join --> ");
    str += text;
    
    TheEngine->Report(str, kReportError);
    
    TheNetworkMgr->SetPortNumber(0);
    //TheNetworkMgr->SetBroadcastPortNumber(kGamePort);
    
    
    // Now we're just going to (try to) connect to the entered address.
    
    NetworkAddress	address;
    
    address = MessageMgr::StringToAddress(text);
    
    address.SetPort(kGamePort);
    
    NetworkResult result = TheMessageMgr->Connect(address);
    TheEngine->Report(String<>("Attempting connection with: ") + MessageMgr::AddressToString(address, true));
    
    if (result == C4::kNetworkOkay)
    {
        TheEngine->Report("Network initialized. Waiting on response...");
    }
    else
    {
        TheEngine->Report(String<>("Issues arose when trying to initialize the connection. Code: ") += result);
    }
}


void Game::HandlePlayerEvent(PlayerEvent event, Player *player, const void *param)
{
	switch (event)
	{
		// We've received a chat. 
        case kPlayerChatReceived:
        {
            // We'll want to display the player's name in front of the chat message,
			// so we'll first paste the player's name and his message together in a String object.
			// We limit the size of the displayed text using the String class, which automatically
			// cuts off text that exceeds the boundary set in the template parameter.
			String<kMaxChatMessageLength + kMaxPlayerNameLength + 2> text(player->GetPlayerName());
			text += ": ";
			text += static_cast<const char *>(param);

			// Next, we'll make the completed message appear in the console.
			// The kReportError parameter tells the engine to put the message in the console. 
			// It doesn't actually mean there's an error.
			TheEngine->Report(text, kReportError);
			break;
        }
            
        case kPlayerConnected:
        {
            TheEngine->Report("Player connected.");

        }
        
        
        case C4::kPlayerInitialized:
        {
            TheEngine->Report("Player initialized.");
            
            GamePlayer *gp = nullptr;
            SoldierController *cont = nullptr;
            Node *node = nullptr;
            PlayerKey key = -1;
            long id = -1;
            Point3D loc;
            
            //send initial state
            
            // This player just joined, so send him 1 message per
            // player to spawn a character
            Player *p = TheMessageMgr->GetFirstPlayer();
            while(p)
            {
                gp = static_cast<GamePlayer *>(p);
                
                cont = gp->GetController();
                if(cont)
                {
                    node = cont->GetTargetNode();
                    
                    key = gp->GetPlayerKey();
                    id = cont->GetControllerIndex();
                    loc = node->GetWorldPosition();
                    
                    TheMessageMgr->SendMessage(player->GetPlayerKey(), SpawnMessage(key, id, loc));
                }
                p = p->Next();
            }
            
            break;
        }
            
        case C4::kPlayerDisconnected:
        case C4::kPlayerTimedOut:
        {
            // If a player disconnects or times out, send a message to every other
            // player to delete this soldier.
            GamePlayer *gp = static_cast<GamePlayer *>(player);
            TheMessageMgr->SendMessageAll(SoldierDestroyMessage(gp->GetController()->GetControllerIndex()));
            break;
        }
	}

	// Finally, we pass the player event to the parent Application's HandlePlayerEvent method,
	// so it can display errors if needed. The method does nothing at the moment, but we'll
	// add it just in case it will somewhere in the future.
	Application::HandlePlayerEvent(event, player, param);
}

void Game::HandleConnectionEvent(ConnectionEvent event, const NetworkAddress& address, const void *param)
{
	switch (event)
	{
		case C4::kConnectionClientOpened:
		{
			Engine::Report("Client Connected.");
			break;
		}
		case C4::kConnectionClientClosed:
		case C4::kConnectionClientTimedOut:
		{
		  Engine::Report("Client Connection Closed.");
		  break;
		}
		case C4::kConnectionServerAccepted:
		{
		  Engine::Report("We are connected");
		  TheMessageMgr->SendMessageAll(RequestMessage());

											  break;
		}
		case C4::kConnectionServerClosed:
		case C4::kConnectionServerTimedOut:
		{
		  Engine::Report("Server Connection Closed.");
		  UnloadWorld();
		  break;
		}
	}

	Application::HandleConnectionEvent(event, address, param);
}

void Game::SpawnSoldier(Player *player, Point3D location, int32 controllerIndex)
{
    World *world = TheWorldMgr->GetWorld();
    if (world)
    {
        GamePlayer *gPlayer = static_cast<GamePlayer *>(player);
        SoldierController *cont = gPlayer->GetController();
        if (!cont)
        {
            cont = new SoldierController();
            cont->SetControllerIndex(controllerIndex);
            
            gPlayer->SetController(cont);
            
            Model *soldier = Model::Get(C4::kModelSoldier);
            soldier->SetController(cont);
            
            soldier->SetNodePosition(location);
            world->AddNewNode(soldier);
        }
    }
    
    TheEngine->Report("Soldier Spawned.");
}


void Game::QuitGame()
{
	TheMessageMgr->DisconnectAll();
	TheEngine->Quit();
}


void Game::UnloadWorld(void)
{
	TheWorldMgr->UnloadWorld();

	TheMessageMgr->EndGame();

	TheGame->soldierController = nullptr;
}

// ZYVRPLN
