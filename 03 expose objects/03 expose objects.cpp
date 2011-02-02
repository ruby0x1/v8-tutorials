

    /*

    03 - Exposing preset objects in v8. 
    
            Often important is creating preset objects in script land. For example,
            in my tech i have a core object, and many other objects already defined 
            in the javascript scope before the first script is loaded. 
                audio.createSound, filesys.read, core.version etc.
        
        This tutorial covers the basics of exposing preset objects into a context.

    Common: 
        What?
            A simple introduction to understanding the v8 javascript engine.
        Why?
            Written for the #altdevblog www.altdevblogaday.com group.
        Who? 
            Written by Sven Bergström ( FuzzYspo0N ).

        
    Discussion :
        
        These examples will serve the simple purpose of demonstrating commonly 
        asked questions with regards to v8 and the embedding of javascript 
        into a c++ application.

        Disclaimer - I am no expert on v8. I just use it, and have hit the wall
        a number of times. Hopefully, anything I say will help you understand the
        mentality, concepts and structure of the v8 engine and how to use it based
        on the journey that I have travelled with the engine.

        For the full post and more info - see the following link -

    */

    #include <iostream>
    #include "../v8/v8.h"
    #include "../common/common.h"
       //Include the print message function from the previous example
    #include "../common/print.h" 
    
using namespace v8;

        //Here will be a simple game class, with one method
        //that we will expose to scripts. This is a direct function
        //being exposed, under a 'c++ game class', in scripts. 
        //In other words - the script side game object - will actually
        //represent an instance of this class. What it doesn't mean -
        //that game is now a type! This is a common catch - you are not
        //exposing a type, simply injecting a global object. Once
        //you injected that object - you injected a function into that
        //objects property set. Imagine the following javascript instead :

            //  var game = {}; 
            //      game.start = function() { print('game started!'); }


    //Declare these in advance
class Game;
 Game* UnwrapGameObject( Handle<Object> jsObject );

class Game {
    
    public:
            Game() {
            }

            ~Game() {
            }
        
    private:
            //The direct function of this class 
            //that will get called on.
        void start() 
        {
            printf("Game started!\n");
        }

            //The v8 handler for this specific function.
            //Of course, you could just printf() inside here
            //but the point is to demonstrate unwrapping a
            //native c++ pointer from a javascript callback.
    public:

        static Handle<Value> gameStart(const Arguments& args) 
        {
            Locker locker;
            HandleScope scope;
                
                //So , we can unwrap a c++ instance by using the args.Holder() or args.This()
                //This usually happens when the user has game.start(); the 'game' is what args
                //stores for us to use and unwrap.
            Game* game = UnwrapGameObject( args.This() );
            
                game->start();

            return Undefined();
        }
    };


    //Here is a helper function to ease the process - This inserts a named property with a callback
    //into the object requested. For example, game.start <- this would be simpler using the function here
    void ExposeProperty(Handle<Object> intoObject, Handle<Value> namev8String, InvocationCallback funcID)
    {
            Locker lock;
            HandleScope handle_scope;

            intoObject->Set( namev8String , FunctionTemplate::New( funcID )->GetFunction(), ReadOnly );
    }

        //This will expose an object with the type Game, into the global scope.
        //It will return a handle to the JS object that represents this c++ instance.
    Handle<Object> WrapGameObject( Game *gameInstance )
    {
		HandleScope scope;

            //This will persist when we leave the handle scope,
            //meaning that the actual c++ instance is preserved
		Persistent<ObjectTemplate> class_template;

            //This is just the preset for an emtpy object
		Handle<ObjectTemplate> raw_template = ObjectTemplate::New();

            //This is so we can store a c++ object on it
		raw_template->SetInternalFieldCount(1);

		    //Create the actual template object, 
        class_template = Persistent<ObjectTemplate>::New( raw_template );

		    //Create an instance of the js object 
		Handle<Object> result = class_template->NewInstance();

            //Create a wrapper for the c++ instance
		Handle<External> class_ptr = External::New( gameInstance ); 

		    //Store the 'external c++ pointer' inside the JS object
		result->SetInternalField( 0 , class_ptr );

		    //Return the JS object representing this class
		return scope.Close(result);
    }

        //This will return the c++ object that WrapGameObject stored, 
        //from an existing jsObject. Used in the start callback
    Game* UnwrapGameObject( Handle<Object> jsObject ) 
    {
        Handle<External> pointer = Handle<External>::Cast( jsObject->GetInternalField(0) );
	    return static_cast<Game*>( pointer->Value() );
    }

    int main(int argc, char **argv) 
    {

        Locker locker;
        HandleScope handle_scope;

        Handle<ObjectTemplate> globalTemplate = ObjectTemplate::New();


            //The most obvious form of getting values into script - set a value on the global scope
            //Note that i set the flags to ReadOnly. This prevents 'version = 5;' from overriding
            //this object accidentally or intentionally.
        globalTemplate->Set( String::New("version"), Number::New( 1.1 ), ReadOnly );
        
            //add the print function to the scope too
        exposePrint( globalTemplate );

            //Create our main context, we need to enter it
        Handle<Context> context = Context::New( NULL, globalTemplate );

            //Last example had no need to, as it was not creating anything.
            //If we create a JS object, we need to be in a context.
            //Creating a context scope says that anything within the scope
            //of this - will be created inside the context specified. If
            //you get errors at runtime with  	
                //v8::internal::Top::global_context() 
            //on top of the stack - you are trying to create objects in
            //a non existant context. Enter a context, and then create some.

        Context::Scope context_scope( context );

            //Now that we have a context - we can start injecting stuff into it.
        Game* game = new Game();
            //We will pack this object into a JS object c++ side, so that we can
            //manipulate the script version from here.
        Handle<Object> jsGame = WrapGameObject( game );

            //Now, we can easily expose the start function for js to use
        ExposeProperty( jsGame, String::New("start"), InvocationCallback( Game::gameStart ) );

            //And finally, add the game object with its functions, into the global scope
        context->Global()->Set( String::New("game"), jsGame, ReadOnly );


        if(argc > 1) {

            eScriptExecResult r = executeScript( std::string( argv[1] ) , context );

        } else {

            printf("Usage: <scriptname.js> \n Execute the javascript file.");

        }

        return 0;
    }