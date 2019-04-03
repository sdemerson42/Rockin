void NPC_main(ScriptComponent @p)
{
	p.log("NPC OK");
	p.setRenderFrame(p.getReg("renderX"), p.getReg("renderY"), 32.0, 32.0);
	p.setReg("node", 0);
	p.setSleep(true);
}

void NPC_updateNode(ScriptComponent @p)
{
	p.modReg("node", 1);
	if (p.getReg("node") == p.getReg("nodeTotal")) p.setReg("node", 0);
}

void NPC_onCollision(ScriptComponent @p, Entity @e)
{

}