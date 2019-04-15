void Dialogue_main(ScriptComponent @p)
{
	p.setSleep(true);
}

void Dialogue_onCollision(ScriptComponent @p, Entity @e, PhysicsComponent @c)
{

}

void ShowDialogue(ScriptComponent @p, string s)
{
	p.setTextString(s);
}