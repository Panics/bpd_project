#ifndef __SharedPtr_H__
#define __SharedPtr_H__

	template<class T> 
	class  SharedPtr {
	protected:
		T* pRep;
		unsigned int* pUseCount;

		template<class Y>
		friend class SharedPtr;

	public:
	
		// ------------------------------------------------------------------------------------------------
		SharedPtr() : pRep(0), pUseCount(0) {
        }

		// ------------------------------------------------------------------------------------------------
        template< class Y>
		SharedPtr(SharedPtr<Y> rep) 
			: pRep(0), pUseCount(0)
		{
			// TODO: dynamic_cast or reinterpret_cast ?
			pRep = dynamic_cast<T*>(rep.get());
			if(pRep){
				pUseCount = rep.pUseCount; 
				++(*pUseCount); 
			}
		}

		explicit SharedPtr(T* rep) : 
			pRep(rep)
			, pUseCount(pRep ? (unsigned int*) new(unsigned int) : 0)
		{
			if(pUseCount)
				*pUseCount=1;
		}

		// ------------------------------------------------------------------------------------------------
		SharedPtr(const SharedPtr& r)
            : pRep(0), pUseCount(0)
		{
		    pRep = r.pRep;
		    pUseCount = r.pUseCount; 
		    // Handle zero pointer gracefully to manage STL containers
		    if(pUseCount) {
			    ++(*pUseCount); 
		    }
		}

		// ------------------------------------------------------------------------------------------------
		SharedPtr& operator=(const SharedPtr& r) {
			if (pRep == r.pRep)
				return *this;
			// Swap current data into a local copy
			// this ensures we deal with rhs and this being dependent
			SharedPtr<T> tmp(r);
			_swap(tmp);
			return *this;
		}
		
		// ------------------------------------------------------------------------------------------------
		//template< class Y>
		//SharedPtr<Y>& operator=(const SharedPtr& r) {
		//	if (pRep == r.getPointer())
		//		return *this;
		//	// Swap current data into a local copy
		//	// this ensures we deal with rhs and this being dependent
		//	SharedPtr<T> tmp(r);
		//	swap(tmp);
		//	return *this;
		//}

		// ------------------------------------------------------------------------------------------------
		virtual ~SharedPtr() {
            _release();
		}

		// ------------------------------------------------------------------------------------------------
		inline T& operator*() const { return *pRep; }
		inline T* operator->() const { return pRep; }
		inline bool operator!() const { return (pRep == 0); }
		inline operator T*() const { return pRep; }
		inline T* get() const { return pRep; }

		// ------------------------------------------------------------------------------------------------
		//void bind(T* rep) {
		//	assert(!pRep && !pUseCount);
		//	pUseCount = new(unsigned int);
		//	pRep = rep;
		//}

		// ------------------------------------------------------------------------------------------------
		inline bool unique() const {  return *pUseCount == 1; }

		// ------------------------------------------------------------------------------------------------
		inline unsigned int useCount() const { return *pUseCount; }

		// ------------------------------------------------------------------------------------------------
		inline unsigned int* useCountPointer() const { return pUseCount; }

		// ------------------------------------------------------------------------------------------------
		inline bool isNull(void) const { return pRep == 0; }

		// ------------------------------------------------------------------------------------------------
        inline void reset(T* rep=0) { 
			if (pRep){
				_release();
				pRep = 0;
				pUseCount = 0;
			}

			if(rep!=0){
				pRep = rep;
				pUseCount = (unsigned int*) new(unsigned int);
				*pUseCount=1;
			}
        }

    protected:

		// ------------------------------------------------------------------------------------------------
        inline void _release(void){
			if (pUseCount){
				if (--(*pUseCount) == 0) {
					_destroy();
				}
			}
		}

		// ------------------------------------------------------------------------------------------------
        virtual void _destroy(void){
            // IF YOU GET A CRASH HERE, YOU FORGOT TO FREE UP POINTERS
            // BEFORE SHUTTING DOWN
            // Use reset() before shutdown or make sure your pointer goes
            // out of scope before shutdown to avoid this.
			delete pRep;
            delete pUseCount;

			pRep = 0;
			pUseCount = 0;
		}

		// ------------------------------------------------------------------------------------------------
		virtual void _swap(SharedPtr<T> &other) {
                        T* tmp = pRep;
                        pRep = other.pRep;
                        other.pRep = tmp;
                 
                 	unsigned int* tm = pUseCount;
                        pUseCount = other.pUseCount;
                        other.pUseCount = tm;
		}
	};

	// ------------------------------------------------------------------------------------------------
	template<class T, class U> inline bool operator==(SharedPtr<T> const& a, SharedPtr<U> const& b){
		return a.get() == b.get();
	}

	// ------------------------------------------------------------------------------------------------
	template<class T, class U> inline bool operator!=(SharedPtr<T> const& a, SharedPtr<U> const& b){
		return a.get() != b.get();
	}

	// ------------------------------------------------------------------------------------------------
//	template<class T, class U> inline bool operator<(SharedPtr<T> const& a, SharedPtr<U> const& b){
//		return std::less<const void*>()(a.get(), b.get());
//	}

		//};


#endif
