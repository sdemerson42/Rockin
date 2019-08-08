void Fireball_main(ScriptComponent @p)
{
	p.suspend(100);
	p.despawn();
}

void Fireball_onCollision(ScriptComponent @p, Entity @e, PhysicsComponent @c)
{	
	if (c.solid() and !e.hasTag("Wizard")) p.despawn();
}
