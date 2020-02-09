#include <istream>

class Flo2istream : public std::istream {
    public:
        Flo2istream(PyObject* obj);

    protected:
        PyObject* obj; 
        PyObject* read_meth; 
        PyObject* result = NULL; 
        
  
};