/*=========================================================================

  Program:   ParaView
  Module:    vtkSIPythonSourceProxy.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkPython.h"

#include "vtkSIPythonSourceProxy.h"

#include "vtkObjectFactory.h"
#include "vtkPythonInterpreter.h"
#include "vtkPythonUtil.h"
#include "vtkSmartPyObject.h"

#include <vtksys/SystemTools.hxx>

class vtkSIPythonSourceProxy::vtkInternals
{
public:
  vtkSmartPyObject Module;
  vtkSmartPyObject VTKObject;
};

vtkStandardNewMacro(vtkSIPythonSourceProxy);
//----------------------------------------------------------------------------
vtkSIPythonSourceProxy::vtkSIPythonSourceProxy()
  : Internals(new vtkSIPythonSourceProxy::vtkInternals())
{
}

//----------------------------------------------------------------------------
vtkSIPythonSourceProxy::~vtkSIPythonSourceProxy()
{
  delete this->Internals;
  this->Internals = NULL;
}

//----------------------------------------------------------------------------
class vtkPythonScopeGilEnsurer
{
public:
   vtkPythonScopeGilEnsurer() : GilState(PyGILState_Ensure()) {}
   ~vtkPythonScopeGilEnsurer()
  {
    PyGILState_Release(GilState);
  }
private:
  PyGILState_STATE GilState;
};

//----------------------------------------------------------------------------
vtkObjectBase* vtkSIPythonSourceProxy::NewVTKObject(const char* className)
{
  vtkPythonInterpreter::Initialize();
  std::string module = vtksys::SystemTools::GetFilenameWithoutLastExtension(className);
  std::string classname = vtksys::SystemTools::GetFilenameLastExtension(className);
  // remove the leading ".".
  classname.erase(classname.begin());

  vtkInternals& internals = (*this->Internals);

  vtkPythonScopeGilEnsurer gilEnsurer;
  internals.Module.TakeReference(PyImport_ImportModule(module.c_str()));
  if (!internals.Module)
    {
    vtkErrorMacro("Failed to import module '" << module.c_str() << "'.");
    // FIXME: how do we handle this?
    return this->Superclass::NewVTKObject("vtkPythonAlgorithm");
    }

  vtkSmartPyObject classPyObject;
  classPyObject.TakeReference(PyObject_GetAttrString(internals.Module, classname.c_str()));
  if (!classPyObject)
    {
    vtkErrorMacro("Failed to locate class '" << classname.c_str() << "' in module '"
      << module.c_str() << "'.");
    // FIXME: how do we handle this?
    return this->Superclass::NewVTKObject("vtkPythonAlgorithm");
    }

  internals.VTKObject.TakeReference(PyObject_CallObject(classPyObject, NULL));
  if (!internals.VTKObject)
    {
    vtkErrorMacro("Failed to create object '"<< className << "'.");
    // FIXME: how do we handle this?
    return this->Superclass::NewVTKObject("vtkPythonAlgorithm");
    }

  return vtkPythonUtil::GetPointerFromObject(internals.VTKObject, "vtkAlgorithm");
}

//----------------------------------------------------------------------------
void vtkSIPythonSourceProxy::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
