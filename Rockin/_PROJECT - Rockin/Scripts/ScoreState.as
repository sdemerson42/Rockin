ScriptComponent @gScoreState;

void ScoreState_main(ScriptComponent @p)
{
	p.setReg("score", 0);
	@gScoreState = p;
}

void ScoreState_onCollision(ScriptComponent @me, Entity @collider, PhysicsComponent @c)
{

}

void ScoreState_setScore(ScriptComponent @p, int score)
{
	p.setReg("score", score);
}

int ScoreState_getScore(ScriptComponent @p)
{
	return p.getReg("score");	
}