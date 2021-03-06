void P1_main(ScriptComponent @p)
{
	float speed = 4.5f;
	float laserSpeed = 6;
	int laserCooldown = 35;
	int laserTimer = 0;	

	while(true)
	{
		if (laserTimer > 0) --laserTimer;

		auto i = p.input();

		if (i.start == 1) p.changeSubscene("ScenePause");

		if (i.stickLeftX < 0) p.setMomentum(-1.0f * speed, 0);
		if (i.stickLeftX == 0) p.setMomentum(0, 0);
		if (i.stickLeftX > 0) p.setMomentum(1.0f * speed, 0);
		if (i.a > 0 and laserTimer == 0)
		{
			auto las = p.spawn("Laser");
			if (las !is null)
			{
				auto ppos = p.position();
				las.setPosition(ppos.x + 12, ppos.y - 18);
				las.setMomentum(0, -1*laserSpeed);
				laserTimer = laserCooldown;
				p.playSound("Laser", 30.0, false, 1);
			}
		}	
		p.suspend();
	}
}


void P1_onCollision(ScriptComponent @p, Entity @collider, PhysicsComponent @c)
{
	if (collider.hasTag("Hazard"))
	{	
		Master_explosion(p);
		Master_playerDead(gMaster);
		p.despawn();
	}
}