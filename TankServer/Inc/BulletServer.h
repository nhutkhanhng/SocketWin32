class BulletServer : public Bullet
{
public:
	static GameObjectPtr	StaticCreate() { return NetworkManagerServer::sInstance->RegisterAndReturn( new BulletServer() ); }
	void HandleDying() override;

	virtual bool		HandleCollisionWithCat( Tank* inCat ) override;

	virtual void Update() override;

protected:
	BulletServer();

private:
	float mTimeToDie;

};