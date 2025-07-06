#pragma once
namespace lit {
	template<typename T, typename... Args> class Functor {
	private:
		// yk we could really over complicate this shi for a performance benefit of less than like 2 milliseconds that would probably be overriden by the compiler anyways lmao
		// what i was thinking was basically taking a template varient into function call and then calling the function like that.
		// I mean or we could just fuck all this callable bullshit and just turn someshi into a pointer thingy. notsuretholwk. 
		// wtf am I yapping about lmao ^
		struct fwCall {
			virtual ~fwCall() = default;
			virtual T Invoke(Args&&... args) = 0;
		};
		template<typename F>
		struct FunctionCall : public fwCall{
			F functor;
			//micro optimization here move applicable. 
			FunctionCall(F&& f) : functor(f) {

			}
			T Invoke(Args&&... args) {
				return functor((args)...); // copy vars (slow ideally perfect forward) 
			}
 		};
		fwCall* m_pCall = nullptr;
	public:
		Functor() = default;
		template<typename F> Functor(F&& f) {
			this->m_pCall = new FunctionCall<F>(f);
		}
		T Call(Args&&... args) {
			if (m_pCall) return m_pCall->Invoke(args); // <---- Optimize here. lit::Take(args)...
		}
		T operator()(Args&&... args) {
			return Call(args...) // <---- Optimize here. lit::Take(args)...
		}
		~Functor() {
			delete m_pCall;
		}
	private:

	};
}