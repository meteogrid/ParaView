/*=========================================================================

  Program:   ParaView
  Module:    $RCSfile$

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkPVMantaView.h"

#include "vtkCamera.h"
#include "vtkDataRepresentation.h"
#include "vtkMantaCamera.h"
#include "vtkMantaLight.h"
#include "vtkMantaRenderer.h"
#include "vtkObjectFactory.h"
#include "vtkPVAxesWidget.h"
#include "vtkPVGenericRenderWindowInteractor.h"
#include "vtkRenderViewBase.h"

vtkStandardNewMacro(vtkPVMantaView);
//----------------------------------------------------------------------------
vtkPVMantaView::vtkPVMantaView()
{
  vtkMantaRenderer *mantaRenderer = vtkMantaRenderer::New();
  this->RenderView->SetRenderer(mantaRenderer);

  vtkMantaCamera *mantaCamera = vtkMantaCamera::New();
  mantaRenderer->SetActiveCamera(mantaCamera);
  this->NonCompositedRenderer->SetActiveCamera(
    mantaCamera);
  mantaCamera->ParallelProjectionOff();

/*
  vtkMemberFunctionCommand<vtkPVRenderView>* observer =
    vtkMemberFunctionCommand<vtkPVRenderView>::New();
  observer->SetCallback(*this, &vtkPVRenderView::ResetCameraClippingRange);
  this->GetRenderer()->AddObserver(vtkCommand::ResetCameraClippingRangeEvent,
    observer);
  observer->FastDelete();
*/
  mantaRenderer->SetUseDepthPeeling(0);

  this->Light->Delete();
  this->Light = vtkMantaLight::New();
  this->Light->SetAmbientColor(1, 1, 1);
  this->Light->SetSpecularColor(1, 1, 1);
  this->Light->SetDiffuseColor(1, 1, 1);
  this->Light->SetIntensity(1.0);
  this->Light->SetLightType(2); // CameraLight

  //TODO:
  //replace with a manta light kit
//  this->LightKit = NULL;//vtkLightKit::New();
  mantaRenderer->AddLight(this->Light);
  mantaRenderer->SetAutomaticLightCreation(0);

//  this->OrderedCompositingBSPCutsSource = vtkBSPCutsGenerator::New();

  if (this->Interactor)
    {
    this->Interactor->SetRenderer(mantaRenderer);
    }

  this->OrientationWidget->SetParentRenderer(mantaRenderer);

  //this->GetRenderer()->AddActor(this->CenterAxes);

  this->SetInteractionMode(INTERACTION_MODE_3D);

}

//----------------------------------------------------------------------------
vtkPVMantaView::~vtkPVMantaView()
{
}

//----------------------------------------------------------------------------
void vtkPVMantaView::Initialize(unsigned int id)
{
  this->Superclass::Initialize(id);

  //disable multipass rendering so mantarenderer will do old school
  //rendering
  this->RenderView->GetRenderer()->SetPass(NULL);
}

//----------------------------------------------------------------------------
void vtkPVMantaView::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
