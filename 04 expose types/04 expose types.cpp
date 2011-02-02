    /*

    04 - Exposing types into js with v8. 

            Also critical, the ability to make 'native' types exist in scripts.
            For example, unwrapping a texture or game class into js, or
            exposing a primitive like vec2 with native properties and code.
        
        This tutorial covers the basics of exposing types that can be instantiated
        on the javascript side.

    Common: 
        What?
            A simple introduction to understanding the v8 javascript engine.
        Why?
            Written for the #altdevblog www.altdevblogaday.com group.
        Who? 
            Written by Sven Bergstr�m ( FuzzYspo0N ).

        
    Discussion :
        
        These examples will serve the simple purpose of demonstrating commonly 
        asked questions with regards to v8 and the embedding of javascript 
        into a c++ application.

        Disclaimer - I am no expert on v8. I just use it, and have hit the wall
        a number of times. Hopefully, anything I say will help you understand the
        mentality, concepts and structure of the v8 engine and how to use it based
        on the journey that I have travelled with the engine.

        For the full post and more info - see the following link - http://altdevblogaday.com/change-is-as-good-as-a-holiday-an-introductio

    */

    #include <iostream>
    #include "../v8/v8.h"
    #include "../common/common.h"
       //Include the print message function from the previous example
    #include "../common/print.h" 
       //For seeing how to expose a class, view the header and cpp for vec2
    #include "vec2.h"
    
using namespace v8;

        //Here will be a simple vec2 class, we will expose this as a native
        //javascript type, so that the following becomes possible in scripts.

            //var position = new vec2(10, 10);
            //print( position.length );
            //print( position.mul(10).length );

    //The first step , is to set up a 'function' template. Objects in JS can be based on
    //function templates, and usually it is a preferred approach with v8 as it offers more
    //for the developer using it in c++.

        //So, we store a persistent template to reuse. When you
        //return a vec2() from c++ to js, you create a new instance
        //of one of these, and return that. This means you expose once,
        //and this becomes the factory for creating instances of vec2 objects.
    Persistent<FunctionTemplate> Vec2Template;

    void exposeVec2( Handle<ObjectTemplate> global ) {

             Locker lock;
             HandleScope scope;

                //Create the function template for the constructor, and point it to our constructor,
                //you can see this function inside vec2.cpp 
             Vec2Template = Persistent<FunctionTemplate>::New( FunctionTemplate::New( Vec2Constructor ) );

                //We can tell the scripts what type to report. In this case, just vec2 will do.
	         Vec2Template->SetClassName(String::New("vec2"));
            
                //This template is the unique properties of this type, we can set 
                //functions, getters, setters, and values directly on each new vec2() object.
	        Handle<ObjectTemplate> Vec2IT = Vec2Template->InstanceTemplate();

                //Again, this is to store the c++ object for use in callbacks.
	        Vec2IT->SetInternalFieldCount(1);

                //These functions are in vec2.h and vec2.cpp, we are exposing the class type here.

            Vec2IT->Set( String::New("mul"), FunctionTemplate::New( InvocationCallback( vec2::MulCallback ) ));
            
                //Accessors are called everytime an assignment, or query is done on a property.
                //For example, position.length = 5; will call the Setter accessor. In this case,
                //you cannot set the length value, so any assignment is ignored inside the setter.
                //You can also leave it out, or pass 0 which means we don't
                //have a setter at all. When you use var len = position.length; it will execute the
                //callback specified on the getter. These callbacks differ from function callbacks, 
                //they have a different signature so be sure to note that.

            Vec2IT->SetAccessor( String::New("length"), AccessorGetter( vec2::GetLengthCallback ), AccessorSetter( vec2::SetLengthCallback ) );

                //Finally, we can tell the global scope that it now has a 'function' called vec2,
                //and that this function returns the template object above.
            global->Set( String::New("vec2"), Vec2Template );

        }

    int main(int argc, char **argv) 
    {

        Locker locker;
        HandleScope handle_scope;

        Handle<ObjectTemplate> globalTemplate = ObjectTemplate::New();

            //add the print function to the scope too
        exposePrint( globalTemplate );

            //Now, we expose (before the context) the type into the template.
        exposeVec2( globalTemplate );

            //Create our main context, we don't need to enter it now
        Handle<Context> context = Context::New( NULL, globalTemplate );

        if(argc > 1) {

            eScriptExecResult r = executeScript( std::string( argv[1] ) , context );

        } else {

            printf("Usage: <scriptname.js> \n Execute the javascript file.");

        }

        return 0;
    }