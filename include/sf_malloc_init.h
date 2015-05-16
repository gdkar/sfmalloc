#ifndef SF_MALLOC_INIT_H
#define SF_MALLOC_INIT_H

class SFMallocInitializer{
  public:
  typedef void (*VoidFunction)();
    SFMallocInitializer(const char *name, VoidFunction ctor, VoidFunction dtor)
      : m_name(name), m_ctor(ctor),m_dtor(dtor){
        if(ctor)ctor();
      }
    ~SFMallocInitializer(){
      if(m_dtor) m_dtor();
    }
private:
  const char *const m_name;
  const VoidFunction m_ctor;
  const VoidFunction m_dtor;

};
#define REGISTER_SFMALLOC_MODULE_INITIALIZER(name, body)\
  namespace{ \
    static void sf_malloc_init_module_##name () { body; } \
    SFMallocInitializer sf_malloc_initializer_module_##name (#name, \
        sf_malloc_init_module_##name, 0);\
  };\

#define REGISTER_SFMALLOC_MODULE_DESTRUCTOR(name, body)\
  namespace{ \
    static void sf_malloc_destruct_module_##name () { body; } \
    SFMallocInitializer sf_malloc_destructor_module_##name (#name, \
       0, sf_malloc_destruct_module_##name);\
  };
#endif
