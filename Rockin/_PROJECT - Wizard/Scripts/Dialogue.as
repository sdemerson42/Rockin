void Dialogue_main(ScriptComponent @p)
{
	p.log("Dialogue OK");
	p.setSleep(true);
}

void Dialogue_onCollision(ScriptComponent @p, Entity @e)
{

}

void ShowDialogue(ScriptComponent @p, string s)
{
	p.setTextString(s);
}