#pragma once
    
    #include "../v8/v8.h"
    using namespace v8;


    /* a simple print function, for printing information to stdout */
    Handle<Value> printMessage(const v8::Arguments& args) 
    {
        Locker locker;
        HandleScope scope;

        if( args.Length() ) 
        {
            String::Utf8Value message( args[0]->ToString() );
            if( message.length() ) 
            {
                //Print the message to stdout
                printf( "%s", *message );

                    //Here we assume newline, and then if the second parameter
                    //is supplied to the function, use that instead.
                bool newline = true;
                if(args.Length() == 2) 
                {
                    newline = args[1]->ToBoolean()->BooleanValue();
                }

                if(newline) printf("\n");

                return scope.Close( Boolean::New( true ) );
            }
        } 

        return Undefined();
    }

    void exposePrint( Handle<ObjectTemplate> targetObject ) 
    {
        targetObject->Set( String::New("print"), v8::FunctionTemplate::New( printMessage ));
    }