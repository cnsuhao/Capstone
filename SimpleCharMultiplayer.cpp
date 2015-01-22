#include "SimpleCharMultiplayer.h"
#include "SimpleChar.h"

using namespace C4;

GamePlayer::GamePlayer(PlayerKey key) : Player(key)
{
	cont = nullptr;
}

GamePlayer::~GamePlayer()
{
}

RequestMessage::RequestMessage() : Message(C4::kMessageRequest)
{
}

RequestMessage::~RequestMessage()
{
}

void RequestMessage::Compress(Compressor &data) const
{
}

bool RequestMessage::Decompress(Decompressor &data)
{
	return true;
}

SpawnMessage::SpawnMessage() : Message(C4::kMessageSpawn)
{
	this->contIndex = 0;
	this->key = -1;
}

SpawnMessage::SpawnMessage(PlayerKey playerkey, int32 controllerIndex, Point3D location) : Message(C4::kMessageSpawn)
{
	this->contIndex = controllerIndex;
	this->key = playerkey;
	this->loc = location;
}

SpawnMessage::~SpawnMessage()
{
}

bool SpawnMessage::HandleMessage(Player *sender) const
{
	Player *player = TheMessageMgr->GetPlayer(GetPlayerKey());
	TheGame->SpawnSoldier(player, GetLocation(), GetControllerIndex());
	return (true);
}

void SpawnMessage::Compress(Compressor &data) const
{
	data << loc;
	data << contIndex;
	data << key;
}

bool SpawnMessage::Decompress(Decompressor &data)
{
	data >> loc;
	data >> contIndex;
	data >> key;

	return (true);
}

ClientMovementMessage::ClientMovementMessage(MessageType type) : Message(type)
{
}

ClientMovementMessage::ClientMovementMessage(MessageType type, unsigned long flag) : Message(type)
{
	movementFlag = flag;
}

ClientMovementMessage::~ClientMovementMessage()
{
}

void ClientMovementMessage::Compress(Compressor& data) const
{
	data << (unsigned char)movementFlag;
}

bool ClientMovementMessage::Decompress(Decompressor& data)
{
	unsigned char	flag;

	data >> flag;
	movementFlag = flag;

	return (true);
}

bool ClientMovementMessage::HandleMessage(Player *sender) const
{
	SoldierController *controller = static_cast<GamePlayer *>(sender)->GetController();
	if (controller)
	{
		switch (GetMessageType())
		{
		case kMessageMovementBegin:

			controller->BeginMovement(movementFlag);
			break;

		case kMessageMovementEnd:

			controller->EndMovement(movementFlag);
			break;
		}
	}

	return (true);
}

ClientOrientationMessage::ClientOrientationMessage(void) : Message(kMessageOrientation)
{
}

ClientOrientationMessage::ClientOrientationMessage(float _orientation) : Message(kMessageOrientation)
{
	orientation = _orientation;
}

ClientOrientationMessage::~ClientOrientationMessage()
{
}

void ClientOrientationMessage::Compress(Compressor& data) const
{
	data << orientation;
}

bool ClientOrientationMessage::Decompress(Decompressor& data)
{
	data >> orientation;
	return (true);
}

bool ClientOrientationMessage::HandleMessage(Player *sender) const
{
	SoldierController *controller = static_cast<GamePlayer *>(sender)->GetController();
	if (controller)
	{
		controller->BeginOrientation(orientation);
	}

	return (true);
}

ClientFiringMessage::ClientFiringMessage(void) : Message(kMessageFired)
{
}

ClientFiringMessage::ClientFiringMessage(float azimuth) : Message(kMessageFired)
{
    firingAzimuth = azimuth;
}

ClientFiringMessage::~ClientFiringMessage()
{
}

void ClientFiringMessage::Compress(Compressor& data) const
{
    data << firingAzimuth;
}

bool ClientFiringMessage::Decompress(Decompressor& data)
{
    data >> firingAzimuth;
    return (true);
}

bool ClientFiringMessage::HandleMessage(Player *sender) const
{
    GamePlayer *player = static_cast<GamePlayer *>(sender);
    SoldierController *controller = player->GetController();
    if (controller)
    {
       controller->BeginFiring(GetFiringAzimuth(), controller->GetTargetNode()->GetWorldPosition());
    }
    
    return (true);
}