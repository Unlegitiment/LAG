#include "gameSkeleton.h"

void fwGameSkeleton::SetMode(unsigned int INIT_MODE)
{
	if (INIT_MODE >= INIT_MAX) {
		return;
	}
	this->mode = INIT_MODE;
}

void fwGameSkeleton::Init()
{
	for (auto* funcInit : m_FunctionInit) {
		funcInit(this->mode);
	}
}

void fwGameSkeleton::AddFunctor_Startup(Functor func)
{
	this->m_FunctionInit.push_back(func);
}

void fwGameSkeleton::AddFunctor_Update(UpdateFunc func)
{
	this->m_FuncUpdate.push_back(func);
}

void fwGameSkeleton::AddFunctor_Shutdown(ShutdownFunc func)
{
	this->m_FunctionDestroy.push_back(func);
}

void fwGameSkeleton::Update()
{
	for (auto* func : m_FuncUpdate) {
		func();
	}
}

void fwGameSkeleton::Shutdown()
{
	for (auto* func : m_FunctionDestroy) {
		func();
	}
}
