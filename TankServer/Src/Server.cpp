#include <TankServerPCH.h>


//uncomment this when you begin working on the server

bool Server::StaticInit()
{
	sInstance.reset( new Server() );

	return true;
}

Server::Server()
{

	GameObjectRegistry::sInstance->RegisterCreationFunction( 'TANK', TankServer::StaticCreate );

	GameObjectRegistry::sInstance->RegisterCreationFunction( 'BULT', BulletServer::StaticCreate );

	InitNetworkManager();
	
	// Setup latency
	float latency = 0.0f;
	string latencyString = StringUtils::GetCommandLineArg( 2 );
	if( !latencyString.empty() )
	{
		latency = stof( latencyString );
	}
	NetworkManagerServer::sInstance->SetSimulatedLatency( latency );
}


int Server::Run()
{
	SetupWorld();

	return Engine::Run();
}

bool Server::InitNetworkManager()
{
	string portString = StringUtils::GetCommandLineArg( 1 );
	uint16_t port = stoi( portString );

	return NetworkManagerServer::StaticInit( port );
}


//namespace
//{
//	
//	void CreateRandomMice( int inMouseCount )
//	{
//		Vector3 mouseMin( -5.f, -3.f, 0.f );
//		Vector3 mouseMax( 5.f, 3.f, 0.f );
//		GameObjectPtr go;
//
//		//make a mouse somewhere- where will these come from?
//		for( int i = 0; i < inMouseCount; ++i )
//		{
//			go = GameObjectRegistry::sInstance->CreateGameObject( 'MOUS' );
//			Vector3 mouseLocation = TankMath::GetRandomVector( mouseMin, mouseMax );
//			go->SetLocation( mouseLocation );
//		}
//	}
//
//
//}


void Server::SetupWorld()
{
	// Inittialize something elseTank
}

void Server::DoFrame()
{
	NetworkManagerServer::sInstance->ProcessIncomingPackets();

	NetworkManagerServer::sInstance->CheckForDisconnects();

	NetworkManagerServer::sInstance->RespawnTanks();

	Engine::DoFrame();

	NetworkManagerServer::sInstance->SendOutgoingPackets();

}

void Server::HandleNewClient( ClientProxyPtr inClientProxy )
{
	
	int playerId = inClientProxy->GetPlayerId();
	
	ScoreBoardManager::sInstance->AddEntry( playerId, inClientProxy->GetName() );
	
	SpawnTankForPlayer( playerId );
}

void Server::SpawnTankForPlayer( int inPlayerId )
{
	TankPtr tank = std::static_pointer_cast< Tank >( GameObjectRegistry::sInstance->CreateGameObject( 'TANK' ) );
	tank->SetColor( ScoreBoardManager::sInstance->GetEntry( inPlayerId )->GetColor() );
	tank->SetPlayerId( inPlayerId );
	//gotta pick a better spawn location than this...
	tank->SetLocation( Vector3( 1.f - static_cast< float >( inPlayerId ), 0.f, 0.f ) );

}

void Server::HandleLostClient( ClientProxyPtr inClientProxy )
{
	int playerId = inClientProxy->GetPlayerId();

	
	ScoreBoardManager::sInstance->RemoveEntry( playerId );
	TankPtr tank = GetTankForPlayer( playerId );

	if( tank )
	{
		tank->SetDoesWantToDie( true );
	}
}

TankPtr Server::GetTankForPlayer( int inPlayerId )
{
	const auto& gameObjects = World::sInstance->GetGameObjects();
	for( int i = 0, c = gameObjects.size(); i < c; ++i )
	{
		GameObjectPtr go = gameObjects[ i ];
		Tank* cat = go->GetAsTank();
		if( cat && cat->GetPlayerId() == inPlayerId )
		{
			return std::static_pointer_cast< Tank >( go );
		}
	}

	return nullptr;

}