void Meteor_main(ScriptComponent @p)
{
	while(true)
	{
		auto vec = p.position();
		
		if (vec.x < -50 or vec.x > 850 or vec.y < -50 or vec.y > 650)
		{
			p.despawn();
		}

		p.suspend(200);
	}
}

void Meteor_onCollision(ScriptComponent @p, Entity @collider, PhysicsComponent @c)
{
	if (collider.hasTag("Laser"))
	{
		Master_explosion(p);
		auto masterHandle = p.getScriptByTag("Master");
		if (masterHandle !is null) Master_incScore(masterHandle);

		p.despawn();
	}
	
	if (collider.hasTag("Hazard"))
	{
		p.playSound("Boom", 5.0f, false, 1);
	}
}

