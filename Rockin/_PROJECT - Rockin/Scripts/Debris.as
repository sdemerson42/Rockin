void Debris_main(ScriptComponent @p)
{
	p.suspend(20);
	p.despawn();
}

void Debris_onCollision(ScriptComponent @me, Entity @collider, PhysicsComponent @c)
{
	
}