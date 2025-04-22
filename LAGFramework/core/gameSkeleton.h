#pragma once
#include <vector>
class fwGameSkeleton {
	typedef void(*UpdateFunc)();
	typedef void(*Functor)(int);
	typedef UpdateFunc ShutdownFunc;
public:
	enum InitModes {
		INIT_NULL,
		INIT_CORE,
		INIT_MAX
	};
	virtual void SetMode(unsigned int INIT_MODE);
	virtual void Init();
	void AddFunctor_Startup(Functor func);
	void AddFunctor_Update(UpdateFunc func);
	void AddFunctor_Shutdown(ShutdownFunc func);
	virtual void Update();
	virtual void Shutdown();
protected:
	std::vector<Functor>& GetInitFunctions() { return this->m_FunctionInit; }
	std::vector<UpdateFunc>& GetUpdateFunctions() { return this->m_FuncUpdate; }
	std::vector<ShutdownFunc>& GetShutdownFunctions() { return this->m_FunctionDestroy; }
private:
	unsigned int mode = INIT_NULL;
	std::vector<Functor> m_FunctionInit;
	std::vector<UpdateFunc> m_FuncUpdate;
	std::vector<ShutdownFunc> m_FunctionDestroy;
};