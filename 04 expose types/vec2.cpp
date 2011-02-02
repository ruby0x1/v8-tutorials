
    #include "../v8/v8.h"
    #include "vec2.h"
    using namespace v8;

    Handle<Value> Vec2Constructor( const Arguments& args )
    {
        Locker lock;
        HandleScope scope;

            //Our constructor is used as new vec2( x, y )
            //where x and y are numbers.

        if(!args[0].IsEmpty() && args[0]->IsNumber() &&
           !args[1].IsEmpty() && args[1]->IsNumber()) 
        {
               //Create a c++ instance of this type,
            vec2* vecObject = new vec2( args[0]->NumberValue(), args[1]->NumberValue() );
                //Create a JS version and store the pointer inside 
            Persistent<Object> self = Persistent<Object>::New( args.Holder() );
                //Note that this index 0 is the internal field we created in the template!
            self->SetInternalField(0, v8::External::New( vecObject ));

            return scope.Close( self );
        }
        
            //Not a valid constructor call, so undefined is returned.
        return Undefined();
     }

    //Helper function for clarity 
    vec2* UnwrapVec2( const AccessorInfo& info ) 
    {
            //Get the self object,
        Local<v8::Object> self = info.Holder();
            //Fetch the c++ pointer.
	    Local<External> external = Local<External>::Cast( self->GetInternalField(0) );
            //Get back to the type.
	    vec2* vec = static_cast<vec2*>( external->Value());
            //Return it
        return vec;
    }


    Handle<Value> vec2::GetLengthCallback( Local<String> prop , const AccessorInfo& info )
    {
        Locker lock;
        HandleScope scope;

        vec2* thisVec2 = UnwrapVec2( info );
        if(thisVec2 != NULL) 
        {
            Handle<Number> lengthVal = Number::New( thisVec2->length() );
            return scope.Close( lengthVal );
        }

        return Undefined();
    }

    void vec2::SetLengthCallback( Local<String> prop, Local<Value> value, const AccessorInfo& info ) 
    {
        //We don't allow changing the value at all.
        //So return undefined.

        return;
    } 

    Handle<Value> vec2::MulCallback(const Arguments& args)
    {
        Locker lock;
        HandleScope scope;

        if(args.Length() == 1 && args[0]->IsNumber()) 
        {
                //Get the self object,
            Local<v8::Object> self = args.Holder();
                //Fetch the c++ pointer.
	        Local<External> external = Local<External>::Cast( self->GetInternalField(0) );
                //Get back to the type.
	        vec2* thisVec2 = static_cast<vec2*>( external->Value());

            if(thisVec2 != NULL) 
            {
                thisVec2->mul( args[0]->NumberValue() );
                return scope.Close( self );
            }
        }

        return Undefined();
    }