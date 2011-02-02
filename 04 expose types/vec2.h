#pragma once
    
    #include "../v8/v8.h"
    using namespace v8;

    #include <math.h>

        //This is inside vec2.cpp, declared here.
    Handle<Value> Vec2Constructor( const Arguments& args );

class vec2 
{
    
    public:
        vec2(double firstx, double firsty ): x( firstx ), y( firsty ) { }
       ~vec2() { }

        double x;
        double y;
        
    private:

        inline void mul( const double scale )
        {
            x *= scale;
            y *= scale;
        }

        inline double length() 
        {
            return sqrt( x*x + y*y );
        }

    public: 

        //These are inside vec2.cpp

        //A getter has the following signature, 
    static Handle<Value> GetLengthCallback( Local<String> prop , const AccessorInfo& info );
        //While a setter has a value passed to it, 
    static void SetLengthCallback( Local<String> prop, Local<Value> value, const AccessorInfo& info );
        //and the normal function callbacks, with arg lists.
    static Handle<Value> MulCallback(const Arguments& args);
};
