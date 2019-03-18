void Laser_main(ScriptComponent @p)
{
	while(true)
	{
		auto pos = p.position();
		if (pos.y < -10) p.despawn();
		p.suspend(60);
	}
}


void Laser_onCollision(ScriptComponent @p, Entity @collider)
{
	p.despawn();
}