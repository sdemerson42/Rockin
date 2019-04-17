void Fireball_main(ScriptComponent @p)
{
	p.setSleep(true);
}

void Fireball_onCollision(ScriptComponent @p, Entity @e, PhysicsComponent @c)
{
	if (c.solid() and !e.hasTag("P1")) p.despawn();
}