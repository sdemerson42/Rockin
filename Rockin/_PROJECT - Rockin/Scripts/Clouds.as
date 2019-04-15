void Clouds_main(ScriptComponent @me)
{
	while(true)
	{
		auto pos = me.position();
		if (pos.y + 1 > 600.0) me.setPosition(0.0, -600.0);
		else me.setPosition(0.0, pos.y + 0.5);
		me.suspend();
	}
}

void Clouds_onCollision(ScriptComponent @me, Entity @collider, PhysicsComponent @c)
{

}