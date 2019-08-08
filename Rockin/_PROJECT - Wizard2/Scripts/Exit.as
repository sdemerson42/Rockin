void Exit_main(ScriptComponent @p)
{
	p.setColliderSize(float(p.getReg("w")), float(p.getReg("h")));
	p.setSleep(true);
}

void Exit_onCollision(ScriptComponent @p, Entity @e, PhysicsComponent @pc)
{
	if (e.hasTag("Wizard"))
	{
		auto sceneName = p.getString(0);
		float destx = float(p.getReg("destx"));
		float desty = float(p.getReg("desty"));
		auto offsetAxis = p.getString(1);
		auto pos = p.position();
		auto wpos = e.scriptComponent().position();
		if (offsetAxis == "x")
		{
			auto delta = wpos.x - pos.x;
			destx += delta;
		}
		else if (offsetAxis == "y")
		{
			auto delta = wpos.y - pos.y;
			desty += delta;
		} 
		gMaster.setReg("destx", destx);
		gMaster.setReg("desty", desty);
		p.changeScene(sceneName);
	}
}