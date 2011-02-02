

    /*

    01 - Command line script execution
        
        A simple primer to understanding the v8 workflow. This is not meant
        to do much more than print a result of a script execution. But, it 
        does demonstrate how to create a global template, a context, and 
        how the script file makes it's way from disk into the context.
        
        The scope of tutorial 1 is not to dig into those details, but a method
        of getting v8 running within minutes for you to mess around with.

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
    using namespace v8;


    int main(int argc, char **argv) 
    {
            //Quite simple, these are scope based 'managers'.
            //One protects threading issues, and one manages
            //the creation of JS handles, for clean up.

        Locker locker;
        HandleScope handle_scope;

            //Create a global object template. This can be stamped
            //repeatedly with little cost into new execution contexts.
            //Note the word template! It is a 'definition' of the context.
            //This is important to get - when you expose something into the
            //global object - it is an instance. When you expose something
            //onto the global template - ALL contexts can share the same 
            //exposed values without needing to redo the expose process.

        Handle<ObjectTemplate> globalTemplate = ObjectTemplate::New();

            //A context is a fresh execution context. Take for example, 
            //a browser tab or frame in Google Chrome. Each of these are
            //a new context, stamped with a global template, and created
            //on demand. When executing scripts, you can execute them in 
            //and existing context by using the Context::context_scope 
            //handlers. This 'switches' context for that execution period
            
            //In this case, we only need one new context.
        Handle<Context> context = Context::New( NULL, globalTemplate );

            //If we receive an argument, execute the script file

        if(argc > 1) {

            eScriptExecResult r = executeScript( std::string( argv[1] ) , context );

        } else {

            printf("Usage: <scriptname.js> \n Execute the javascript file.");

        }

        return 0;
    }