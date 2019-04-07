void Master_main(ScriptComponent @p)
{
	p.log("Master OK");
	p.playMusic("Assets/Sounds/Castle.wav", 10.0f, true);
	p.setSleep(true);
}

void Master_onCollision(ScriptComponent @p, Entity @collider)
{
	
}