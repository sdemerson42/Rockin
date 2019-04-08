void Portal_main(ScriptComponent @p)
{
	p.log("Portal OK");
	p.setRenderFrame(p.getReg("renderX"), p.getReg("renderY"), 32.0, 32.0);
	p.setSleep(true);
}

void Portal_execute(ScriptComponent @p)
{
	auto master = p.getScriptByTag("Master");
	Master_changeScene(p.getString(0), 0, p.getReg("destX"), p.getReg("destY"));
	p.playSound("Portal", 30.0f, false, 5);	
}

void Portal_onCollision(ScriptComponent @p, Entity @e)
{

}