#include <iostream>
#include <vector>
#include <stack>
#include <cassert>
using namespace std;


struct tokRslt
{
  bool success;
    vector < string > syms;
};

struct tokRslt tokenize (string s)
{
  vector < string > fleep;
  struct tokRslt bleep;
  while (s.length () > 0)
    {
      switch (s[0])
	{
	case ' ':
	  s.erase (0, 1);
	  break;

	case 'T':
	  fleep.push_back ("T");
	  s.erase (0, 1);
	  break;

	case 'F':
	  fleep.push_back ("F");
	  s.erase (0, 1);
	  break;

	case '^':
	  fleep.push_back ("^");
	  s.erase (0, 1);
	  break;

	case 'v':
	  fleep.push_back ("v");
	  s.erase (0, 1);
	  break;

	case '~':
	  fleep.push_back ("~");
	  s.erase (0, 1);
	  break;

	case '=':
	  if (s.length () >= 2 && s[1] == '>')
	    {
	      fleep.push_back ("=>");
	      s.erase (0, 2);
	      break;
	    }
	  else
	    {
	      bleep.success = false;
	      return bleep;
	    }
	case '<':
	  if (s.length () >= 3 && s[1] == '=' && s[2] == '>')
	    {
	      fleep.push_back ("<=>");
	      s.erase (0, 3);
	      break;
	    }
	  else
	    {
	      bleep.success = false;
	      return bleep;
	    }
	case '(':
	  fleep.push_back ("(");
	  s.erase (0, 1);
	  break;

	case ')':
	  fleep.push_back (")");
	  s.erase (0, 1);
	  continue;
	default:
	  bleep.success = false;
	  return bleep;
	}
    }
  bleep.syms = fleep;
  bleep.success = true;
  return bleep;

}

typedef struct AST *pNODE;
struct AST
{
  string info;
  pNODE children[2];
};



struct parseRslt
{
  bool success;
  struct AST ast;
};

int
getOperatorPrec (string s)
{
  if (s == "~")
    return 100;
  if (s == "^")
    return 99;
  if (s == "v")
    return 98;
  if (s == "=>")
    return 97;
  if (s == "<=>")
    return 96;
  return -99999;
}

bool isOp (string s)
{
  if (s == "~")
    return true;
  if (s == "^")
    return true;
  if (s == "v")
    return true;
  if (s == "=>")
    return true;
  if (s == "<=>")
    return true;
  return false;
}

bool
isLeft (string s)
{
  if (s == "^" || s == "v")
    return true;
  return false;
}


struct AST *
putIntoAST (vector < string > &foo, bool &fubar)
{
  struct AST *flangus = new struct AST;
  flangus->children[0] = NULL;
  flangus->children[1] = NULL;
  if (fubar)
    {
      return flangus;
    }
  if (isOp (foo.back ()))
    {
      if (foo.back () == "~")
	{
	  flangus->info = "~";
	  foo.pop_back ();
	  if (foo.size () < 1)
	    {
	      fubar = true;
	      return flangus;
	    }
	  flangus->children[0] = putIntoAST (foo, fubar);
	  return flangus;
	}
      else
	{
	  flangus->info = foo.back ();
	  foo.pop_back ();
	  if (foo.size () < 2)
	    {
	      fubar = true;
	      return flangus;
	    }
	  flangus->children[1] = putIntoAST (foo, fubar);
	  if (foo.size () < 1)
	    {
	      fubar = true;
	      return flangus;
	    }
	  flangus->children[0] = putIntoAST (foo, fubar);
	  return flangus;
	}
    }
  else
    {
      flangus->info = foo.back ();
      foo.pop_back ();
      return flangus;
    }
}


struct parseRslt
parse (vector < string > V)
{
  vector < string > flangus; //output queue
  stack < string > yeah;//operator stack
  int i = 0;
  
  if (V.size () == 0)
    {
      struct parseRslt bleep;
      bleep.success = false;
      return bleep;
    }
    //Shunting yard algorithm
  while (i < V.size ())
    {
      string fleep = V[i];
      if (fleep == "T" || fleep == "F")
	    {
	        flangus.push_back (fleep);
	        i++;
	    }
      else if (isOp (fleep))
	{

	  while (!yeah.empty () &&
		 isOp (yeah.top ()) &&
		 (getOperatorPrec (fleep) <= getOperatorPrec (yeah.top ()) &&
		 isLeft (fleep) || getOperatorPrec(fleep) < getOperatorPrec(yeah.top())) )
	    {
	      flangus.push_back (yeah.top ());
	      yeah.pop ();
	    }
	  yeah.push (fleep);
	  i++;
	}
      else if (fleep == "(")
	{
	  yeah.push (fleep);
	  i++;
	}
      else if (fleep == ")")
	{
	  i++;
	  bool foundit = false;
	  while (!yeah.empty () && yeah.top () != "(")
	    {
	      flangus.push_back (yeah.top ());
	      yeah.pop ();
	    }
	  if (!yeah.empty () && yeah.top () == "(")
	    {
	      foundit = true;
	      yeah.pop ();
	    }

	  if (!foundit)
	    {
	      struct parseRslt qwertyuiop;
	      qwertyuiop.success = false;
	      return qwertyuiop;
	    }
	}
    }

  while (!yeah.empty ())
    {
      string thing = yeah.top();
      if (thing == "(") {
        struct parseRslt qwertyuiop;
        qwertyuiop.success = false;
        return qwertyuiop;
      }
      flangus.push_back (yeah.top ());
      yeah.pop ();
    }
  struct parseRslt qwertyuiop;
  bool fubar = false;

  qwertyuiop.ast = *putIntoAST (flangus, fubar);
  qwertyuiop.success = !fubar && flangus.size() == 0;
  return qwertyuiop;


}


bool
eval (AST T)
{
  string blarp = T.info;
  if (blarp == "~")
    {
      return !eval (*T.children[0]);
    }
  else if (blarp == "T")
    {
      return true;
    }
  else if (blarp == "F")
    {
      return false;
    }
  else if (blarp == "v")
    {
      return eval (*T.children[1]) || eval (*T.children[0]);
    }
  else if (blarp == "^")
    {
      return eval (*T.children[1]) && eval (*T.children[0]);
    }
  else if (blarp == "=>")
    {
      return !eval (*T.children[0]) || eval (*T.children[1]);
    }
  else if (blarp == "<=>")
    {
      return eval (*T.children[1]) == eval (*T.children[0]);
    }
  else
    {
      
      cout << "I HAVE NO MOUTH\n";
      cout << blarp;
      cout << "\nAND I MUST SCREAM\n";
      
      return 3;
    }
}

struct TPERslt
{
  bool val;
  string msg;
};



struct TPERslt
TPE (string s)
{
  struct TPERslt flangus;
  struct tokRslt qwer = tokenize (s);
  if (!qwer.success)
    {
      flangus.msg = "symbol error";
      return flangus;
    }
  struct parseRslt bleep = parse (qwer.syms);
  if (!bleep.success)
    {
      flangus.msg = "grammar error";
      return flangus;
    }
  flangus.msg = "success";
  flangus.val = eval (bleep.ast);
  return flangus;
}


string
TPEOut (string s)
{
  struct TPERslt flarp = TPE (s);
  if (flarp.msg == "success")
    {
      if (flarp.val)
	{
	  return "true";
	}
      else
	{
	  return "false";
	}
    }
  else
    {
      return flarp.msg;
    }
}







int
main ()
{
  
  
  
  string fleep;
  while(1) {
    cout << "Give an input(enter 'stop' to exit)\n";
    getline(cin, fleep);
    if (fleep == "stop") {
      break;
    }
    cout << TPEOut(fleep);
    cout << std::endl;
    
  } 
return 0;
}