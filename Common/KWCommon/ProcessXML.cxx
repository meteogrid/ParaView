/*=========================================================================

  Program:   ParaView
  Module:    ProcessXML.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkObject.h"

#include <vtkstd/string>
#include <vtkstd/vector>
#include "kwsys/SystemTools.hxx"
#include "kwsys/RegularExpression.hxx"

class Output
{
public:
  Output()
    {
    this->MaxLen = 16000;
    this->CurrentPosition = 0;
    }
  ~Output()
    {
    this->Stream.rdbuf()->freeze(0);
    }
  Output(const Output&){}
  void operator=(const Output&){}
  ostrstream Stream;

  int MaxLen;
  long CurrentPosition;
  int Count;
  vtkstd::string Prefix;
  vtkstd::string Suffix;

  void PrintHeader(const char* title, const char* file)
    {
    this->Stream << endl
      << "// From file " << file << endl
      << "static const char* " << this->Prefix.c_str() << title << this->Suffix.c_str() << this->Count 
      << " =" << endl;
    this->CurrentPosition = this->Stream.tellp();
    }

  void CheckSplit(const char* title, const char* file, int force=0)
    {
    if ( (static_cast<long>(this->Stream.tellp()) - this->CurrentPosition) > this->MaxLen ||
      force )
      {
      this->Count ++;
      this->Stream << ";" << endl;
      this->PrintHeader(title, file);
      }
    }

  int ProcessFile(const char* file, const char* title)
    {
    kwsys_ios::ifstream ifs(file);
    if ( !ifs )
      {
      cout << "Canot open file: " << file << endl;
      return 0;
      }
    int ch;
    int in_ifdef = 0;

    this->Count = 0;
    this->PrintHeader(title, file);
    this->Stream << "\"";

    vtkstd::string line;
    vtkstd::string::size_type cc;

    kwsys::RegularExpression reIfDef("^[ \r\n\t]*#[ \r\n\t]*if");
    kwsys::RegularExpression reElse("^[ \r\n\t]*#[ \r\n\t]*el(se|if)");
    kwsys::RegularExpression reEndif("^[ \r\n\t]*#[ \r\n\t]*endif");
    int res = 0;

    while ( kwsys::SystemTools::GetLineFromStream(ifs, line) )
      {
      res ++;
      int regex = 0;
      int ifdef_line = 0;
      if ( reIfDef.find(line) )
        {
        in_ifdef ++;
        regex = 1;
        ifdef_line = 1;
        }
      else if ( reElse.find(line) )
        {
        regex = 1;
        }
      else if ( reEndif.find(line) )
        {
        in_ifdef --;
        regex = 1;
        }
      if ( regex )
        {
        this->Stream << "\\n\"" << endl;
        if ( ifdef_line )
          {
          this->CheckSplit(title, file, 1);
          }
        this->Stream << line.c_str() << endl;
        if ( !ifdef_line )
          {
          this->CheckSplit(title, file);
          }
        this->Stream << "\"";
        }
      else
        {
        for ( cc = 0; cc < line.size(); cc ++ )
          {
          ch = line[cc];
          if ( ch == '\\' )
            {
            this->Stream << "\\\\";
            }
          else if ( ch == '\"' )
            {
            this->Stream << "\\\"";
            }
          else
            {
            this->Stream << (unsigned char)ch;
            }
          }
        this->Stream << "\\n\"" << endl;
        if ( !in_ifdef )
          {
          this->CheckSplit(title, file);
          }
        this->Stream << "\"";
        }
      }
    this->Stream << "\\n\";" << endl;

    if ( !res )
      {
      return 0;
      }
    return this->Count+1;
    }
};

int main(int argc, char* argv[])
{
  if ( argc < 4 )
    {
    cout << "Usage: " << argv[0] << " <output-file> <input-path> <prefix> <suffix> <getmethod> <modules>..." 
      << endl;
    return 1;
    }
  Output ot;
  ot.Prefix = argv[3];
  ot.Stream << "// Loadable modules" << endl
    << "//" << endl
    << "// Generated by " << argv[0] << endl
    << "//" << endl
    << "#ifndef __" << ot.Prefix.c_str() << "_h" << endl
    << "#define __" << ot.Prefix.c_str() << "_h" << endl
    << endl;

  vtkstd::string output = argv[1];
  vtkstd::string input = argv[2];

  int cc;
  for ( cc = 6; cc < argc; cc ++ )
    {
    vtkstd::string fname = input + "/" + argv[cc] + ".xml";

    int num = 0;
    if ( (num = ot.ProcessFile(fname.c_str(), argv[cc])) == 0 )
      {
      cout << "Problem generating header file from XML file: " << fname.c_str() << endl;
      return 1;
      }
    int kk;
  ostrstream createstring;
  ostrstream lenstr;
    for ( kk = 0; kk < num; kk ++ )
      {
      lenstr << endl 
        << "    + strlen(" << ot.Prefix.c_str() 
        << argv[cc] << ot.Suffix.c_str() << kk << ")";
      createstring << "  strcat(res, " << ot.Prefix.c_str() << argv[cc] << ot.Suffix.c_str()
        << kk << ");" << endl;
      }
    ot.Stream
      << "// Get single string" << endl
      << "char* " << ot.Prefix.c_str() << argv[cc] << argv[5] << "()" << endl
      << "{" << endl
      << "  int len = ( 0"
      << lenstr.rdbuf()
      << " );" << endl
      << "  char* res = new char[ len + 1];" << endl
      << "  res[0] = 0;" << endl
      << createstring.rdbuf()
      << "  return res;" << endl
      << "}" << endl << endl;
  }
  
  ot.Stream
    << endl << endl
    << "#endif" << endl;
  ot.Stream << ends;
  FILE* fp = fopen(output.c_str(), "w");
  if ( !fp )
    {
    cout << "Cannot open output file: " << output.c_str() << endl;
    return 1;
    }
  fprintf(fp, "%s", ot.Stream.str());
  fclose(fp);
  return 0;
}
