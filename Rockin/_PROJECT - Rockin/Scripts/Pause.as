void PauseLogic_main(ScriptComponent @me)
{
	me.log("Pause OK");
	while(true)
	{
		auto i = me.input();
		if (i.fire) me.changeSubscene("Scene1");
		me.suspend();
	}
}

void PauseLogic_onCollision(ScriptComponent @me, Entity @e)
{

}