set (__private_dependencies)
if (PARAVIEW_ENABLE_PYTHON)
  list(APPEND __private_dependencies vtkPythonInterpreter)
endif()

vtk_module(vtkPVServerImplementationCore
  GROUPS
    ParaViewCore
  DEPENDS
    vtkPVClientServerCoreCore
    vtkprotobuf
  PRIVATE_DEPENDS
    vtksys
    ${__private_dependencies}
  TEST_LABELS
    PARAVIEW
)
