/*=========================================================================

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// First include the required header files for the VTK classes we are using.
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkConeSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char *[]) {
    vtkNew<vtkNamedColors> colors;

    //
    // Next we create an instance of vtkConeSource and set some of its
    // properties. The instance of vtkConeSource "cone" is part of a
    // visualization pipeline (it is a source process object); it produces data
    // (output type is vtkPolyData) which other filters may process.
    //
    vtkNew<vtkConeSource> cone;
    cone->SetHeight(3.0);
    cone->SetRadius(1.0);
    cone->SetResolution(10);

    //
    // In this example we terminate the pipeline with a mapper process object.
    // (Intermediate filters such as vtkShrinkPolyData could be inserted in
    // between the source and the mapper.)  We create an instance of
    // vtkPolyDataMapper to map the polygonal data into graphics primitives. We
    // connect the output of the cone source to the input of this mapper.
    //
    vtkNew<vtkPolyDataMapper> coneMapper;
    coneMapper->SetInputConnection(cone->GetOutputPort());

    //
    // Create an actor to represent the cone. The actor orchestrates rendering
    // of the mapper's graphics primitives. An actor also refers to properties
    // via a vtkProperty instance, and includes an internal transformation
    // matrix. We set this actor's mapper to be coneMapper which we created
    // above.
    //
    vtkNew<vtkActor> coneActor;
    coneActor->SetMapper(coneMapper);
    coneActor->GetProperty()->SetColor(colors->GetColor3d("Bisque").GetData());

    //
    // Create the Renderer and assign actors to it. A renderer is like a
    // viewport. It is part or all of a window on the screen and it is
    // responsible for drawing the actors it has.  We also set the background
    // color here.
    //
    vtkNew<vtkRenderer> ren1;
    ren1->AddActor(coneActor);
    ren1->SetBackground(colors->GetColor3d("MidnightBlue").GetData());

    //
    // Finally we create the render window which will show up on the screen.
    // We put our renderer into the render window using AddRenderer. We also
    // set the size to be 300 pixels by 300.
    //
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(ren1);
    renWin->SetSize(300, 300);
    renWin->SetWindowName("Tutorial_Step5");

    //
    // The vtkRenderWindowInteractor class watches for events (e.g., keypress,
    // mouse) in the vtkRenderWindow. These events are translated into
    // event invocations that VTK understands (see VTK/Common/vtkCommand.h
    // for all events that VTK processes). Then observers of these VTK
    // events can process them as appropriate.
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    //
    // By default the vtkRenderWindowInteractor instantiates an instance
    // of vtkInteractorStyle. vtkInteractorStyle translates a set of events
    // it observes into operations on the camera, actors, and/or properties
    // in the vtkRenderWindow associated with the vtkRenderWinodwInteractor.
    // Here we specify a particular interactor style.
    vtkNew<vtkInteractorStyleTrackballCamera> style;
    iren->SetInteractorStyle(style);

    //
    // Unlike the previous scripts where we performed some operations and then
    // exited, here we leave an event loop running. The user can use the mouse
    // and keyboard to perform the operations on the scene according to the
    // current interaction style. When the user presses the "e" key, by default
    // an ExitEvent is invoked by the vtkRenderWindowInteractor which is caught
    // and drops out of the event loop (triggered by the Start() method that
    // follows.
    //
    iren->Initialize();
    iren->Start();

    //
    // Final note: recall that observers can watch for particular events and
    // take appropriate action. Pressing "u" in the render window causes the
    // vtkRenderWindowInteractor to invoke a UserEvent. This can be caught to
    // popup a GUI, etc. See the Tcl Cone5.tcl example for an idea of how this
    // works.

    return EXIT_SUCCESS;
}