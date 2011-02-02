	
#pragma once
    
    #include "../v8/v8.h"
    using namespace v8;

    enum eScriptExecResult {
		eSCRIPT_ERROR_UNKNOWN = 0,
		eSCRIPT_ERROR_NOT_FOUND,
		eSCRIPT_ERROR_EMPTY_SOURCE,
		eSCRIPT_ERROR_COMPILE_FAILED,
		eSCRIPT_ERROR_NONE,
		eSCRIPT_ERROR_COUNT
	};


     /* Return a file size from a FILE pointer */
    const int getFileSize( FILE *f ) 
    {
        if(f == NULL) return -1;
        int size = -1;

        fseek(f, 0, SEEK_END);
         size = ftell(f);
		rewind(f);

        return size;
    }

        /* Function to print errors to the console */
    void reportException( TryCatch* try_catch ) 
    {
        Locker lock;
        HandleScope handle_scope;

            //Get a string from the error message and exception detail
        String::Utf8Value exception( try_catch->Exception() );
        Handle<Message> message = try_catch->Message();

            //This error has no message
        if ( message.IsEmpty() ) 
        {
            printf("%s\n" , *exception );
            return;
        }
		   
        char ex[1024];
        String::Utf8Value filename( message->GetScriptResourceName() );
        int linenum = message->GetLineNumber();

            // Print (filename):(line number): (message).
		sprintf( ex , "%s:%i: %s\n", *filename , linenum , *exception );
        printf(ex);

		    // Print line of source code.
		String::Utf8Value sourceline( message->GetSourceLine() );
		printf( "%s\n", *sourceline );

	}

        /*  readFile from the v8 samples. 
            Returns a v8::String from a file. */
    const Handle<String> readFile( const std::string &str ) 
    {
            //Handle scope explained later,
            //simply collects all v8::Handle items.
        HandleScope scope;

        FILE* file = fopen( str.c_str() , "rb" );
        if (file == NULL) return v8::Handle<v8::String>();

	    int size = getFileSize( file );

            //Null terminate, and load the file into
            //memory, so we can return a v8::String

		  char* chars = new char[size + 1]; 
		   chars[size] = '\0';

		  for (int i = 0; i < size;) 
          {
			int read = fread( &chars[i] , 1, size - i, file);
			i += read;
		  }

		  fclose(file);
           Handle<String> result = v8::String::New(chars, size);
		  delete[] chars;

            //This cleans the handles up, but reserves the result
            //for the caller to use as it wishes.
          return scope.Close( result );
    }


        /* Execute a specific piece of text in the execution context specified */
    bool executeString( Handle<String> source, Handle<Value> name, const Handle<Context> &executionContext ) 
        {
		  Locker lock;
          HandleScope handle_scope;

          if( source->Length() == 0 ) return false;

            //Switch to the context we want to execute in
		  Context::Scope context_scope( executionContext );

            //Try to compile the script code
		  TryCatch try_catch;
		  Handle<Script> script = Script::Compile( source, name );

            //If the script is empty, there were compile errors.
		  if ( script.IsEmpty() ) 
          {

            reportException( &try_catch );
			return false;

		  } 
          else 
          {
                //So if compilation succeeds, execute it.
			Handle<Value> result = script->Run();

                //If the results are empty, there was a runtime
                //error, so we can report these errors.
			if ( result.IsEmpty() ) 
            {
				reportException( &try_catch );
			    return false;
			} 
            else 
            {
                //If there is a result, print it to the console
			  if ( !result->IsUndefined() && !result.IsEmpty() ) 
              {
                        //Convert the results to string
                    v8::String::Utf8Value str( result );
				    printf("%s\n", *str);
			  }

                //Done
			  return true;
			}
		  }
		}


        /* Execute the script by filename in the execution context specified */
    const eScriptExecResult executeScript( const std::string &filename, const Handle<Context> &executionContext )
	{
            //A v8 Locker allows multithreading 
            //in the debuggers, and elsewhere to work.
		Locker lock;
            //A handle scope for collecting handles
        HandleScope handle_scope;
            
        //The source code of this file.
        v8::Handle<v8::String> source;

            //Fetch the file 
        source = readFile( filename.c_str() );

            //No data in the file.
		if( source.IsEmpty() ) return eSCRIPT_ERROR_EMPTY_SOURCE;

            //Return compilation error
        if( !executeString( source, String::New( filename.c_str() ), executionContext )) return eSCRIPT_ERROR_COMPILE_FAILED;

            //Succesfully executed
        return eSCRIPT_ERROR_NONE;
	}