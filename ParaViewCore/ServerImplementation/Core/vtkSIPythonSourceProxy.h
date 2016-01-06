/*=========================================================================

  Program:   ParaView
  Module:    vtkSIPythonSourceProxy.h

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkSIPythonSourceProxy
// .SECTION Description
//

#ifndef vtkSIPythonSourceProxy_h
#define vtkSIPythonSourceProxy_h

#include "vtkSISourceProxy.h"

class VTKPVSERVERIMPLEMENTATIONCORE_EXPORT vtkSIPythonSourceProxy : public vtkSISourceProxy
{
public:
  static vtkSIPythonSourceProxy* New();
  vtkTypeMacro(vtkSIPythonSourceProxy, vtkSISourceProxy);
  void PrintSelf(ostream& os, vtkIndent indent);

//BTX
protected:
  vtkSIPythonSourceProxy();
  ~vtkSIPythonSourceProxy();

  // Description:
  // Overridden to use the Python interpreter to instantiate Python classes.
  virtual vtkObjectBase* NewVTKObject(const char* className);

private:
  vtkSIPythonSourceProxy(const vtkSIPythonSourceProxy&); // Not implemented
  void operator=(const vtkSIPythonSourceProxy&); // Not implemented

  class vtkInternals;
  vtkInternals* Internals;
//ETX
};

#endif
